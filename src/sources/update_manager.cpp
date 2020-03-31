#include "headers/update_manager.h"
#include "headers/sequence_dispatcher.h"

using namespace telegram;

void UpdateManager::setUpdateCallback(UpdateCallback &&cb) {
    callback.swap(cb);
}

void UpdateManager::addSequence(int64_t user_id,
                  std::shared_ptr<Sequence<MessageCallback>> callback) {
    dispatcher[user_id] = callback;
}
void UpdateManager::removeSequence(int64_t user_id) {
    dispatcher.erase(user_id);
}
size_t UpdateManager::getOffset() const noexcept {
    return lastUpdate;
}
void UpdateManager::addCallback(std::string_view cmd, telegram::callbacks &&callback) {
    m_callbacks.insert(cmd,callback);
}

void UpdateManager::routeCallback(const std::string &str) {
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
                std::thread(&Sequence<MessageCallback>::input<Message>,
                            std::ref(*result->second),
                            fromJson<Message>(utility::objectToJson(message)))
                        .detach();
            }
            return;
        }
    }
    auto callback_router = [this](const auto &it) {
        if (it.HasMember("callback_query")) {
            auto obj = it["callback_query"].GetObject();
            auto data = obj["data"].GetString();

            if (findCallback<QueryCallback>(data) &&
                    runCallback<QueryCallback>(data, utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("inline_query")) {
            auto obj = it["inline_query"].GetObject();
            auto data = obj["query"].GetString();

            if (findCallback<InlineQueryCallback>(data) &&
                    runCallback<InlineQueryCallback>(data, utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("chosen_inline_result")) {
            auto obj = it["chosen_inline_result"].GetObject();
            auto data = obj["query"].GetString();

            if (findCallback<ChosenInlineResultCallback>(data) &&
                    runCallback<ChosenInlineResultCallback>(data,
                                                     utility::objectToJson(obj)))
                return;
        } else if (it.HasMember("message") &&
                   it["message"].GetObject().HasMember("text")) {
            auto obj = it["message"].GetObject();
            auto data = obj["text"].GetString();

            if (findCallback<MessageCallback>(data) &&
                    runCallback<MessageCallback>(data, utility::objectToJson(obj)))
                return;
        }
        else if (callback)
            std::thread(callback,fromJson<Update>(utility::objectToJson(it))).detach();
    };

    if (auto has_result = doc.HasMember("result");
            (has_result && doc["result"].IsArray()) || doc.IsArray()) {
        const auto &updates_arr =
                has_result ? doc["result"].GetArray() : doc.GetArray();
        if (updates_arr.Size())
            lastUpdate = updates_arr[updates_arr.Size() - 1]
                    .GetObject()["update_id"]
                    .GetUint64() + 1;

        for (auto &&it : updates_arr) {
            callback_router(it.GetObject());
        }
    } else if ((has_result && doc["result"].IsObject()) ||
               (!has_result && doc.IsObject())) {
        const auto &update =
                !has_result ? doc.GetObject() : doc["result"].GetObject();
        lastUpdate = update["update_id"].GetUint64() + 1;
        callback_router(update);
    } else {
        utility::logger::warn("Json document does not contain any parsable value");
        return;
    }
}
void UpdateManager::setOffset(size_t offset) {
    lastUpdate = offset;
}
