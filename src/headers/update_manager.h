#pragma once
#include <thread>
#include <functional>
#include <variant>
#include "utility/trie.h"
#include "telegram_structs.h"
#include "headers/sequence_dispatcher.h"

namespace telegram {

using UpdateCallback = std::function<void(Update &&)>;
using MessageCallback = std::function<void(Message &&)>;
using QueryCallback = std::function<void(CallbackQuery &&)>;
using InlineQueryCallback = std::function<void(InlineQuery &&)>;
using ChosenInlineResultCallback = std::function<void(ChosenInlineResult &&)>;
using callbacks = std::variant<MessageCallback, QueryCallback, InlineQueryCallback,
ChosenInlineResultCallback>;


class UpdateManager {
private:
    UpdateCallback callback;
    Trie<callbacks> m_callbacks;

    std::unordered_map<int64_t, std::shared_ptr<Sequence<MessageCallback>>>
    dispatcher;
    size_t lastUpdate = 0;
public:
    UpdateManager() {}
    void setUpdateCallback(UpdateCallback &&cb);

    void addSequence(int64_t user_id,
                      std::shared_ptr<Sequence<MessageCallback>> callback);
    void removeSequence(int64_t user_id);

    size_t getOffset() const noexcept;
    void setOffset(size_t offset);

    void addCallback(std::string_view cmd, telegram::callbacks &&cb);
    void routeCallback(const std::string &str);
    template <class CallbackType>
    bool runCallback(std::string_view cmd, const std::string &data);
    template<class CallbackType>
    bool findCallback(std::string_view cmd) {
        telegram::callbacks value = m_callbacks.find(cmd);
        return std::holds_alternative<CallbackType>(value);
    }
    template <class CallbackType>
    void removeCallback(std::string_view cmd);
};

template <class CallbackType>
bool UpdateManager::runCallback(std::string_view cmd, const std::string &data) {
    telegram::callbacks value = m_callbacks.find(cmd);

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
void UpdateManager::removeCallback(std::string_view cmd) {
    m_callbacks.erase(cmd);
}

} // namespace telegram
