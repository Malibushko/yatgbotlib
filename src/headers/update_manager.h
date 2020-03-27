#pragma once
#include <functional>
#include <thread>
#include "rapidjson/document.h"
#include "../include/telegram_structs.h"
#include "headers/json_parser.h"
#include "headers/sequence_dispatcher.h"
#include "utility/trie.h"
#include "utility/utility.h"

namespace telegram {

using update_callback = std::function<void(Update&&)>;
using msg_callback = std::function<void(Message&&)>;
using query_callback = std::function<void(CallbackQuery&&)>;
using inline_callback = std::function<void(InlineQuery&&)>;
using chosen_inline_callback = std::function<void(ChosenInlineResult&&)>;
using callbacks = std::variant<msg_callback,query_callback,
                                inline_callback,chosen_inline_callback>;
class UpdateManager {
private:
    update_callback callback;
    Trie<msg_callback> msg_callbacks;
    Trie<query_callback> query_callbacks;
    Trie<inline_callback> inline_callbacks;
    Trie<chosen_inline_callback> chosen_callbacks;
    size_t last_update = 0;

    std::unordered_map<int64_t,std::shared_ptr<Sequence<msg_callback>>> dispatcher;
public:
    UpdateManager() {}
    void setRawCallback(update_callback && cb) {
        callback.swap(cb);
    }
    void addSequence(int64_t user_id,std::shared_ptr<Sequence<msg_callback>> callback) {
        dispatcher[user_id] = callback;
    }
    void removeSequence(int64_t user_id) {
        dispatcher.erase(user_id);
    }
    size_t getUpdateOffset() const noexcept{
        return last_update;
    }
    void setUpdateOffset(size_t offset) {
        last_update = offset;
    }
    void addCallback(std::string_view cmd,callbacks&& cb) {
        std::visit([&](auto && callback){
            using cb_type = std::decay_t<decltype (callback)>;
            if constexpr (std::is_same_v<cb_type, msg_callback>) {
                msg_callbacks.insert(cmd,callback);
            } else if constexpr (std::is_same_v<cb_type, query_callback>) {
                query_callbacks.insert(cmd,callback);
            } else if constexpr (std::is_same_v<cb_type, inline_callback>) {
                inline_callbacks.insert(cmd,callback);
            } else if constexpr (std::is_same_v<cb_type, chosen_inline_callback>) {
                chosen_callbacks.insert(cmd,callback);
            } else {
                static_assert ("Undefined callback type");
            }
        },cb);
    }
    template<class T>
    bool runCallback(std::string_view cmd,const std::string& data) {
            using cb_type = T;
            if constexpr (std::is_same_v<cb_type, msg_callback>) {
                auto result = msg_callbacks.find(cmd);
                if (result) {
                    std::thread(result,from_json<Message>(data)).detach();
                    return true;
                }
                else
                    std::cerr << "'message callback' not found for " << cmd << " command";

            } else if constexpr (std::is_same_v<cb_type, query_callback>) {
                auto result = query_callbacks.find(cmd);
                if (result) {
                    std::thread(result,from_json<CallbackQuery>(data)).detach();
                    return true;
                }
                else
                    std::cerr << "'query callback' not found for " << cmd << " command";

            } else if constexpr (std::is_same_v<cb_type, inline_callback>) {
                auto result = inline_callbacks.find(cmd);
                if (result) {
                    std::thread(result,from_json<InlineQuery>(data)).detach();
                    return true;
                }
                else
                    std::cerr << "'inline callback' not found for " << cmd << " command";

            } else if constexpr (std::is_same_v<cb_type, chosen_inline_callback>) {
                auto result = chosen_callbacks.find(cmd);
                if (result) {
                    std::thread(result,from_json<ChosenInlineResult>(data)).detach();
                    return true;
                }
                else
                    std::cerr << "'chosen inline result' callback not found for " << cmd << " command";
            } else if constexpr (std::is_same_v<cb_type, update_callback>){
                if (callback) {
                    std::thread(callback,from_json<Update>(data)).detach();
                    return true;
                }
            }
            return false;
    }
    template<class T>
    void removeCallback(std::string_view cmd) {
        if constexpr (std::is_same_v<T, msg_callback>) {
            msg_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, query_callback>) {
            query_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, inline_callback>) {
            inline_callbacks.erase(cmd);
        } else if constexpr (std::is_same_v<T, chosen_inline_callback>) {
            chosen_callbacks.erase(cmd);
        } else {
            static_assert ("Undefined callback type");
        }
    }
    void routeCallback(const std::string& str)  {
        rapidjson::Document doc;
        const auto& ok = doc.Parse(str.data());
        if (ok.HasParseError()) {
            std::cerr << "Document parse error. \nRapidjson Error Code: "<<ok.GetParseError()
                      << "\nOffset: " << ok.GetErrorOffset() << "\n";
            return;
        }
        if (doc.HasMember("ok") && !doc["ok"].GetBool()) {
            std::cerr << "Error: " << doc["description"].GetString() << std::endl;
            return;
        }
        auto callback_router = [this](const auto& it){
            if (it.HasMember("callback_query")) {
                auto obj = it["callback_query"].GetObject();
                auto data = obj["data"].GetString();

                if (runCallback<query_callback>(data,
                            utility::objectToJson(obj)))
                    return;
            }
            else if (it.HasMember("inline_query")) {
                auto obj = it["inline_query"].GetObject();
                auto data = obj["query"].GetString();

                if (runCallback<inline_callback>(data,
                            utility::objectToJson(obj)))
                    return;
            }
            else if (it.HasMember("chosen_inline_result")) {
                auto obj = it["chosen_inline_result"].GetObject();
                auto data = obj["query"].GetString();

                if (runCallback<chosen_inline_callback>(data,utility::objectToJson(obj)))
                    return;
            }
            else if (it.HasMember("message") && it["message"].GetObject().HasMember("text")) {
                if (dispatcher.size()) {
                    const auto & message = it["message"].GetObject();
                    if (auto result = dispatcher.find(message["from"].GetObject()["id"].GetInt64());
                            result != dispatcher.end()) {
                        if (result->second->isFinished()) {
                            dispatcher.erase(result);
                        }
                        else {
                            std::thread(&Sequence<msg_callback>::input<Message>,
                                        std::ref(*result->second),
                                        from_json<Message>(utility::objectToJson(message))).detach();
                            return;
                        }
                    }
                }
                if (msg_callbacks.getSize()) {
                    std::string_view text = it["message"].GetObject()["text"].GetString();
                    size_t firstSpace = text.find_first_of(' ');
                    std::string_view cmd(text.data(),firstSpace == std::string::npos ? text.size() : firstSpace);
                    if (msg_callbacks.find(cmd) && (runCallback<msg_callback>(cmd,
                                utility::objectToJson(it["message"].GetObject()))))
                        return;
                }
            }
               runCallback<update_callback>({},utility::objectToJson(it));
        };


        if (auto has_result = doc.HasMember("result");(has_result && doc["result"].IsArray()) || doc.IsArray()) {
            const auto& updates_arr = has_result ? doc["result"].GetArray() : doc.GetArray();
            if (updates_arr.Size())
                last_update = updates_arr[updates_arr.Size()-1].GetObject()["update_id"].GetUint64() + 1;

            for (auto&& it : updates_arr) {
                callback_router(it.GetObject());
            }
        } else if ((has_result && doc["result"].IsObject()) || (!has_result && doc.IsObject())) {
                const auto & update = !has_result ? doc.GetObject() : doc["result"].GetObject();
                last_update = update["update_id"].GetUint64() + 1;
                callback_router(update);
        }
        else {
            std::cerr << "Json document does not contain any parsable value";
            return;
        }
    }
};
}
