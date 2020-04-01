#include "headers/update_manager.h"
#include "headers/sequence_dispatcher.h"

using namespace telegram;

void UpdateManager::setUpdateCallback(UpdateCallback &&cb) {
    callback = cb;
}
void UpdateManager::addSequence(int64_t user_id,
                  std::shared_ptr<Sequences> callback) {
    dispatcher[user_id] = callback;
}
void UpdateManager::removeSequence(int64_t user_id) {
    dispatcher.erase(user_id);
}
size_t UpdateManager::getOffset() const noexcept {
    return lastUpdate;
}
void UpdateManager::addCallback(std::string_view cmd, telegram::Callbacks &&callback) {
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
    auto callback_router = [this](const auto &it) {
        if (runIfExist<QueryCallback>("callback_query","data",it)) {
             return;
        }
        if (runIfExist<InlineQueryCallback>("inline_query","query",it)) {
             return;
        }
        if (runIfExist<ChosenInlineResultCallback>("chosen_inline_result","query",it)) {
             return;
        }
        if (runIfExist<MessageCallback>("message","text",it)) {
                         return;
        }
        if (callback)
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
