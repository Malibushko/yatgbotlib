#pragma once
#include <thread>
#include <functional>
#include <variant>
#include <regex>
#include "telegram_structs.h"
#include "utility/trie.h"
#include "sequence_dispatcher.h"
#include "json_parser.h"
namespace telegram {

using UpdateCallback = std::function<void(Update &&)>;
using MessageCallback = std::function<void(Message &&)>;
using QueryCallback = std::function<void(CallbackQuery &&)>;
using InlineQueryCallback = std::function<void(InlineQuery &&)>;
using ChosenInlineResultCallback = std::function<void(ChosenInlineResult &&)>;
using Callbacks = std::variant<MessageCallback, QueryCallback, InlineQueryCallback,
ChosenInlineResultCallback>;
using Sequences = std::variant<Sequence<MessageCallback>,
Sequence<QueryCallback>,
Sequence<InlineQueryCallback>,
Sequence<ChosenInlineResultCallback>>;

class UpdateManager {
private:
    UpdateCallback callback;
    Trie<Callbacks> m_callbacks;
    std::vector<std::pair<std::regex,Callbacks>> m_regex;

    std::unordered_map<int64_t, std::shared_ptr<Sequences>>
    dispatcher;
    size_t lastUpdate = 0;
public:
    UpdateManager() {}
    void setUpdateCallback(UpdateCallback &&cb);

    void addSequence(int64_t user_id,
                     std::shared_ptr<Sequences> callback);
    void removeSequence(int64_t user_id);

    size_t getOffset() const noexcept;
    void setOffset(size_t offset);

    void addCallback(std::string_view cmd, telegram::Callbacks &&cb);
    void addCallback(std::regex cmd, telegram::Callbacks &&callback);
    void routeCallback(const std::string &str);
    template <class CallbackType>
    bool runCallback(std::string_view cmd, const std::string &data);

    template <class CallbackType>
    bool runCallback(const Callbacks& cb,const std::string &data);

    template<class CallbackType>
    bool findCallback(std::string_view cmd);
    template <class CallbackType>
    void removeCallback(std::string_view cmd);

    template <class CallbackType,class Value>
    bool runIfExist(std::string_view callback_name,std::string_view callback_data,
                    const Value& doc);

    template<class CallbackType,class Value>
    bool runIfSequence(int64_t id,const Value& val);
};

template <class CallbackType>
bool UpdateManager::runCallback(std::string_view cmd, const std::string &data) {
    telegram::Callbacks value = m_callbacks.find(cmd).value();

    bool value_found = false;
    std::visit([&](auto&& value){
        using value_type = std::decay_t<decltype (value)>;
        if constexpr (std::is_same_v<CallbackType,value_type>) {
            using callback_arg_type = typename traits::func_signature<value_type>::args_type;
            if (value) {
                std::thread(value,fromJson<callback_arg_type>(data)).detach();
                value_found = true;
            }
        }
    },value);
    return value_found;
}
template <class CallbackType>
bool UpdateManager::runCallback(const Callbacks& cb,const std::string &data) {
    bool value_found = false;
    std::visit([&](auto&& value){
        using value_type = std::decay_t<decltype (value)>;
        if constexpr (std::is_same_v<CallbackType,value_type>) {
            using callback_arg_type = typename traits::func_signature<value_type>::args_type;
            if (value) {
                std::thread(value,fromJson<callback_arg_type>(data)).detach();
                value_found = true;
            }
        }
    },cb);
    return value_found;
}
template <class CallbackType,class Value>
bool UpdateManager::runIfExist(std::string_view callback_name,std::string_view callback_data,
                               const Value& doc) {
    // check if json contain callback data for current type
    if (!doc.HasMember(callback_name.data()))
        return false;
    // check if there is a sequence for the user
    if (dispatcher.size()
            && doc[callback_name.data()].HasMember("from")
            && runIfSequence<CallbackType>(doc[callback_name.data()]["from"]["id"].GetInt64(),
                                           doc[callback_name.data()].GetObject())) {
        return true;
    }
    // else run callback if it exists
    if (doc[callback_name.data()].HasMember(callback_data.data())) {
        const char*  data = doc[callback_name.data()][callback_data.data()].GetString();
        if (findCallback<CallbackType>(data) &&
                runCallback<CallbackType>(data,
                                          utility::objectToJson(doc[callback_name.data()].GetObject())))
            return true;
    }
    // check on any regex match
    for (auto && [regex,callback] : m_regex) {
        if (std::regex_match(callback_data.data(),regex)) {
            if (runCallback<CallbackType>(callback,utility::objectToJson(doc[callback_name.data()])))
                return true;
        }
    }
    return false;
}
template<class CallbackType>
bool UpdateManager::findCallback(std::string_view cmd) {
    auto value = m_callbacks.find(cmd);
    if (!value)
        return false;
    return std::holds_alternative<CallbackType>(value.value()) && !value->valueless_by_exception();
}
template <class CallbackType>
void UpdateManager::removeCallback(std::string_view cmd) {
    m_callbacks.erase(cmd);
}
template<class CallbackType,class Value>
bool UpdateManager::runIfSequence(int64_t id,const Value& val) {
    bool call_successfull = false;
    if (auto result = dispatcher.find(id);result != dispatcher.end()) {
        if (result->second) {
            std::visit([&](auto&& value){
                using value_type = typename std::decay_t<decltype (value)>::EventType;
                if constexpr (std::is_same_v<value_type, CallbackType>) {
                    if (value.finished()) {
                        dispatcher.erase(result);
                    } else {
                        call_successfull = true;
                        std::thread([&,object = utility::objectToJson(val)](){
                            using callbackArgType = typename traits::func_signature<value_type>::args_type;
                            value.input(fromJson<callbackArgType>(object));
                        }).detach();
                    }
                }
            },*result->second);
        }
    }

    return call_successfull;
}

} // namespace telegram
