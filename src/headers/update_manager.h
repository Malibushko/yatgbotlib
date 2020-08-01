#pragma once
#include <functional>
#include <variant>
#include <regex>
#include <fmt/format.h>

#include "telegram_structs.h"
#include "sequence_dispatcher.h"
#include "json_parser.h"
#include "utility/trie.h"
#include "utility/threadpool.h"

namespace telegram {

using UpdateCallback = std::function<void(const Update &)>;
using MessageCallback = std::function<void(const Message &)>;
using QueryCallback = std::function<void(const CallbackQuery &)>;
using InlineQueryCallback = std::function<void(const InlineQuery &)>;
using ShippingQueryCallback = std::function<void(const ShippingQuery &)>;
using PreCheckoutQueryCallback = std::function<void(const PreCheckoutQuery&)>;
using ChosenInlineResultCallback = std::function<void(const ChosenInlineResult &)>;

using Callbacks = std::variant<MessageCallback, QueryCallback, InlineQueryCallback,
ChosenInlineResultCallback,ShippingQueryCallback,PreCheckoutQueryCallback>;

using Sequences = std::variant<std::shared_ptr<Sequence<MessageCallback>>,
                               std::shared_ptr<Sequence<QueryCallback>>,
                               std::shared_ptr<Sequence<InlineQueryCallback>>,
                               std::shared_ptr<Sequence<ChosenInlineResultCallback>>,
                               std::shared_ptr<Sequence<ShippingQueryCallback>>,
                               std::shared_ptr<Sequence<PreCheckoutQueryCallback>>>;

/**
 * @brief This class performs routing on updates \n
 *
 * This class joins three callbacks types  \n
 * 1) Sequences  \n
 * 2) Callbacks  \n
 * 3) Usual updates (callback with UpdateCallback signature)  \n
 *
 * The priority of routing is equal to list order (Sequence -> Callback -> updates)  \n
 */
class UpdateManager {
private:
    /// Callback for Update (only one)
    UpdateCallback callback;
    /// Trie of commands
    utility::Trie<Callbacks> m_callbacks;
    /// Regexes
    std::vector<std::pair<std::regex,Callbacks>> m_regex;

    /// Container of sequences
    std::unordered_map<int64_t, Sequences>
    dispatcher;

    // ThreadPool for controlling  number of threads
    utility::ThreadPool pool;
    // for making requests to Telegram Bot Api
    size_t lastUpdate = 0;
public:
    explicit UpdateManager(std::size_t thread_num) : pool(thread_num) {
    }
    /**
     * @brief set callback for Update object
     * @param cb callback
     * There can be only one UpdateCallback
     * \warning previous callback will be deleted
     */
    void setUpdateCallback(UpdateCallback &&cb);
    /**
     * Add sequence for 'id' number
     * @param id Number to identify sequence
     * @param callback Callback to be invoked when there is new Update for 'id'
     */
    void addSequence(int64_t id,
                     const Sequences &callback);
    /**
     * Remove sequence with 'id' number
     * @param id Sequence id that was specified in 'addSequence'
     */
    void removeSequence(int64_t id);

    /// get current offset that is used for long polling
    size_t getOffset() const noexcept;
    /// set offset for long polling
    void setOffset(size_t offset);
    /**
     * @brief Add callback for the following command or data
     * @param cmd command or data that will trigger callback
     * @param cb callback
     */
    void addCallback(std::string_view cmd, telegram::Callbacks &&cb);
    /**
     * @brief addCallback for the following regex.
     * Regexes has the lowest priority (e.g will be triggered only if there is no command to match)
     * @param cmd regex that will trigger the callback if matched
     * @param callback
     */
    void addCallback(std::regex cmd, telegram::Callbacks &&callback);
    /**
     * @brief routeCallback
     * @param str - json string representing the value
     */
    void routeCallback(const std::string &str);
    /**
     * Find and Run callback for the folliwng command
     * @param cmd - command or data to route
     * @data json string represention the value
     */
    template <class CallbackType>
    bool runCallback(std::string_view cmd, const std::string &data);
    /**
     * Run callback for the folliwng command
     * @param cmd - command or data to route
     * @data json string represention the value
     */
    template <class CallbackType>
    bool runCallback(const Callbacks& cb,const std::string &data);

    /**
     * Look for callback and return boolean value if one present or not
     * @param cmd - Command/data to find
     * @return true if callback exists, false otherwise
     */
    template<class CallbackType>
    bool findCallback(std::string_view cmd);

    /**
     * Remove callback for the following command
     * @param cmd - Command/data to remove
     */
    template <class CallbackType>
    void removeCallback(std::string_view cmd);

    /**
     * Check if callback/regex/sequence is presend and run it
     * @param callback_name - name of data in Update object (e.g "shipping_query","callback_query" etc.)
     * @param callback_data - name of data in Callback object (for example "shipping_query" is "query")
     * @param document containing json value
     * @return true if callback was invoked, false otherwise
     */
    template <class CallbackType,class Value>
    bool runIfExist(std::string_view callback_name,std::string_view callback_data,
                    const Value& doc);
    /**
     * Look for sequence and run if it exist for current id
     * @param id - id to look for
     * @param val - json value representing the object
     * @return true if callback was invoked, false otherwise
     */
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
            // get argument type from callback type (void(Update&&) -> Update)
            using callback_arg_type = typename traits::func_signature<value_type>::args_type;
            if (value) {
                // process detached
                utility::Logger::info(fmt::format("Run callback for command: {}",cmd.data()));
                pool.enqueue(value,JsonParser::i().fromJson<callback_arg_type>(data));
                // set the flag if run was successfull
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
            // get argument type from callback type (void(Update&&) -> Update)
            using callback_arg_type = typename traits::func_signature<value_type>::args_type;
            if (value) {
                // process detached
                pool.enqueue(std::forward<decltype(value)>(value),JsonParser::i().fromJson<callback_arg_type>(data));
                // set the flag if run was successfull
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
                                          JsonParser::i().rapidObjectToJson(doc[callback_name.data()].GetObject())))
            return true;
    }
    // check on any regex match
    for (auto && [regex,callback] : m_regex) {
        if (std::regex_match(callback_data.data(),regex)) {
            if (runCallback<CallbackType>(callback,JsonParser::i().rapidObjectToJson(doc[callback_name.data()])))
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
        // if sequence present for current user
            std::visit([&](auto&& value){
                using variant_type = std::decay_t<decltype (value)>;
                using sequence_type = typename variant_type::element_type;
                using value_type = typename sequence_type::EventType;
                if constexpr (std::is_same_v<value_type, CallbackType>) {
                    if (!value || value->finished()) {
                        // if sequence has finished erase it and return not triggering the callback
                        dispatcher.erase(result);
                    } else {
                        call_successfull = true;
                        pool.enqueue([&,object = JsonParser::i().rapidObjectToJson(val)](){
                            // get real argument type anr run detached
                            using callbackArgType = typename traits::func_signature<value_type>::args_type;
                            value->input(JsonParser::i().fromJson<callbackArgType>(object));
                        });
                    }
                }
            },result->second);
    }
    return call_successfull;
}

} // namespace telegram
