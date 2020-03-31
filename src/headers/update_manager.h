#pragma once
#include <thread>
#include <functional>
#include <variant>
#include "utility/trie.h"
#include "telegram_structs.h"
#include "headers/sequence_dispatcher.h"

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
    Trie<callbacks> m_callbacks;

    std::unordered_map<int64_t, std::shared_ptr<sequence<msg_callback>>>
    dispatcher;
    size_t last_update = 0;
public:
    update_manager() {}
    void set_update_callback(update_callback &&cb);

    void add_sequence(int64_t user_id,
                      std::shared_ptr<sequence<msg_callback>> callback);
    void remove_sequence(int64_t user_id);

    size_t get_offset() const noexcept;
    void set_offset(size_t offset);

    void add_callback(std::string_view cmd, telegram::callbacks &&cb);
    void route_callback(const std::string &str);
    template <class CallbackType>
    bool run_callback(std::string_view cmd, const std::string &data);
    template<class CallbackType>
    bool find_callback(std::string_view cmd) {
        telegram::callbacks value = m_callbacks.find(cmd);
        return std::holds_alternative<CallbackType>(value);
    }
    template <class CallbackType>
    void remove_callback(std::string_view cmd);
};

template <class CallbackType>
bool update_manager::run_callback(std::string_view cmd, const std::string &data) {
    telegram::callbacks value = m_callbacks.find(cmd);

    bool value_found = false;
    std::visit([&](auto&& value){
        using value_type = std::decay_t<decltype (value)>;
        if constexpr (std::is_same_v<CallbackType,value_type>) {
            using callback_arg_type = typename traits::func_signature<value_type>::args_type;
            if (value) {
                std::thread(value,from_json<callback_arg_type>(data)).detach();
                value_found = true;
            }
        }
    },value);
    return value_found;
}

template <class CallbackType>
void update_manager::remove_callback(std::string_view cmd) {
    m_callbacks.erase(cmd);
}
} // namespace telegram
