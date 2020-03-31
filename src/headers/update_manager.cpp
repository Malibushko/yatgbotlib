#include "update_manager.h"

telegram::update_manager::update_manager() {}

void telegram::update_manager::set_update_callback(telegram::update_callback &&cb) {
    callback.swap(cb);
}

void telegram::update_manager::add_sequence(int64_t user_id, std::shared_ptr<sequence<telegram::msg_callback> > callback) {
    dispatcher[user_id] = callback;
}

void telegram::update_manager::remove_sequence(int64_t user_id) { dispatcher.erase(user_id); }

size_t telegram::update_manager::get_offset() const noexcept { return last_update; }

void telegram::update_manager::set_offset(size_t offset) { last_update = offset; }

void telegram::update_manager::add_callback(std::string_view cmd, telegram::callbacks &&cb) {
    std::visit(
                [&](auto &&callback) {
        using cb_type = std::decay_t<decltype(callback)>;
        if constexpr (std::is_same_v<cb_type, msg_callback>) {
            msg_callbacks.insert(cmd, callback);
        } else if constexpr (std::is_same_v<cb_type, query_callback>) {
            query_callbacks.insert(cmd, callback);
        }
    });
}
