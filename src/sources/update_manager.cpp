#include "headers/update_manager.h"
#include "headers/sequence_dispatcher.h"

using namespace telegram;

void update_manager::set_update_callback(update_callback &&cb) {
    callback.swap(cb);
}

void update_manager::add_sequence(int64_t user_id,
                  std::shared_ptr<sequence<msg_callback>> callback) {
    dispatcher[user_id] = callback;
}
void update_manager::remove_sequence(int64_t user_id) {
    dispatcher.erase(user_id);
}
size_t update_manager::get_offset() const noexcept {
    return last_update;
}
void update_manager::add_callback(std::string_view cmd, telegram::callbacks &&callback) {
    m_callbacks.insert(cmd,callback);
}

void update_manager::route_callback(const std::string &str) {
    rapidjson::Document doc;
    const auto &ok = doc.Parse(str.data());
    if (ok.HasParseError()) {
        utility::logger::warn("Document parse error. \nRapidjson Error Code: ",
                              ok.GetParseError(),"\nOffset: ",ok.GetErrorOffset(),'\n');
        return;
    }
    if (doc.HasMember("ok") && !doc["ok"].GetBool()) {
        utility::logger::warn("Error: ",doc["description"].GetString(),'\n');
        return;
    }
    if (dispatcher.size()) {
        const auto &message = doc["message"].GetObject();
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
            }
            return;
        }
    }
    auto callback_router = [this](const auto &it) {
        if (it.HasMember("callback_query")) {
            auto obj = it["callback_query"].GetObject();
            auto data = obj["data"].GetString();

            if (find_callback<query_callback>(data) &&
                    run_callback<query_callback>(data, utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("inline_query")) {
            auto obj = it["inline_query"].GetObject();
            auto data = obj["query"].GetString();

            if (find_callback<inline_callback>(data) &&
                    run_callback<inline_callback>(data, utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("chosen_inline_result")) {
            auto obj = it["chosen_inline_result"].GetObject();
            auto data = obj["query"].GetString();

            if (find_callback<chosen_inline_callback>(data) &&
                    run_callback<chosen_inline_callback>(data,
                                                     utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("message") &&
                   it["message"].GetObject().HasMember("text")) {
            auto obj = it["message"].GetObject();
            auto data = obj["text"].GetString();

            if (find_callback<msg_callback>(data) &&
                    run_callback<msg_callback>(data, utility::objectToJson(obj)))
                return;
        }
        else if (callback)
            std::thread(callback,from_json<Update>(utility::objectToJson(it))).detach();
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
        utility::logger::warn("Json document does not contain any parsable value");
        return;
    }
}
void update_manager::set_offset(size_t offset) {
    last_update = offset;
}
