#include "telegram_bot.h"
#include "headers/querybuilder.h"
#include "headers/apimanager.h"
#include "utility/utility.h"

namespace telegram {

Bot::Bot(const std::string &token, std::size_t thread_number) noexcept
    : api{std::make_unique<ApiManager>("https://api.telegram.org/bot" + token +
                                       '/')},
    updater(std::max(thread_number,2ul)) {}

void Bot::onUpdate(UpdateCallback &&cb) {
  updater.setUpdateCallback(std::move(cb));
}

void Bot::onChosenInlineResult(std::string_view cmd,
                         ChosenInlineResultCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onQueryCallback(std::string_view cmd, QueryCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onMessage(std::string_view cmd, MessageCallback &&cb) {
  updater.addCallback(cmd, std::move(cb));
}

void Bot::onPreCheckoutQuery(std::string_view cmd,PreCheckoutQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}

void Bot::onInlineQuery(std::string_view cmd, InlineQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}

void Bot::onShippingQuery(std::string_view cmd,ShippingQueryCallback&& cb) {
    updater.addCallback(cmd, std::move(cb));
}
void Bot::stopSequence(int64_t user_id) { updater.removeSequence(user_id); }

std::pair<WebhookInfo, opt_error> Bot::getWebhookInfo() const {
  return api->ApiCall<WebhookInfo>(__func__);
}

void Bot::start(opt_uint64 timeout, opt_uint64 offset, opt_uint8 limit,
                std::optional<std::vector<std::string_view>> allowed_updates) {
  if (auto &&[webhook, Error] = getWebhookInfo();
      Error || webhook.url.size() || webhookSet) {
    if (Error)
      utility::Logger::warn(Error.value());
    utility::Logger::critical("You must remove webhook before using long polling method.");
    return;
  }
  stopPolling = false;
  updater.setOffset(offset.value_or(0));
  while (!stopPolling) {
    updater.routeCallback(getUpdatesRawJson(updater.getOffset(), limit, timeout,
                                            allowed_updates));
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout.value_or(1)));
  }
}
void Bot::stop() { stopPolling = true; }
std::pair<User, opt_error> Bot::getMe() const {
  return api->ApiCall<User>(__func__);
}

std::pair<Message, opt_error>
Bot::sendMessage(Bot::IntOrString chat_id, const std::string &text,
                 std::optional<ParseMode> parse_mode, opt_bool disable_web_page_preview,
                 opt_bool disable_notification, opt_int64 reply_to_message_id,
                 std::optional<Bot::ReplyMarkups> reply_markup) const {

  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(text)
          << make_named_pair(parse_mode)
          << make_named_pair(disable_web_page_preview)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error> Bot::reply(const Message &msg,
                                         const std::string &message) const {
  return sendMessage(msg.chat.id, message, {}, {}, {}, msg.message_id);
}

std::pair<Message, opt_error>
Bot::reply(const Message &msg, const std::string &text,
           const Bot::ReplyMarkups &markup) const {
  return sendMessage(msg.chat.id, text, {}, {}, {}, msg.message_id, markup);
}

std::pair<std::vector<Update>, opt_error> Bot::getUpdates(
    std::optional<uint32_t> offset, std::optional<uint8_t> limit,
    std::optional<uint32_t> timeout,
    std::optional<std::vector<std::string_view>> allowed_updates) const {
  QueryBuilder builder;
  builder << make_named_pair(offset) << make_named_pair(limit)
          << make_named_pair(timeout) << make_named_pair(allowed_updates);
  return api->ApiCall<std::vector<Update>>(__func__, builder);
}

std::string Bot::getUpdatesRawJson(
    std::optional<uint32_t> offset, std::optional<uint8_t> limit,
    std::optional<uint32_t> timeout,
    std::optional<std::vector<std::string_view>> allowed_updates) {
  QueryBuilder builder;
  builder << make_named_pair(offset) << make_named_pair(limit)
          << make_named_pair(timeout) << make_named_pair(allowed_updates);
  return api->ApiCallRaw("getUpdates", builder);
}

std::pair<Message, opt_error>
Bot::forwardMessage(Bot::IntOrString chat_id, Bot::IntOrString from_chat_id,
                    int64_t message_id, opt_bool disable_notification) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(from_chat_id)
          << make_named_pair(message_id)
          << make_named_pair(disable_notification);
  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendPhoto(Bot::IntOrString chat_id, const std::string &photo,
               std::string_view caption, std::optional<ParseMode> parse_mode,
               opt_bool disable_notification, opt_int64 reply_to_message_id,
               std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder, {make_named_pair(photo)});
}

std::pair<Message, opt_error>
Bot::sendAudio(Bot::IntOrString chat_id, std::string audio,
               opt_string_view caption, std::optional<ParseMode> parse_mode,
               std::optional<int> duration, opt_string_view performer,
               opt_string_view title, const std::optional<std::string> &thumb,
               opt_bool disable_notification, opt_int64 reply_to_message_id,
               std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode) << make_named_pair(duration)
          << make_named_pair(performer) << make_named_pair(title)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);
  std::vector<name_value_pair> params{make_named_pair(audio)};
  if (thumb.has_value())
    params.push_back(make_named_pair(thumb.value()));

  return api->ApiCall<Message>(__func__, builder, params);
}

std::pair<bool, opt_error> Bot::setWebhook(
    std::string_view url, const std::optional<std::string> &certificate,
    const std::optional<uint32_t> max_connections,
    const std::optional<std::vector<std::string_view>> &allowed_updates) const {
  QueryBuilder builder;
  builder << make_named_pair(url) << make_named_pair(max_connections)
          << make_named_pair(allowed_updates);
  if (certificate)
    return api->ApiCall<bool>(__func__, builder,
                              {make_named_pair(certificate.value())});
  else
    return api->ApiCall<bool>(__func__, builder);
}

bool Bot::setWebhookServer(const std::string &url, uint16_t port,
                           const std::string &cert_path,
                           const std::string &key_path) {

  webhookSet = true;

  // Send Telegram Bot Api request first
  auto [result, Error] =
      setWebhook(url + ':' + std::to_string(port), cert_path);
 // if result is not true
 if (!result)
    return false;
  // or if there is error
  if (Error) {
    utility::Logger::warn(Error->toString());
    return false;
  }

  httplib::SSLServer server(cert_path.data(), key_path.data());
  if (!server.is_valid()) {
    utility::Logger::critical("Server is not valid. Check certificate paths.");
    return false;
  }
  // one rule for routing
  server.Post("/", [&](const httplib::Request &req, httplib::Response &) {
    if (auto host = req.headers.find("REMOTE_ADDR");
        host != req.headers.end()) {
      uint32_t host_ip = NetworkManager::ipv4(host->second.data());
      // check if address is in one of two telegram subnets (filtering)
      if (host_ip != std::clamp(host_ip,
                                utility::telegram_first_subnet_range_begin,
                                utility::telegram_second_subned_range_end) &&
          host_ip != std::clamp(host_ip,
                                utility::teleram_second_subnet_range_begin,
                                utility::telegram_second_subned_range_end)) {
        utility::Logger::info("Request from unknown subnet", host_ip);
        return;
      }
    } else {
      utility::Logger::warn("Cannot resolve host ip");
      return;
    }
    updater.routeCallback(req.body);
  });
  return server.listen("0.0.0.0", port);
}

std::pair<Message, opt_error>
Bot::sendDocument(IntOrString chat_id, const std::string &document,
                  const std::optional<std::string> thumb,
                  opt_string_view caption, std::optional<ParseMode> parse_mode,
                  opt_bool disable_notification, opt_int64 reply_to_message_id,
                  std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  std::vector<name_value_pair> params{make_named_pair(document)};
  if (thumb.has_value())
    params.push_back(make_named_pair(thumb.value()));

  return api->ApiCall<Message>(__func__, builder, params);
}

std::pair<Message, opt_error>
Bot::sendVideo(IntOrString chat_id, const std::string &video,
               opt_int32 duration, opt_int32 width, opt_int32 height,
               const std::optional<std::string> &thumb, opt_string_view caption,
               std::optional<ParseMode> parse_mode, opt_bool supports_streaming,
               opt_bool disable_notification, opt_int64 reply_to_message_id,
               std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode) << make_named_pair(duration)
          << make_named_pair(width) << make_named_pair(height)
          << make_named_pair(supports_streaming)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  std::vector<name_value_pair> params{make_named_pair(video)};
  if (thumb.has_value())
    params.push_back(make_named_pair(thumb.value()));

  return api->ApiCall<Message>(__func__, builder, params);
}

std::pair<Message, opt_error>
Bot::sendAnimation(IntOrString chat_id, const std::string &animation,
                   opt_int32 duration, opt_int32 width, opt_int32 height,
                   const opt_string &thumb, opt_string_view caption,
                   std::optional<ParseMode> parse_mode, opt_bool supports_streaming,
                   opt_bool disable_nofitication, opt_int64 reply_to_message_id,
                   std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode) << make_named_pair(duration)
          << make_named_pair(width) << make_named_pair(height)
          << make_named_pair(supports_streaming)
          << make_named_pair(disable_nofitication)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  std::vector<name_value_pair> params{make_named_pair(animation)};
  if (thumb.has_value())
    params.push_back(make_named_pair(thumb.value()));

  return api->ApiCall<Message>(__func__, builder, params);
}

std::pair<Message, opt_error>
Bot::sendVoice(IntOrString chat_id, const std::string &voice,
               opt_string_view caption, std::optional<ParseMode> parse_mode,
               opt_int32 duration, opt_bool disable_notification,
               opt_int64 reply_to_message_id,
               std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(parse_mode) << make_named_pair(duration)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder, {make_named_pair(voice)});
}

std::pair<Message, opt_error>
Bot::sendVideoNote(IntOrString chat_id, const std::string &video_note,
                   opt_int32 duration, opt_int32 length,
                   const std::optional<std::string> &thumb,
                   opt_bool disable_notification, opt_int64 reply_to_message_id,
                   std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(duration)
          << make_named_pair(length) << make_named_pair(disable_notification)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  std::vector<name_value_pair> params{make_named_pair(video_note)};
  if (thumb.has_value())
    params.push_back(make_named_pair(thumb.value()));

  return api->ApiCall<Message>(__func__, builder, params);
}

std::pair<Message, opt_error>
Bot::sendLocation(IntOrString chat_id, float latitude, float longitude,
                  opt_int32 live_period, opt_bool disable_notification,
                  opt_int64 reply_to_message_id,
                  std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(latitude)
          << make_named_pair(disable_notification) << make_named_pair(longitude)
          << make_named_pair(live_period)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error> Bot::editMessageLiveLocation(
    IntOrString chat_id, int64_t message_id, float latitude, float longitude,
    opt_string_view inline_message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(latitude) << make_named_pair(longitude)
          << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);
  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error> Bot::editMessageLiveLocation(
    float latitude, float longitude, std::string_view inline_message_id,
    std::optional<Bot::IntOrString> chat_id, opt_int64 message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(latitude) << make_named_pair(longitude)
          << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);
  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error> Bot::stopMessageLiveLocation(
    IntOrString chat_id, int64_t message_id, opt_string_view inline_message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);
  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error> Bot::stopMessageLiveLocation(
    std::string_view inline_message_id, std::optional<Bot::IntOrString> chat_id,
    opt_int64 message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);
  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendVenue(Bot::IntOrString chat_id, float latitude, float longitude,
               std::string_view title, std::string_view address,
               opt_string_view foursquare_id, opt_string_view foursquare_type,
               opt_bool disable_notification, opt_int64 reply_to_message_id,
               std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(latitude)
          << make_named_pair(longitude) << make_named_pair(title)
          << make_named_pair(address) << make_named_pair(foursquare_id)
          << make_named_pair(foursquare_type)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendContact(Bot::IntOrString chat_id, std::string_view phone_number,
                 std::string_view first_name, opt_string_view last_name,
                 opt_string_view vcard, opt_bool disable_notification,
                 opt_int64 reply_to_message_id,
                 std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(phone_number)
          << make_named_pair(first_name) << make_named_pair(last_name)
          << make_named_pair(vcard) << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendPoll(Bot::IntOrString chat_id, std::string_view question,
              const std::vector<std::string_view> &options,
              opt_bool is_anonymous, opt_bool allows_multiple_answers,
              opt_string_view type, opt_int32 correct_option_id,
              opt_bool is_closed, opt_bool disable_notification,
              opt_int64 reply_to_message_id,
              std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id)
          << make_named_pair(allows_multiple_answers)
          << make_named_pair(question) << make_named_pair(options)
          << make_named_pair(is_anonymous) << make_named_pair(type)
          << make_named_pair(correct_option_id)
          << make_named_pair(disable_notification) << make_named_pair(is_closed)
          << make_named_pair(reply_markup)
          << make_named_pair(reply_to_message_id);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<bool, opt_error> Bot::sendChatAction(Bot::IntOrString chat_id,
                                               ChatAction action) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(action);
  return api->ApiCall<bool>(__func__, builder);
}

std::pair<UserProfilePhotos, opt_error>
Bot::getUserProfilePhotos(int64_t user_id, opt_int32 offset,
                          opt_int32 limit) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(offset)
          << make_named_pair(limit);

  return api->ApiCall<UserProfilePhotos>(__func__, builder);
}

std::pair<File, opt_error> Bot::getFile(std::string_view &file_id) const {
  QueryBuilder builder;
  builder << make_named_pair(file_id);

  return api->ApiCall<File>(__func__, builder);
}

std::pair<bool, opt_error> Bot::kickChatMember(Bot::IntOrString chat_id,
                                               int64_t user_id,
                                               opt_int32 until_date) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id)
          << make_named_pair(until_date);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::unbanChatMember(Bot::IntOrString chat_id,
                                                int64_t user_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::restrictChatMember(Bot::IntOrString chat_id,
                                                   int64_t user_id,
                                                   const ChatPermissions &perms,
                                                   opt_int64 until_date) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id)
          << make_named_pair(perms) << make_named_pair(until_date);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::promoteChatMember(Bot::IntOrString chat_id, int64_t user_id,
                       opt_bool can_change_info, opt_bool can_post_messages,
                       opt_bool can_edit_messages, opt_bool can_delete_messages,
                       opt_bool can_invite_users, opt_bool can_restrict_members,
                       opt_bool can_pin_messages,
                       opt_bool can_promote_members) {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id)
          << make_named_pair(can_change_info)
          << make_named_pair(can_post_messages)
          << make_named_pair(can_delete_messages)
          << make_named_pair(can_invite_users)
          << make_named_pair(can_restrict_members)
          << make_named_pair(can_pin_messages)
          << make_named_pair(can_promote_members)
          << make_named_pair(can_edit_messages);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::setChatAdministratorCustomTitle(Bot::IntOrString chat_id, int64_t user_id,
                                     std::string_view custom_title) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id)
          << make_named_pair(custom_title);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::setChatPermissions(Bot::IntOrString chat_id,
                        const ChatPermissions &permissions) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(permissions);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<std::string, opt_error>
Bot::exportChatInviteLink(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<std::string>(__func__, builder);
}

std::pair<bool, opt_error> Bot::setChatPhoto(Bot::IntOrString chat_id,
                                             const std::string &photo) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);
  return api->ApiCall<bool>(__func__, builder, {make_named_pair(photo)});
}

std::pair<bool, opt_error>
Bot::deleteChatPhoto(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::setChatTitle(Bot::IntOrString chat_id,
                                             std::string_view title) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(title);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::setChatDescription(Bot::IntOrString chat_id,
                        opt_string_view description) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(description);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::pinChatMessage(Bot::IntOrString chat_id, int64_t message_id,
                    opt_bool disable_notification) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(disable_notification);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::unpinChatMessage(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::leaveChat(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);
  return api->ApiCall<bool>(__func__, builder);
}

std::pair<Chat, opt_error> Bot::getChat(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<Chat>(__func__, builder);
}

std::pair<std::vector<ChatMember>, opt_error>
Bot::getChatAdministrators(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<std::vector<ChatMember>>(__func__, builder);
}

std::pair<uint32_t, opt_error>
Bot::getChatMembersCount(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);
  return api->ApiCall<uint32_t>(__func__, builder);
}

std::pair<ChatMember, opt_error> Bot::getChatMember(Bot::IntOrString chat_id,
                                                    uint32_t user_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(user_id);

  return api->ApiCall<ChatMember>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::setChatStickerSet(Bot::IntOrString chat_id,
                       std::string_view sticker_set_name) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(sticker_set_name);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::deleteChatStickerSet(Bot::IntOrString chat_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::answerCallbackQuery(std::string_view callback_query_id,
                         opt_string_view text, opt_bool show_alert,
                         opt_string_view url, opt_int32 cache_time) const {
  QueryBuilder builder;
  builder << make_named_pair(callback_query_id) << make_named_pair(text)
          << make_named_pair(show_alert) << make_named_pair(url)
          << make_named_pair(cache_time);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::editMessageText(Bot::IntOrString chat_id, std::string_view text,
                     opt_int64 message_id, opt_string_view inline_message_id,
                     std::optional<ParseMode> parse_mode,
                     opt_bool disable_web_page_preview,
                     std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(text)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(parse_mode)
          << make_named_pair(disable_web_page_preview)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error> Bot::editMessageText(
    std::string_view text, const std::string &inline_message_id,
    std::optional<Bot::IntOrString> chat_id, opt_int64 message_id,
    std::optional<ParseMode> parse_mode, opt_bool disable_web_page_preview,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(text)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(parse_mode)
          << make_named_pair(disable_web_page_preview)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::editMessageCaption(Bot::IntOrString chat_id, opt_int64 message_id,
                        opt_string_view inline_message_id,
                        opt_string_view caption, std::optional<ParseMode> parse_mode,
                        std::optional<InlineKeyboardMarkup> reply_markup) {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(parse_mode) << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error> Bot::editMessageCaption(
    const std::string &inline_message_id,
    std::optional<Bot::IntOrString> chat_id, opt_int64 message_id,
    opt_string_view caption, std::optional<ParseMode> parse_mode,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(caption)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(parse_mode) << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::editMessageMedia(Bot::InputMedia media, opt_int64 inline_message_id,
                      std::optional<Bot::IntOrString> chat_id,
                      opt_int64 message_id,
                      std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(media) << make_named_pair(chat_id)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::editMessageMedia(Bot::InputMedia media, Bot::IntOrString chat_id,
                      int64_t message_id, opt_string_view inline_message_id,
                      std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(media) << make_named_pair(chat_id)
          << make_named_pair(message_id) << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error> Bot::editMessageReplyMarkup(
    std::string_view inline_message_id, std::optional<Bot::IntOrString> chat_id,
    opt_int64 message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error> Bot::editMessageReplyMarkup(
    Bot::IntOrString chat_id, int64_t message_id,
    std::string_view inline_message_id,
    std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<bool, opt_error> Bot::deleteMessage(Bot::IntOrString chat_id,
                                              int64_t message_id) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<Poll, opt_error>
Bot::stopPoll(Bot::IntOrString chat_id, int64_t message_id,
              std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Poll>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendSticker(Bot::IntOrString chat_id, const std::string &sticker,
                 opt_bool disable_notification, opt_int64 reply_to_message_id,
                 std::optional<Bot::ReplyMarkups> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder, {make_named_pair(sticker)});
}

std::pair<StickerSet, opt_error>
Bot::getStickerSet(const std::string &name) const {
  QueryBuilder builder;
  builder << make_named_pair(name);

  return api->ApiCall<StickerSet>(__func__, builder);
}

std::pair<File, opt_error>
Bot::uploadStickerFile(int64_t user_id, const std::string &png_sticker) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id);
  return api->ApiCall<File>(__func__, builder, {make_named_pair(png_sticker)});
}

std::pair<bool, opt_error> Bot::createNewStickerSet(
    int64_t user_id, std::string_view name, std::string_view title,
    const std::string &png_sticker, std::string_view emojis,
    opt_bool contains_masks,
    const std::optional<MaskPosition> &mask_position) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(name)
          << make_named_pair(title) << make_named_pair(emojis)
          << make_named_pair(contains_masks) << make_named_pair(mask_position);

  return api->ApiCall<bool>(__func__, builder, {make_named_pair(png_sticker)});
}

std::pair<bool, opt_error>
Bot::addStickerToSet(int64_t user_id, std::string_view name,
                     const std::string &png_sticker, std::string_view emojis,
                     const std::optional<MaskPosition> &mask_position) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(name)
          << make_named_pair(emojis) << make_named_pair(mask_position);

  return api->ApiCall<bool>(__func__, builder, {make_named_pair(png_sticker)});
}

std::pair<bool, opt_error>
Bot::setStickerPositionInSet(std::string_view sticker, int32_t position) const {
  QueryBuilder builder;
  builder << make_named_pair(sticker) << make_named_pair(position);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::deleteStickerFromSet(const std::string &sticker) const {
  QueryBuilder builder;
  builder << make_named_pair(sticker);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<Message, opt_error> Bot::sendInvoice(
    int64_t chat_id, std::string_view title, std::string_view description,
    std::string_view payload, std::string_view provider_token,
    std::string_view start_parameter, std::string_view currency,
    const std::vector<LabeledPrice> &prices, opt_string_view provider_data,
    opt_string_view photo_url, opt_int32 photo_size, opt_int32 photo_width,
    opt_int32 photo_height, opt_bool need_name, opt_bool need_phone_number,
    opt_bool need_email, opt_bool need_shipping_address,
    opt_bool send_phone_number_to_provider, opt_bool send_email_to_provider,
    opt_bool is_flexible, opt_bool disable_notification,
    opt_int32 reply_to_message_id,
    const std::optional<InlineKeyboardMarkup> &reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(title)
          << make_named_pair(description) << make_named_pair(payload)
          << make_named_pair(provider_token) << make_named_pair(start_parameter)
          << make_named_pair(currency) << make_named_pair(prices)
          << make_named_pair(provider_data) << make_named_pair(photo_url)
          << make_named_pair(photo_size) << make_named_pair(photo_width)
          << make_named_pair(photo_height) << make_named_pair(need_name)
          << make_named_pair(need_phone_number) << make_named_pair(need_email)
          << make_named_pair(need_shipping_address)
          << make_named_pair(send_phone_number_to_provider)
          << make_named_pair(send_email_to_provider)
          << make_named_pair(is_flexible)
          << make_named_pair(disable_notification)
          << make_named_pair(reply_to_message_id)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::answerShippingQuery(std::string_view shipping_query_id, std::false_type,
                         std::string_view error_message) const {
  QueryBuilder builder;
  builder << make_named_pair(shipping_query_id)
          << name_value_pair{"ok", "false"} << make_named_pair(error_message);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::answerShippingQuery(
    std::string_view shipping_query_id, std::true_type,
    const std::vector<ShippingOption> &shipping_options) const {
  QueryBuilder builder;
  builder << make_named_pair(shipping_query_id) << name_value_pair{"ok", "true"}
          << make_named_pair(shipping_options);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::answerPreCheckoutQuery(std::string_view pre_checkout_query_id, bool ok,
                            opt_string_view error_message) const {
  QueryBuilder builder;
  builder << make_named_pair(ok) << make_named_pair(pre_checkout_query_id)
          << make_named_pair(error_message);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::answerPreCheckoutQuery(std::string_view pre_checkout_query_id,
                            std::false_type,
                            std::string_view error_message) const {
  QueryBuilder builder;
  builder << name_value_pair{"ok", "false"}
          << make_named_pair(pre_checkout_query_id)
          << make_named_pair(error_message);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::setPassportDataErrors(
    int64_t user_id,
    const std::vector<Bot::PassportElementError> &errors) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(errors);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<Message, opt_error>
Bot::sendGame(int64_t chat_id, std::string_view game_short_name,
              opt_bool disable_notification, opt_int32 duration,
              std::optional<InlineKeyboardMarkup> reply_markup) const {
  QueryBuilder builder;
  builder << make_named_pair(chat_id) << make_named_pair(game_short_name)
          << make_named_pair(disable_notification) << make_named_pair(duration)
          << make_named_pair(reply_markup);

  return api->ApiCall<Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::setGameScore(int64_t user_id, int32_t score,
                  std::string_view inline_message_id, opt_int64 chat_id,
                  opt_int64 message_id, opt_bool force,
                  opt_bool disable_edit_message) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(score)
          << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id) << make_named_pair(force)
          << make_named_pair(disable_edit_message);
  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<std::variant<bool, Message>, opt_error>
Bot::setGameScore(int64_t user_id, uint32_t score, int64_t chat_id,
                  int64_t message_id, opt_string_view inline_message_id,
                  opt_bool force, opt_bool disable_edit_message) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(score)
          << make_named_pair(chat_id) << make_named_pair(message_id)
          << make_named_pair(inline_message_id) << make_named_pair(force)
          << make_named_pair(disable_edit_message);
  return api->ApiCall<std::variant<bool, Message>, Message>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::getGameHighScores(int64_t user_id, int64_t chat_id, int64_t message_id,
                       opt_string_view inline_message_id) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(chat_id)
          << make_named_pair(message_id) << make_named_pair(inline_message_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error>
Bot::getGameHighScores(int64_t user_id, std::string_view inline_message_id,
                       opt_int64 chat_id, opt_int64 message_id) const {
  QueryBuilder builder;
  builder << make_named_pair(user_id) << make_named_pair(chat_id)
          << make_named_pair(message_id) << make_named_pair(inline_message_id);

  return api->ApiCall<bool>(__func__, builder);
}

std::pair<bool, opt_error> Bot::deleteWebhook() const {
  return api->ApiCall<bool>(__func__);
}
std::pair<Message,opt_error>
Bot::sendDice(IntOrString chat_id,opt_bool disable_notification,
         opt_int64 reply_to_message_id,
              std::optional<ReplyMarkups> reply_markup) const {
    QueryBuilder builder;
    builder << make_named_pair(chat_id) << make_named_pair(disable_notification)
            << make_named_pair(reply_to_message_id) << make_named_pair(reply_markup);
    return api->ApiCall<Message>(__func__,builder);
}

std::pair<bool,opt_error>
Bot::setMyCommands(const std::vector<BotCommand>& commands) const {
    QueryBuilder builder;
    builder << make_named_pair(commands);
    return api->ApiCall<bool>(__func__,builder);
}

std::pair<std::vector<BotCommand>,opt_error> Bot::getMyCommands() const {
   return api->ApiCall<std::vector<BotCommand>>(__func__);
}

} // namespace telegram
