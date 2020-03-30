#pragma once
#include <thread>
#include "headers/sequence_dispatcher.h"
#include "headers/json_parser.h"
#include "telegram_structs.h"
#include "utility/trie.h"

namespace telegram {

using update_callback = std::function<void(Update &&)>;
using msg_callback = std::function<void(Message &&)>;
using query_callback = std::function<void(CallbackQuery &&)>;
using inline_callback = std::function<void(InlineQuery &&)>;
using chosen_inline_callback = std::function<void(ChosenInlineResult &&)>;
using callbacks = std::variant<msg_callback, query_callback, inline_callback,
chosen_inline_callback>;

class update_manager {
private:
    update_callback callback;
    Trie<msg_callback> msg_callbacks;
    Trie<query_callback> query_callbacks;
    Trie<inline_callback> inline_callbacks;
    Trie<chosen_inline_callback> chosen_callbacks;

    std::unordered_map<int64_t, std::shared_ptr<sequence<msg_callback>>>
    dispatcher;

    size_t last_update = 0;
public:
    update_manager() {}
    void set_raw_callback(update_callback &&cb) { callback.swap(cb); }
    void add_sequence(int64_t user_id,
                      std::shared_ptr<sequence<msg_callback>> callback) {
        dispatcher[user_id] = callback;
    }
    void remove_sequence(int64_t user_id) { dispatcher.erase(user_id); }
    size_t get_offset() const noexcept { return last_update; }
    void set_offset(size_t offset) { last_update = offset; }
    void add_callback(std::string_view cmd, callbacks &&cb) {
        std::visit(
                    [&](auto &&callback) {
            using cb_type = std::decay_t<decltype(callback)>;
            if constexpr (std::is_same_v<cb_type, msg_callback>) {
                msg_callbacks.insert(cmd, callback);
            } else if constexpr (std::is_same_v<cb_type, query_callback>) {
                query_callbacks.insert(cmd, callback);
            } else if constexpr (std::is_same_v<cb_type, inline_callback>) {
                inline_callbacks.insert(cmd, callback);
            } else if constexpr (std::is_same_v<cb_type,
                    chosen_inline_callback>) {
                chosen_callbacks.insert(cmd, callback);
            } else {
                static_assert("Undefined callback type");
            }
        },
        cb);
    }
    template <class T>
    bool run_callback(std::string_view cmd, const std::string &data) {
        using cb_type = T;
        if constexpr (std::is_same_v<cb_type, msg_callback>) {
            auto result = msg_callbacks.find(cmd);
            if (result) {
                std::thread(result, from_json<Message>(data)).detach();
                return true;
            } else
                utility::logger::info("'message callback' not found for ",cmd," command");

        } else if constexpr (std::is_same_v<cb_type, query_callback>) {
            auto result = query_callbacks.find(cmd);
            if (result) {
                std::thread(result, from_json<CallbackQuery>(data)).detach();
                return true;
            } else
                utility::logger::info("'query callback' not found for ",cmd," command");

        } else if constexpr (std::is_same_v<cb_type, inline_callback>) {
            auto result = inline_callbacks.find(cmd);
            if (result) {
                std::thread(result, from_json<InlineQuery>(data)).detach();
                return true;
            } else
                utility::logger::info("'inline callback' not found for ",cmd," command");

        } else if constexpr (std::is_same_v<cb_type, chosen_inline_callback>) {
            auto result = chosen_callbacks.find(cmd);
            if (result) {
                std::thread(result, from_json<ChosenInlineResult>(data)).detach();
                return true;
            } else
                utility::logger::info("'chosen inline result' callback not found for ",cmd," command");
        } else if constexpr (std::is_same_v<cb_type, update_callback>) {
            if (callback) {
                std::thread(callback, from_json<Update>(data)).detach();
                return true;
            }
        }
        return false;
    }
    template <class T> void remove_callback(std::string_view cmd) {
        if constexpr (std::is_same_v<T, msg_callback>) {
            msg_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, query_callback>) {
            query_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, inline_callback>) {
            inline_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, chosen_inline_callback>) {
            chosen_callbacks.erase(cmd);
        } else {
            static_assert("Undefined callback type");
        }
    }
    void route_callback(const std::string &str) {
        rapidjson::Document doc;
        const auto &ok = doc.Parse(str.data());
        if (ok.HasParseError()) {
            utility::logger::warn("Document parse error. \nRapidjson Error Code: ",
                                  ok.GetParseError(),"\nOffset: ",ok.GetErrorOffset(),'\n');
            return;
        }
        if (doc.HasMember("ok") && !doc["ok"].GetBool()) {
            utility::logger::warn(std::string("Error: ") + doc["description"].GetString() + '\n');
            return;
        }
        auto callback_router = [this](const auto &it) {
            if (it.HasMember("callback_query")) {
                auto obj = it["callback_query"].GetObject();
                auto data = obj["data"].GetString();

                if (run_callback<query_callback>(data, utility::objectToJson(obj)))
                    return;
            } else if (it.HasMember("inline_query")) {
                auto obj = it["inline_query"].GetObject();
                auto data = obj["query"].GetString();

                if (run_callback<inline_callback>(data, utility::objectToJson(obj)))
                    return;
            } else if (it.HasMember("chosen_inline_result")) {
                auto obj = it["chosen_inline_result"].GetObject();
                auto data = obj["query"].GetString();

                if (run_callback<chosen_inline_callback>(data,
                                                         utility::objectToJson(obj)))
                    return;
            } else if (it.HasMember("message") &&
                       it["message"].GetObject().HasMember("text")) {
                if (dispatcher.size()) {
                    const auto &message = it["message"].GetObject();
                    if (auto result =
                            dispatcher.find(message["from"].GetObject()["id"].GetInt64());
                            result != dispatcher.end()) {
                        if (result->second->finished()) {
                            dispatcher.erase(result);
                        } else {
                            std::thread(&sequence<msg_callback>::input<Message>,
                                        std::ref(*result->second),
                                        from_json<Message>(utility::objectToJson(message)))
                                    .detach();
                            return;
                        }
                    }
                }
                if (msg_callbacks.size()) {
                    std::string_view text = it["message"].GetObject()["text"].GetString();
                    size_t firstSpace = text.find_first_of(' ');
                    std::string_view cmd(text.data(), firstSpace == std::string::npos
                                         ? text.size()
                                         : firstSpace);
                    if (msg_callbacks.find(cmd) &&
                            (run_callback<msg_callback>(
                                 cmd, utility::objectToJson(it["message"].GetObject()))))
                        return;
                }
            }
            run_callback<update_callback>({}, utility::objectToJson(it));
        };

        if (auto has_result = doc.HasMember("result");
                (has_result && doc["result"].IsArray()) || doc.IsArray()) {
            const auto &updates_arr =
                    has_result ? doc["result"].GetArray() : doc.GetArray();
            if (updates_arr.Size())
                last_update = updates_arr[updates_arr.Size() - 1]
                        .GetObject()["update_id"]
                        .GetUint64() + 1;

            for (auto &&it : updates_arr) {
                callback_router(it.GetObject());
            }
        } else if ((has_result && doc["result"].IsObject()) ||
                   (!has_result && doc.IsObject())) {
            const auto &update =
                    !has_result ? doc.GetObject() : doc["result"].GetObject();
            last_update = update["update_id"].GetUint64() + 1;
            callback_router(update);
        } else {
            utility::logger::info("Json document does not contain any parsable value");
            return;
        }
    }
};
} // namespace telegram
