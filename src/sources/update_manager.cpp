#include <type_traits>
#include <future>

#include "headers/update_manager.h"
#include "headers/sequence_dispatcher.h"

using namespace telegram;

void UpdateManager::setUpdateCallback(UpdateCallback &&cb) {
    callback = cb;
}
void UpdateManager::addSequence(int64_t user_id, const Sequences& callback) {
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
void UpdateManager::addCallback(std::regex cmd, telegram::Callbacks &&callback) {
    m_regex.emplace_back(cmd,callback);
}
void UpdateManager::routeCallback(const std::string &str) {
    rapidjson::Document doc;
    const auto &ok = doc.Parse(str.data());
    if (ok.HasParseError()) {
        utility::Logger::warn("Document parse error. \nRapidjson Error Code: ",
                              ok.GetParseError(),"\nOffset: ",ok.GetErrorOffset(),'\n',
                              "JSON: ",str);
        return;
    }
    if (doc.HasMember("ok") && !doc["ok"].GetBool()) {
        utility::Logger::warn("Error: ",doc["description"].GetString(),'\n');
        return;
    }

    auto callback_router = [this](const auto &it) {
        // try to find callback and run it
        // if run was successfull no other callback will be triggered

        if (runIfExist<QueryCallback>("callback_query","data",it)
                || runIfExist<InlineQueryCallback>("inline_query","query",it)
                || runIfExist<ChosenInlineResultCallback>("chosen_inline_result","query",it)
                || runIfExist<ShippingQueryCallback>("shipping_query","invoice_payload",it)
                || runIfExist<PreCheckoutQueryCallback>("pre_checkout_query","invoice_payload",it)
                || runIfExist<MessageCallback>("message","text",it))
            return;
        // if no other callback/regex/sequence match the callback, run the default callback (if it present)

        if (callback)
            pool.enqueue(callback,JsonParser::i().fromJson<Update>(JsonParser::i().rapidObjectToJson(it)));
    };
    // if result is array, extract the array and process every update
    if (doc["result"].IsArray() || doc.IsArray()) {
        const auto &updates_arr = !doc.IsArray() ? doc["result"].GetArray() : doc.GetArray();
        // update offset value for next queries
        if (updates_arr.Size())
            lastUpdate = updates_arr[updates_arr.Size() - 1].GetObject()["update_id"].GetUint64() + 1;

        for (auto &&it : updates_arr) {
            callback_router(it.GetObject());
        }
        // the same but with only one element
    } else if (doc["result"].IsObject() || doc.IsObject()) {
        const auto &update = doc.IsObject() ? doc.GetObject() : doc["result"].GetObject();
        lastUpdate = update["update_id"].GetUint64() + 1;
        callback_router(update);
    } else {
        utility::Logger::warn("Json document does not contain any parsable value");
        return;
    }
}
void UpdateManager::setOffset(size_t offset) {
    lastUpdate = offset;
}
