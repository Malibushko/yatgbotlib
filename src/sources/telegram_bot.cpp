#include <spdlog/spdlog.h>

#include "include/telegram_bot.h"
#include "headers/querybuilder.h"
#include "headers/apimanager.h"
#include "utility/utility.h"

namespace telegram {

Bot::Bot(const std::string &token, std::size_t thread_number) noexcept
    : api{std::make_unique<ApiManager>("https://api.telegram.org/bot" + token +
                                       '/')},
      updater(std::max(thread_number,std::size_t{2})) {
    utility::Logger::info(fmt::format("Threadpool capacity set to {}",std::max(thread_number,std::size_t{2})));
}

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
void Bot::stopSequence(int64_t user_id) {
    utility::Logger::info(fmt::format("Removed sequence for user #{}",user_id));
    updater.removeSequence(user_id);
}

void Bot::start(std::optional<int64_t> timeout, std::optional<int64_t> offset, std::optional<int8_t> limit,
                std::optional<std::vector<std::string_view>> allowed_updates) {
  if (auto &&[webhook, Error] = getWebhookInfo();
      Error || webhook.url.size() || webhookSet) {
    if (Error) {
      utility::Logger::warn(Error.value());
    }
    utility::Logger::critical("You must remove webhook before using long polling method.");
    return;
  }
  stopPolling = false;
  updater.setOffset(offset.value_or(0));
  utility::Logger::info("Bot started");
  while (!stopPolling) {
    updater.routeCallback(getUpdatesRawJson(updater.getOffset(), limit, timeout,
                                            allowed_updates));
    std::this_thread::sleep_for(std::chrono::milliseconds(timeout.value_or(1)));
  }
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
void Bot::stop() {
    utility::Logger::info("Stopping bot.");
    stopPolling = true;
}

std::pair<Message, opt_error> Bot::reply(const Message &msg,
                                         const std::string &message) const {
  return sendMessage(msg.chat.id, message, {}, {}, {}, msg.message_id);
}

bool Bot::setWebhookServer(const std::string &url, uint16_t port,
                           const std::string &cert_path,
                           const std::string &key_path) {

  webhookSet = true;
  // Send Telegram Bot Api request first
  auto [result, Error] =
      setWebhook(url + ':' + std::to_string(port), cert_path);
 // if result is not true
 if (!result) {
      utility::Logger::warn("Failed to set webhook. Check certificates paths are valid.");
      return false;
  }
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
        utility::Logger::warn("Request from unknown subnet", host_ip);
        return;
      }
    } else {
      utility::Logger::warn("Cannot resolve host ip");
      return;
    }
    updater.routeCallback(req.body);
  });
  utility::Logger::info(fmt::format("Server started on port [{}]",port));
  return server.listen("0.0.0.0", port);
}

// --------------------------- AUTOGENERAGED CODE ------------------------------


std::pair<std::vector<Update>,opt_error> Bot::getUpdates ( 
															std::optional<int64_t>	offset,
															std::optional<int64_t>	limit,
															std::optional<int64_t>	timeout,
															const std::optional<std::vector<std::string>>& 	allowed_updates
) const {
QueryBuilder builder;
builder << make_named_pair(offset)
 << make_named_pair(limit)
 << make_named_pair(timeout)
 << make_named_pair(allowed_updates);
;
return api->ApiCall<std::vector<Update>>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setWebhook ( 
															const std::string& 	url,
															const std::optional<std::string>& 	certificate,
															std::optional<int64_t>	max_connections,
															const std::optional<std::vector<std::string>>& 	allowed_updates
) const {
QueryBuilder builder;
builder << make_named_pair(url)
 << make_named_pair(max_connections)
 << make_named_pair(allowed_updates);
;
std::vector<name_value_pair> params;
params.reserve(1);
if (certificate.has_value())
	params.push_back(name_value_pair{"certificate",std::move(certificate.value())});
return api->ApiCall<bool>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::deleteWebhook ( 

) const {
return api->ApiCall<bool>(__func__);
}
std::pair<WebhookInfo,opt_error> Bot::getWebhookInfo ( 

) const {
return api->ApiCall<WebhookInfo>(__func__);
}
std::pair<User,opt_error> Bot::getMe ( 

) const {
return api->ApiCall<User>(__func__);
}
std::pair<Message,opt_error> Bot::sendMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	text,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	disable_web_page_preview,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(text)
 << make_named_pair(parse_mode)
 << make_named_pair(disable_web_page_preview)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::forwardMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::variant<int64_t,std::string>& 	from_chat_id,
															int64_t	message_id,
															std::optional<bool>	disable_notification
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(from_chat_id)
 << make_named_pair(disable_notification)
 << make_named_pair(message_id);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	photo,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(1);
params.push_back(name_value_pair{"photo",std::move(photo)});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendAudio ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	audio,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<int64_t>	duration,
															const std::optional<std::string>& 	performer,
															const std::optional<std::string>& 	title,
															const std::optional<std::string>& 	thumb,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(duration)
 << make_named_pair(performer)
 << make_named_pair(title)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(2);
params.push_back(name_value_pair{"audio",std::move(audio)});
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendDocument ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	document,
															const std::optional<std::string>& 	thumb,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(2);
params.push_back(name_value_pair{"document",std::move(document)});
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendVideo ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	video,
															std::optional<int64_t>	duration,
															std::optional<int64_t>	width,
															std::optional<int64_t>	height,
															const std::optional<std::string>& 	thumb,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	supports_streaming,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(duration)
 << make_named_pair(width)
 << make_named_pair(height)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(supports_streaming)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(2);
params.push_back(name_value_pair{"video",std::move(video)});
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendAnimation ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	animation,
															std::optional<int64_t>	duration,
															std::optional<int64_t>	width,
															std::optional<int64_t>	height,
															const std::optional<std::string>& 	thumb,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(duration)
 << make_named_pair(width)
 << make_named_pair(height)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(2);
params.push_back(name_value_pair{"animation",std::move(animation)});
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendVoice ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	voice,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															std::optional<int64_t>	duration,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(duration)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(1);
params.push_back(name_value_pair{"voice",std::move(voice)});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendVideoNote ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	video_note,
															std::optional<int64_t>	duration,
															std::optional<int64_t>	length,
															const std::optional<std::string>& 	thumb,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(duration)
 << make_named_pair(length)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(2);
params.push_back(name_value_pair{"video_note",std::move(video_note)});
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<Message,opt_error> Bot::sendLocation ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															float	latitude,
															float	longitude,
															std::optional<int64_t>	live_period,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(latitude)
 << make_named_pair(longitude)
 << make_named_pair(live_period)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::editMessageLiveLocation ( 
															float	latitude,
															float	longitude,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(latitude)
 << make_named_pair(longitude)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::stopMessageLiveLocation ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendVenue ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															float	latitude,
															float	longitude,
															const std::string& 	title,
															const std::string& 	address,
															const std::optional<std::string>& 	foursquare_id,
															const std::optional<std::string>& 	foursquare_type,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(latitude)
 << make_named_pair(longitude)
 << make_named_pair(title)
 << make_named_pair(address)
 << make_named_pair(foursquare_id)
 << make_named_pair(foursquare_type)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendContact ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	phone_number,
															const std::string& 	first_name,
															const std::optional<std::string>& 	last_name,
															const std::optional<std::string>& 	vcard,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(phone_number)
 << make_named_pair(first_name)
 << make_named_pair(last_name)
 << make_named_pair(vcard)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendPoll ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	question,
															const std::vector<std::string>& 	options,
															std::optional<bool>	is_anonymous,
															const std::optional<std::string>& 	type,
															std::optional<bool>	allows_multiple_answers,
															std::optional<int64_t>	correct_option_id,
															const std::optional<std::string>& 	explanation,
															const std::optional<std::string>& 	explanation_parse_mode,
															std::optional<int64_t>	open_period,
															std::optional<int64_t>	close_date,
															std::optional<bool>	is_closed,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(question)
 << make_named_pair(options)
 << make_named_pair(is_anonymous)
 << make_named_pair(type)
 << make_named_pair(allows_multiple_answers)
 << make_named_pair(correct_option_id)
 << make_named_pair(explanation)
 << make_named_pair(explanation_parse_mode)
 << make_named_pair(open_period)
 << make_named_pair(close_date)
 << make_named_pair(is_closed)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendDice ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::optional<std::string>& 	emoji,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(emoji)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<bool,opt_error> Bot::sendChatAction ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	action
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(action);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<UserProfilePhotos,opt_error> Bot::getUserProfilePhotos ( 
															int64_t	user_id,
															std::optional<int64_t>	offset,
															std::optional<int64_t>	limit
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(offset)
 << make_named_pair(limit);
;
return api->ApiCall<UserProfilePhotos>(__func__,builder);
}
std::pair<File,opt_error> Bot::getFile ( 
															const std::string& 	file_id
) const {
QueryBuilder builder;
builder << make_named_pair(file_id);
;
return api->ApiCall<File>(__func__,builder);
}
std::pair<bool,opt_error> Bot::kickChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															std::optional<int64_t>	until_date
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id)
 << make_named_pair(until_date);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::unbanChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<std::variant<bool>,opt_error> Bot::restrictChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															const ChatPermissions& 	permissions,
															std::optional<int64_t>	until_date
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id)
 << make_named_pair(permissions)
 << make_named_pair(until_date);
;
return api->ApiCall<std::variant<bool>,bool>(__func__,builder);
}
std::pair<std::variant<bool>,opt_error> Bot::promoteChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															std::optional<bool>	can_change_info,
															std::optional<bool>	can_post_messages,
															std::optional<bool>	can_edit_messages,
															std::optional<bool>	can_delete_messages,
															std::optional<bool>	can_invite_users,
															std::optional<bool>	can_restrict_members,
															std::optional<bool>	can_pin_messages,
															std::optional<bool>	can_promote_members
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id)
 << make_named_pair(can_change_info)
 << make_named_pair(can_post_messages)
 << make_named_pair(can_edit_messages)
 << make_named_pair(can_delete_messages)
 << make_named_pair(can_invite_users)
 << make_named_pair(can_restrict_members)
 << make_named_pair(can_pin_messages)
 << make_named_pair(can_promote_members);
;
return api->ApiCall<std::variant<bool>,bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatAdministratorCustomTitle ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															const std::string& 	custom_title
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id)
 << make_named_pair(custom_title);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatPermissions ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const ChatPermissions& 	permissions
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(permissions);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<std::string,opt_error> Bot::exportChatInviteLink ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<std::string>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	photo
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
;
std::vector<name_value_pair> params;
params.reserve(1);
params.push_back(name_value_pair{"photo",std::move(photo)});
return api->ApiCall<bool>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::deleteChatPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatTitle ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	title
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(title);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatDescription ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::optional<std::string>& 	description
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(description);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::pinChatMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id,
															std::optional<bool>	disable_notification
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(disable_notification);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::unpinChatMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::leaveChat ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<Chat,opt_error> Bot::getChat ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<Chat>(__func__,builder);
}
std::pair<std::vector<ChatMember>,opt_error> Bot::getChatAdministrators ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<std::vector<ChatMember>>(__func__,builder);
}
std::pair<int64_t,opt_error> Bot::getChatMembersCount ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<int64_t>(__func__,builder);
}
std::pair<ChatMember,opt_error> Bot::getChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(user_id);
;
return api->ApiCall<ChatMember>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setChatStickerSet ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	sticker_set_name
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(sticker_set_name);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::deleteChatStickerSet ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::answerCallbackQuery ( 
															const std::string& 	callback_query_id,
															const std::optional<std::string>& 	text,
															std::optional<bool>	show_alert,
															const std::optional<std::string>& 	url,
															std::optional<int64_t>	cache_time
) const {
QueryBuilder builder;
builder << make_named_pair(callback_query_id)
 << make_named_pair(text)
 << make_named_pair(show_alert)
 << make_named_pair(url)
 << make_named_pair(cache_time);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setMyCommands ( 
															const std::vector<BotCommand>& 	commands
) const {
QueryBuilder builder;
builder << make_named_pair(commands);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<std::vector<BotCommand>,opt_error> Bot::getMyCommands ( 

) const {
return api->ApiCall<std::vector<BotCommand>>(__func__);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::editMessageText ( 
															const std::string& 	text,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<std::string>& 	parse_mode,
															std::optional<bool>	disable_web_page_preview,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(text)
 << make_named_pair(parse_mode)
 << make_named_pair(disable_web_page_preview)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::editMessageCaption ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<std::string>& 	caption,
															const std::optional<std::string>& 	parse_mode,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(caption)
 << make_named_pair(parse_mode)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::editMessageMedia ( 
															const InputMedia& 	media,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(media)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::editMessageReplyMarkup ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<Poll,opt_error> Bot::stopPoll ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Poll>(__func__,builder);
}
std::pair<bool,opt_error> Bot::deleteMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(message_id);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendSticker ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	sticker,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
std::vector<name_value_pair> params;
params.reserve(1);
params.push_back(name_value_pair{"sticker",std::move(sticker)});
return api->ApiCall<Message>(__func__,builder,params);
}
std::pair<StickerSet,opt_error> Bot::getStickerSet ( 
															const std::string& 	name
) const {
QueryBuilder builder;
builder << make_named_pair(name);
;
return api->ApiCall<StickerSet>(__func__,builder);
}
std::pair<File,opt_error> Bot::uploadStickerFile ( 
															int64_t	user_id,
															const std::string& 	png_sticker
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
;
std::vector<name_value_pair> params;
params.reserve(1);
params.push_back(name_value_pair{"png_sticker",std::move(png_sticker)});
return api->ApiCall<File>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::createNewStickerSet ( 
															int64_t	user_id,
															const std::string& 	name,
															const std::string& 	title,
															const std::string& 	emojis,
															const std::optional<std::string>& 	png_sticker,
															const std::optional<std::string>& 	tgs_sticker,
															std::optional<bool>	contains_masks,
															const std::optional<MaskPosition>& 	mask_position
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(name)
 << make_named_pair(title)
 << make_named_pair(emojis)
 << make_named_pair(contains_masks)
 << make_named_pair(mask_position);
;
std::vector<name_value_pair> params;
params.reserve(2);
if (png_sticker.has_value())
	params.push_back(name_value_pair{"png_sticker",std::move(png_sticker.value())});
if (tgs_sticker.has_value())
	params.push_back(name_value_pair{"tgs_sticker",std::move(tgs_sticker.value())});
return api->ApiCall<bool>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::addStickerToSet ( 
															int64_t	user_id,
															const std::string& 	name,
															const std::string& 	emojis,
															const std::optional<std::string>& 	png_sticker,
															const std::optional<std::string>& 	tgs_sticker,
															const std::optional<MaskPosition>& 	mask_position
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(name)
 << make_named_pair(emojis)
 << make_named_pair(mask_position);
;
std::vector<name_value_pair> params;
params.reserve(2);
if (png_sticker.has_value())
	params.push_back(name_value_pair{"png_sticker",std::move(png_sticker.value())});
if (tgs_sticker.has_value())
	params.push_back(name_value_pair{"tgs_sticker",std::move(tgs_sticker.value())});
return api->ApiCall<bool>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::setStickerPositionInSet ( 
															const std::string& 	sticker,
															int64_t	position
) const {
QueryBuilder builder;
builder << make_named_pair(sticker)
 << make_named_pair(position);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::deleteStickerFromSet ( 
															const std::string& 	sticker
) const {
QueryBuilder builder;
builder << make_named_pair(sticker);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setStickerSetThumb ( 
															const std::string& 	name,
															int64_t	user_id,
															const std::optional<std::string>& 	thumb
) const {
QueryBuilder builder;
builder << make_named_pair(name)
 << make_named_pair(user_id)
;
std::vector<name_value_pair> params;
params.reserve(1);
if (thumb.has_value())
	params.push_back(name_value_pair{"thumb",std::move(thumb.value())});
return api->ApiCall<bool>(__func__,builder,params);
}
std::pair<bool,opt_error> Bot::answerInlineQuery ( 
															const std::string& 	inline_query_id,
															const std::vector<InlineQueryResult>& 	results,
															std::optional<int64_t>	cache_time,
															std::optional<bool>	is_personal,
															const std::optional<std::string>& 	next_offset,
															const std::optional<std::string>& 	switch_pm_text,
															const std::optional<std::string>& 	switch_pm_parameter
) const {
QueryBuilder builder;
builder << make_named_pair(inline_query_id)
 << make_named_pair(results)
 << make_named_pair(cache_time)
 << make_named_pair(is_personal)
 << make_named_pair(next_offset)
 << make_named_pair(switch_pm_text)
 << make_named_pair(switch_pm_parameter);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendInvoice ( 
															int64_t	chat_id,
															const std::string& 	title,
															const std::string& 	description,
															const std::string& 	payload,
															const std::string& 	provider_token,
															const std::string& 	start_parameter,
															const std::string& 	currency,
															const std::vector<LabeledPrice>& 	prices,
															const std::optional<std::string>& 	provider_data,
															const std::optional<std::string>& 	photo_url,
															std::optional<int64_t>	photo_size,
															std::optional<int64_t>	photo_width,
															std::optional<int64_t>	photo_height,
															std::optional<bool>	need_name,
															std::optional<bool>	need_phone_number,
															std::optional<bool>	need_email,
															std::optional<bool>	need_shipping_address,
															std::optional<bool>	send_phone_number_to_provider,
															std::optional<bool>	send_email_to_provider,
															std::optional<bool>	is_flexible,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(title)
 << make_named_pair(description)
 << make_named_pair(payload)
 << make_named_pair(provider_token)
 << make_named_pair(start_parameter)
 << make_named_pair(currency)
 << make_named_pair(prices)
 << make_named_pair(provider_data)
 << make_named_pair(photo_url)
 << make_named_pair(photo_size)
 << make_named_pair(photo_width)
 << make_named_pair(photo_height)
 << make_named_pair(need_name)
 << make_named_pair(need_phone_number)
 << make_named_pair(need_email)
 << make_named_pair(need_shipping_address)
 << make_named_pair(send_phone_number_to_provider)
 << make_named_pair(send_email_to_provider)
 << make_named_pair(is_flexible)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<bool,opt_error> Bot::answerShippingQuery ( 
															const std::string& 	shipping_query_id,
															bool	ok,
															const std::optional<std::vector<ShippingOption>>& 	shipping_options,
															const std::optional<std::string>& 	error_message
) const {
QueryBuilder builder;
builder << make_named_pair(shipping_query_id)
 << make_named_pair(ok)
 << make_named_pair(shipping_options)
 << make_named_pair(error_message);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::answerPreCheckoutQuery ( 
															const std::string& 	pre_checkout_query_id,
															bool	ok,
															const std::optional<std::string>& 	error_message
) const {
QueryBuilder builder;
builder << make_named_pair(pre_checkout_query_id)
 << make_named_pair(ok)
 << make_named_pair(error_message);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<bool,opt_error> Bot::setPassportDataErrors ( 
															int64_t	user_id,
															const std::vector<PassportElementError>& 	errors
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(errors);
;
return api->ApiCall<bool>(__func__,builder);
}
std::pair<Message,opt_error> Bot::sendGame ( 
															int64_t	chat_id,
															const std::string& 	game_short_name,
															std::optional<bool>	disable_notification,
															std::optional<int64_t>	reply_to_message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup
) const {
QueryBuilder builder;
builder << make_named_pair(chat_id)
 << make_named_pair(game_short_name)
 << make_named_pair(disable_notification)
 << make_named_pair(reply_to_message_id)
 << make_named_pair(reply_markup);
;
return api->ApiCall<Message>(__func__,builder);
}
std::pair<std::variant<bool, Message>,opt_error> Bot::setGameScore ( 
															int64_t	user_id,
															int64_t	score,
															std::optional<bool>	force,
															std::optional<bool>	disable_edit_message,
															std::optional<int64_t>	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(score)
 << make_named_pair(force)
 << make_named_pair(disable_edit_message)
 << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id);
;
return api->ApiCall<std::variant<bool, Message>,Message>(__func__,builder);
}
std::pair<std::vector<GameHighScore>,opt_error> Bot::getGameHighScores ( 
															int64_t	user_id,
															std::optional<int64_t>	chat_id,
															std::optional<int64_t>	message_id,
															const std::optional<std::string>& 	inline_message_id
) const {
QueryBuilder builder;
builder << make_named_pair(user_id)
 << make_named_pair(chat_id)
 << make_named_pair(message_id)
 << make_named_pair(inline_message_id);
;
return api->ApiCall<std::vector<GameHighScore>>(__func__,builder);
}
}
