#include "telegram_bot.h"

namespace telegram {
Bot::Bot(const std::string &token) noexcept :
    api{std::make_unique<api_manager>("https://api.telegram.org/bot" + token + '/')} {
}

void Bot::onUpdate(update_callback &&cb) {
    updater.set_raw_callback(std::move(cb));
}


void Bot::onInlineResult(std::string_view cmd, chosen_inline_callback &&cb) {
    updater.add_callback(cmd,std::move(cb));
}


void Bot::onCallback(std::string_view cmd, query_callback &&cb) {
    updater.add_callback(cmd,std::move(cb));
}


void Bot::onCommand(std::string_view cmd, msg_callback &&cb) {
    updater.add_callback(cmd,std::move(cb));
}


void Bot::startSequence(int64_t user_id, std::shared_ptr<sequence<msg_callback> > seq) {
    updater.add_sequence(user_id,seq);
}


void Bot::stopSequence(int64_t user_id) {
    updater.remove_sequence(user_id);
}


std::pair<WebhookInfo, std::optional<error>> Bot::getWebhookInfo() const {
    return api->call_api<WebhookInfo>(__func__);
}


void Bot::start(std::optional<uint32_t> timeout, std::optional<uint32_t> offset, std::optional<uint8_t> limit, std::optional<std::vector<std::string_view> > allowed_updates) {
    if (auto&& [webhook,error] = getWebhookInfo();error || webhook.url.size() || webhookSet) {
        if (error)
            std::cerr << error.value();
        std::cerr << "You must remove webhook before using long polling method.";
        return;
    }
    stopPolling = false;
    updater.set_offset(offset.value_or(0));
    while (!stopPolling) {
        updater.route_callback(getUpdatesRawJson(updater.get_offset(),limit,timeout,allowed_updates));
        std::this_thread::sleep_for(100*std::chrono::milliseconds(timeout.value_or(1)));
    }
}
void Bot::stop() {
    stopPolling = true;
}
std::pair<User, o_error> Bot::getMe() const {
    return api->call_api<User>(__func__);
}

std::pair<Message,o_error> Bot::sendMessage(Bot::IntOrString chat_id, const std::string &text, o_string parse_mode, o_bool disable_web_page_preview, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {

    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(text)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(disable_web_page_preview)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::reply(const Message &msg, const std::string &message) const {
    return sendMessage(msg.chat.id,message,{},{},{},msg.message_id);
}


std::pair<Message,o_error> Bot::reply(const Message &msg, const std::string &text, const Bot::ReplyMarkups &markup) const {
    return sendMessage(msg.chat.id,text,{},{},{},msg.message_id,markup);
}


std::pair<std::vector<Update>,o_error> Bot::getUpdates(std::optional<uint32_t> offset,
                                                       std::optional<uint8_t> limit,
                                                       std::optional<uint32_t> timeout,
                                                       std::optional<std::vector<std::string_view>> allowed_updates) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(offset) << NAME_VALUE_PAIR(limit) <<
               NAME_VALUE_PAIR(timeout) << NAME_VALUE_PAIR(allowed_updates);
    return api->call_api<std::vector<Update>>(__func__,builder);
}


std::string Bot::getUpdatesRawJson(std::optional<uint32_t> offset, std::optional<uint8_t> limit, std::optional<uint32_t> timeout, std::optional<std::vector<std::string_view> > allowed_updates) {
    query_builder builder;
    builder << NAME_VALUE_PAIR(offset) << NAME_VALUE_PAIR(limit) <<
               NAME_VALUE_PAIR(timeout) << NAME_VALUE_PAIR(allowed_updates);
    return api->call_api_raw_json("getUpdates",builder);
}

std::pair<Message,o_error> Bot::forwardMessage(Bot::IntOrString chat_id,
                                               Bot::IntOrString from_chat_id,
                                               int64_t message_id,
                                               o_bool disable_notification) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(from_chat_id)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(disable_notification);
    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendPhoto(Bot::IntOrString chat_id, const std::string &photo, std::string_view caption, std::string_view parse_mode, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) <<
               NAME_VALUE_PAIR(caption) << NAME_VALUE_PAIR(parse_mode)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder,{NAME_VALUE_PAIR(photo)});
}


std::pair<Message,o_error> Bot::sendAudio(Bot::IntOrString chat_id, std::string audio, o_sv caption, o_sv parse_mode, std::optional<int> duration, o_sv performer, o_sv title, const std::optional<std::string> &thumb, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(performer) << NAME_VALUE_PAIR(title)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);
    std::vector<name_value_pair> params{NAME_VALUE_PAIR(audio)};
    if (thumb.has_value())
        params.push_back(NAME_VALUE_PAIR(thumb.value()));

    return api->call_api<Message>(__func__,builder,params);
}


std::pair<bool,o_error> Bot::setWebhook(std::string_view url,
                                        const std::optional<std::string> &certificate,
                                        const std::optional<uint32_t> max_connections,
                                        const std::optional<std::vector<std::string_view>> &allowed_updates) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(url) << NAME_VALUE_PAIR(max_connections)
            << NAME_VALUE_PAIR(allowed_updates);
    if (certificate)
        return api->call_api<bool>(__func__,builder,{NAME_VALUE_PAIR(certificate.value())});
    else
        return api->call_api<bool>(__func__,builder);
}


bool Bot::setWebhookServer(const std::string &url, uint16_t port, const std::string &cert_path, const std::string &key_path) {

    webhookSet = true;
    auto [result,error] = setWebhook(url+':'+std::to_string(port),cert_path);
            if (error || !result) {
        if (error)
            std::cerr << error.value();
        return false;
    }
    httplib::SSLServer server(cert_path.data(),key_path.data());
    server.Post("/",[&](const httplib::Request& req,httplib::Response& res){
        if (auto host = req.headers.find("REMOTE_ADDR");host != req.headers.end()) {
            uint32_t host_ip = utility::ipv4(host->second.data());
            if (host_ip != std::clamp(host_ip,
                                      utility::telegram_first_subnet_range_begin,
                                      utility::telegram_second_subned_range_end)
                    &&
               host_ip != std::clamp(host_ip,
                                             utility::teleram_second_subnet_range_begin,
                                             utility::telegram_second_subned_range_end)) {
                return;
            }
        } else
            return;
        updater.route_callback(req.body);
    });
    return server.listen("0.0.0.0",port);
}


std::pair<Message,o_error> Bot::sendDocument(Bot::IntOrString chat_id, const std::string &document, const std::optional<std::string> thumb, o_sv caption, o_sv parse_mode, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(parse_mode)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    std::vector<name_value_pair> params{NAME_VALUE_PAIR(document)};
    if (thumb.has_value())
        params.push_back(NAME_VALUE_PAIR(thumb.value()));

    return api->call_api<Message>(__func__,builder,params);
}


std::pair<Message,o_error> Bot::sendVideo(Bot::IntOrString chat_id, const std::string &video, Bot::o_int32 duration, Bot::o_int32 width, Bot::o_int32 height, const std::optional<std::string> &thumb, o_sv caption, o_sv parse_mode, o_bool supports_streaming, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(width) << NAME_VALUE_PAIR(height)
            << NAME_VALUE_PAIR(supports_streaming) << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    std::vector<name_value_pair> params{NAME_VALUE_PAIR(video)};
    if (thumb.has_value())
        params.push_back(NAME_VALUE_PAIR(thumb.value()));

    return api->call_api<Message>(__func__,builder,params);
}


std::pair<Message, o_error> Bot::sendAnimation(Bot::IntOrString chat_id, const std::string &animation, Bot::o_int32 duration, Bot::o_int32 width, Bot::o_int32 height, const std::optional<std::string> &thumb, o_sv caption, o_sv parse_mode, o_bool supports_streaming, o_bool disable_nofitication, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(width) << NAME_VALUE_PAIR(height)
            << NAME_VALUE_PAIR(supports_streaming) << NAME_VALUE_PAIR(disable_nofitication)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    std::vector<name_value_pair> params{NAME_VALUE_PAIR(animation)};
    if (thumb.has_value())
        params.push_back(NAME_VALUE_PAIR(thumb.value()));

    return api->call_api<Message>(__func__,builder,params);

}


std::pair<Message,o_error> Bot::sendVoice(Bot::IntOrString chat_id, const std::string &voice, o_sv caption, o_sv parse_mode, Bot::o_int32 duration, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder,{NAME_VALUE_PAIR(voice)});
}


std::pair<Message,o_error> Bot::sendVideoNote(Bot::IntOrString chat_id, const std::string &video_note, Bot::o_int32 duration, Bot::o_int32 length, const std::optional<std::string> &thumb, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(length) << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    std::vector<name_value_pair> params{NAME_VALUE_PAIR(video_note)};
    if (thumb.has_value())
        params.push_back(NAME_VALUE_PAIR(thumb.value()));

    return api->call_api<Message>(__func__,builder,params);
}


std::pair<Message,o_error> Bot::sendLocation(Bot::IntOrString chat_id, float latitude, float longitude, Bot::o_int32 live_period, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(latitude)
            << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(longitude) << NAME_VALUE_PAIR(live_period)
            << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::editMessageLiveLocation(Bot::IntOrString chat_id, int64_t message_id, float latitude, float longitude, o_sv inline_message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(latitude) << NAME_VALUE_PAIR(longitude)
            << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);
    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::editMessageLiveLocation(float latitude, float longitude, std::string_view inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(latitude) << NAME_VALUE_PAIR(longitude)
            << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);
    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::stopMessageLiveLocation(Bot::IntOrString chat_id, int64_t message_id, o_sv inline_message_id, std::optional<InlineKeyboardMarkup> reply_markup) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);
    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::stopMessageLiveLocation(std::string_view inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);
    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendVenue(Bot::IntOrString chat_id, float latitude, float longitude, std::string_view title, std::string_view address, o_sv foursquare_id, o_sv foursquare_type, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(latitude) << NAME_VALUE_PAIR(longitude)
            << NAME_VALUE_PAIR(title) << NAME_VALUE_PAIR(address) << NAME_VALUE_PAIR(foursquare_id)
            << NAME_VALUE_PAIR(foursquare_type) << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendContact(Bot::IntOrString chat_id, std::string_view phone_number, std::string_view first_name, o_sv last_name, o_sv vcard, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(phone_number)
            << NAME_VALUE_PAIR(first_name) << NAME_VALUE_PAIR(last_name)
            << NAME_VALUE_PAIR(vcard) << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendPoll(Bot::IntOrString chat_id, std::string_view question, const std::vector<std::string_view> &options, o_bool is_anonymous, o_bool allows_multiple_answers, o_sv type, Bot::o_int32 correct_option_id, o_bool is_closed, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) <<  NAME_VALUE_PAIR(allows_multiple_answers)
            << NAME_VALUE_PAIR(question) << NAME_VALUE_PAIR(options)
            << NAME_VALUE_PAIR(is_anonymous) << NAME_VALUE_PAIR(type) << NAME_VALUE_PAIR(correct_option_id)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(is_closed) <<
               NAME_VALUE_PAIR(reply_markup) << NAME_VALUE_PAIR(reply_to_message_id);

    return api->call_api<Message>(__func__,builder);
}


std::pair<bool,o_error> Bot::sendChatAction(Bot::IntOrString chat_id, std::string_view action) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(action);
    return api->call_api<bool>(__func__,builder);
}


std::pair<UserProfilePhotos,o_error> Bot::getUserProfilePhotos(int64_t user_id, Bot::o_int32 offset, Bot::o_int32 limit) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(offset) << NAME_VALUE_PAIR(limit);

    return api->call_api<UserProfilePhotos>(__func__,builder);
}


std::pair<File,o_error> Bot::getFile(std::string_view &file_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(file_id);

    return api->call_api<File>(__func__,builder);
}


std::pair<bool,o_error> Bot::kickChatMember(Bot::IntOrString chat_id, int64_t user_id, Bot::o_int32 until_date) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(until_date);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::unbanChatMember(Bot::IntOrString chat_id, int64_t user_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::restrictChatMember(Bot::IntOrString chat_id, int64_t user_id, const ChatPermissions &perms, o_int64 until_date) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id)
            << NAME_VALUE_PAIR(perms) << NAME_VALUE_PAIR(until_date);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::promoteChatMember(Bot::IntOrString chat_id, int64_t user_id, o_bool can_change_info, o_bool can_post_messages, o_bool can_edit_messages, o_bool can_delete_messages, o_bool can_invite_users, o_bool can_restrict_members, o_bool can_pin_messages, o_bool can_promote_members) {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id)
            << NAME_VALUE_PAIR(can_change_info) << NAME_VALUE_PAIR(can_post_messages)
            << NAME_VALUE_PAIR(can_delete_messages) << NAME_VALUE_PAIR(can_invite_users)
            << NAME_VALUE_PAIR(can_restrict_members) << NAME_VALUE_PAIR(can_pin_messages)
            << NAME_VALUE_PAIR(can_promote_members) << NAME_VALUE_PAIR(can_edit_messages);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error>  Bot::setChatAdministratorCustomTitle(Bot::IntOrString chat_id, int64_t user_id, std::string_view custom_title) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(custom_title);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::setChatPermissions(Bot::IntOrString chat_id, const ChatPermissions &permissions) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(permissions);

    return api->call_api<bool>(__func__,builder);
}


std::pair<std::string,o_error> Bot::exportChatInviteLink(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<std::string>(__func__,builder);
}


std::pair<bool,o_error>  Bot::setChatPhoto(Bot::IntOrString chat_id, const std::string &photo) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);
    return api->call_api<bool>(__func__,builder,{NAME_VALUE_PAIR(photo)});
}


std::pair<bool,o_error> Bot::deleteChatPhoto(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::setChatTitle(Bot::IntOrString chat_id, std::string_view title) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(title);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error>  Bot::setChatDescription(Bot::IntOrString chat_id, o_sv description) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(description);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::pinChatMessage(Bot::IntOrString chat_id, int64_t message_id, o_bool disable_notification) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(disable_notification);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::unpinChatMessage(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error>  Bot::leaveChat(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);
    return api->call_api<bool>(__func__,builder);
}


std::pair<Chat,o_error>  Bot::getChat(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<Chat>(__func__,builder);
}


std::pair<std::vector<ChatMember>,o_error>  Bot::getChatAdministrators(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<std::vector<ChatMember>>(__func__,builder);
}


std::pair<uint32_t,o_error> Bot::getChatMembersCount(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);
    return api->call_api<uint32_t>(__func__,builder);
}

std::pair<ChatMember,o_error> Bot::getChatMember(Bot::IntOrString chat_id, uint32_t user_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(user_id);

    return api->call_api<ChatMember>(__func__,builder);
}


std::pair<bool,o_error> Bot::setChatStickerSet(Bot::IntOrString chat_id, std::string_view sticker_set_name) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(sticker_set_name);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::deleteChatStickerSet(Bot::IntOrString chat_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::answerCallbackQuery(std::string_view callback_query_id, o_sv text, o_bool show_alert, o_sv url, Bot::o_int32 cache_time) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(callback_query_id) << NAME_VALUE_PAIR(text)
            << NAME_VALUE_PAIR(show_alert) << NAME_VALUE_PAIR(url)
            << NAME_VALUE_PAIR(cache_time);

    return api->call_api<bool>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageText(Bot::IntOrString chat_id, std::string_view text, o_int64 message_id, o_sv inline_message_id, o_sv parse_mode, o_bool disable_web_page_preview, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(text)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(disable_web_page_preview)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageText(std::string_view text, const std::string &inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, o_sv parse_mode, o_bool disable_web_page_preview, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(text)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(disable_web_page_preview)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageCaption(Bot::IntOrString chat_id, o_int64 message_id, o_sv inline_message_id, o_sv caption, o_sv parse_mode, std::optional<InlineKeyboardMarkup> reply_markup) {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageCaption(const std::string &inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, o_sv caption, o_sv parse_mode, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(caption)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id)
            << NAME_VALUE_PAIR(parse_mode) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageMedia(Bot::InputMedia media, o_int64 inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(media) << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageMedia(Bot::InputMedia media, Bot::IntOrString chat_id, int64_t message_id, o_sv inline_message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(media) << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageReplyMarkup(std::string_view inline_message_id, std::optional<Bot::IntOrString> chat_id, o_int64 message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::editMessageReplyMarkup(Bot::IntOrString chat_id, int64_t message_id, std::string_view inline_message_id, std::optional<InlineKeyboardMarkup> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<bool,o_error> Bot::deleteMessage(Bot::IntOrString chat_id, int64_t message_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<Poll,o_error> Bot::stopPoll(Bot::IntOrString chat_id, int64_t message_id, std::optional<InlineKeyboardMarkup> reply_markup) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id)
            << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Poll>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendSticker(Bot::IntOrString chat_id, const std::string &sticker, o_bool disable_notification, o_int64 reply_to_message_id, std::optional<Bot::ReplyMarkups> reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id)
            << NAME_VALUE_PAIR(disable_notification)
            << NAME_VALUE_PAIR(reply_to_message_id) << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder,{NAME_VALUE_PAIR(sticker)});
}


std::pair<StickerSet,o_error> Bot::getStickerSet(const std::string &name) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(name);

    return api->call_api<StickerSet>(__func__,builder);
}


std::pair<File,o_error> Bot::uploadStickerFile(int64_t user_id, const std::string &png_sticker) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id);
    return api->call_api<File>(__func__,builder,{NAME_VALUE_PAIR(png_sticker)});
}


std::pair<bool,o_error> Bot::createNewStickerSet(int64_t user_id, std::string_view name, std::string_view title, const std::string &png_sticker, std::string_view emojis, o_bool contains_masks, const std::optional<MaskPosition> &mask_position) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(name)
            << NAME_VALUE_PAIR(title) << NAME_VALUE_PAIR(emojis)
            << NAME_VALUE_PAIR(contains_masks) << NAME_VALUE_PAIR(mask_position);

    return api->call_api<bool>(__func__,builder,{NAME_VALUE_PAIR(png_sticker)});

}


std::pair<bool,o_error> Bot::addStickerToSet(int64_t user_id, std::string_view name, const std::string &png_sticker, std::string_view emojis, const std::optional<MaskPosition> &mask_position) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(name)
            << NAME_VALUE_PAIR(emojis) << NAME_VALUE_PAIR(mask_position);

    return api->call_api<bool>(__func__,builder,{NAME_VALUE_PAIR(png_sticker)});
}


std::pair<bool,o_error> Bot::setStickerPositionInSet(std::string_view sticker, int32_t position) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(sticker) << NAME_VALUE_PAIR(position);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::deleteStickerFromSet(const std::string &sticker) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(sticker);

    return api->call_api<bool>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendInvoice(int64_t chat_id, std::string_view title, std::string_view description, std::string_view payload, std::string_view provider_token, std::string_view start_parameter, std::string_view currency, const std::vector<LabeledPrice> &prices, o_sv provider_data, o_sv photo_url, Bot::o_int32 photo_size, Bot::o_int32 photo_width, Bot::o_int32 photo_height, o_bool need_name, o_bool need_phone_number, o_bool need_email, o_bool need_shipping_address, o_bool send_phone_number_to_provider, o_bool send_email_to_provider, o_bool is_flexible, o_bool disable_notification, Bot::o_int32 reply_to_message_id, const std::optional<InlineKeyboardMarkup> &reply_markup) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(title)
            << NAME_VALUE_PAIR(description) << NAME_VALUE_PAIR(payload)
            << NAME_VALUE_PAIR(provider_token) << NAME_VALUE_PAIR(start_parameter)
            << NAME_VALUE_PAIR(currency) << NAME_VALUE_PAIR(prices) << NAME_VALUE_PAIR(provider_data)
            << NAME_VALUE_PAIR(photo_url) << NAME_VALUE_PAIR(photo_size) << NAME_VALUE_PAIR(photo_width)
            << NAME_VALUE_PAIR(photo_height) << NAME_VALUE_PAIR(need_name) <<
               NAME_VALUE_PAIR(need_phone_number) << NAME_VALUE_PAIR(need_email)
            << NAME_VALUE_PAIR(need_shipping_address) << NAME_VALUE_PAIR(send_phone_number_to_provider)
            << NAME_VALUE_PAIR(send_email_to_provider) << NAME_VALUE_PAIR(is_flexible)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(reply_to_message_id)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}


std::pair<bool,o_error> Bot::answerShippingQuery(std::string_view shipping_query_id, std::false_type, std::string_view error_message) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(shipping_query_id) << name_value_pair{"ok","false"}
            << NAME_VALUE_PAIR(error_message);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::answerShippingQuery(std::string_view shipping_query_id, std::true_type, const std::vector<ShippingOption> &shipping_options) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(shipping_query_id) << name_value_pair{"ok","true"}
            << NAME_VALUE_PAIR(shipping_options);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::answerPreCheckoutQuery(std::string_view pre_checkout_query_id, bool ok, o_sv error_message) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(ok)
            << NAME_VALUE_PAIR(pre_checkout_query_id) << NAME_VALUE_PAIR(error_message);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::answerPreCheckoutQuery(std::string_view pre_checkout_query_id, std::false_type, std::string_view error_message) const {
    query_builder builder;
    builder << name_value_pair{"ok","false"}
            << NAME_VALUE_PAIR(pre_checkout_query_id) << NAME_VALUE_PAIR(error_message);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::setPassportDataErrors(int64_t user_id, const std::vector<Bot::PassportElementError> &errors) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(errors);

    return api->call_api<bool>(__func__,builder);
}


std::pair<Message,o_error> Bot::sendGame(int64_t chat_id, std::string_view game_short_name, o_bool disable_notification, Bot::o_int32 duration, std::optional<InlineKeyboardMarkup> reply_markup) const  {
    query_builder builder;
    builder << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(game_short_name)
            << NAME_VALUE_PAIR(disable_notification) << NAME_VALUE_PAIR(duration)
            << NAME_VALUE_PAIR(reply_markup);

    return api->call_api<Message>(__func__,builder);
}

std::pair<std::variant<bool,Message>,o_error> Bot::setGameScore(int64_t user_id, int32_t score, std::string_view inline_message_id, o_int64 chat_id, o_int64 message_id, o_bool force, o_bool disable_edit_message) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(score)
            << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(force)
            << NAME_VALUE_PAIR(disable_edit_message);
    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<std::variant<bool,Message>,o_error> Bot::setGameScore(int64_t user_id, uint32_t score, int64_t chat_id, int64_t message_id, o_sv inline_message_id, o_bool force, o_bool disable_edit_message) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(score)
            << NAME_VALUE_PAIR(chat_id) << NAME_VALUE_PAIR(message_id)
            << NAME_VALUE_PAIR(inline_message_id) << NAME_VALUE_PAIR(force)
            << NAME_VALUE_PAIR(disable_edit_message);
    return api->call_api<std::variant<bool,Message>,Message>(__func__,builder);
}


std::pair<bool,o_error> Bot::getGameHighScores(int64_t user_id, int64_t chat_id, int64_t message_id, o_sv inline_message_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(chat_id)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::getGameHighScores(int64_t user_id, std::string_view inline_message_id, o_int64 chat_id, o_int64 message_id) const {
    query_builder builder;
    builder << NAME_VALUE_PAIR(user_id) << NAME_VALUE_PAIR(chat_id)
            << NAME_VALUE_PAIR(message_id) << NAME_VALUE_PAIR(inline_message_id);

    return api->call_api<bool>(__func__,builder);
}


std::pair<bool,o_error> Bot::deleteWebhook() const {
    return api->call_api<bool>(__func__);
}
}
