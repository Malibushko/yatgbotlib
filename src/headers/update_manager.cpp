#include "update_manager.h"

telegram::UpdateManager::UpdateManager() {}

void telegram::UpdateManager::setUpdateCallback(telegram::UpdateCallback &&cb) {
    callback.swap(cb);
}

void telegram::UpdateManager::addSequence(int64_t user_id, std::shared_ptr<Sequence<telegram::MessageCallback> > callback) {
    dispatcher[user_id] = callback;
}

void telegram::UpdateManager::removeSequence(int64_t user_id) { dispatcher.erase(user_id); }

size_t telegram::UpdateManager::getOffset() const noexcept { return lastUpdate; }

void telegram::UpdateManager::setOffset(size_t offset) { lastUpdate = offset; }

void telegram::UpdateManager::addCallback(std::string_view cmd, telegram::Callbacks &&cb) {
    std::visit(
                [&](auto &&callback) {
        using cb_type = std::decay_t<decltype(callback)>;
        if constexpr (std::is_same_v<cb_type, MessageCallback>) {
            msg_callbacks.insert(cmd, callback);
        } else if constexpr (std::is_same_v<cb_type, QueryCallback>) {
            query_callbacks.insert(cmd, callback);
        }
    });
}
