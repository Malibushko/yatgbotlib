#pragma once
#include <cassert>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <variant>

#include "headers/update_manager.h"
#include "headers/apimanager.h"

namespace telegram {
using opt_error = std::optional<Error>;
/**
 * @brief The Telegram Bot class that contains methods that implements Telegram Bot Api
 * This class also contain some additional methods to add sequences or callbacks
 *
 *
 * The implementation of this class is rather trivial, if you look at any API method
 * it would look like this
 *
 * std::pair<T,opt_error> method(args) {
 *  QueryBuilder builder;
 *  builder << make_named_pair(args);
 *  return ApiCall(__func__,method);
 * }
 *
 * Explanation:
 * 1) Every API method returns pair of type and error. Users are supposed to always
 * check if error contains value (because in that case there will be default constucted T)
 * 2) Args are always named as required in API
 * 3) make_named_pair is a macro, that produces a pair of arg and it`s name,
 * so users MUST NOT change arg names
 * 4) ApiCall is function from ApiManager class, that uses first parameter and a
 * name of method from Telegram Bot Api, so users MUST NOT change function names.
 */

class Bot {
  std::unique_ptr<ApiManager> api;
  UpdateManager updater;
  bool stopPolling = false;
  bool webhookSet = false;

public:
  Bot(const std::string &token,std::size_t thread_number = std::thread::hardware_concurrency()) noexcept;
  /**
   * @brief Set callback for Updates
   * \warning there can be only ONE callback for Updates
   */
  void onUpdate(UpdateCallback &&cb);

  /**
   * @brief set callback for ChosenInlineResult
   * @param cmd - data that will trigger the callback
   * @param cb - callback function that must follow void(ChosenInlineResult&&) signature
   */
  void onChosenInlineResult(std::string_view cmd, ChosenInlineResultCallback &&cb);

  /**
   * @brief set callback for CallbackQuery
   * @param cmd - data that will trigger the callback
   * @param cb - callback function that must follow void(CallbackQuery&&) signature
   */
  void onQueryCallback(std::string_view cmd, QueryCallback &&cb);

  /**
   * @brief set callback for Message (preferrably commands like /help)
   * @param cmd - text -in message that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onMessage(std::string_view cmd, MessageCallback &&cb);

  /**
   * @brief set callback for InlineQuery
   * @param cmd - query in InlineQuery that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onInlineQuery(std::string_view cmd, InlineQueryCallback&& cb);

  /**
   * @brief set callback for ShippingQuery
   * @param cmd - invoice_payload that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onShippingQuery(std::string_view cmd,ShippingQueryCallback&& cb);

  /**
   * @brief set callback for PrecheckoutQuery query
   * @param cmd - invoice_payload -in query that will trigger the callback
   * @param cb - callback that must follow void(ShippingQuery&&) signature
   */
  void onPreCheckoutQuery(std::string_view cmd,PreCheckoutQueryCallback&& cb);

  /**
   * @brief Templated function that can be used to set callback with uniform syntax
   */
  template<class CallbackType>
  void onEvent(std::string_view cmd, CallbackType&& cb) {
       updater.addCallback(cmd, std::forward<CallbackType>(cb));
  }
  /**
   * @brief Templated function that can be used to set callback using std::regex
   * \warning regexes have the lowest priorty in callbacks routing
   * @param cmd - regex that will trigger the callback
   * @param cb - callback, must be one either one of MessageCallback, QueryCallback, InlineQueryCallback,
     ChosenInlineResultCallback,ShippingQueryCallback,PreCheckoutQueryCallback;
   */
  template<class CallbackType>
  void onEvent(std::regex cmd, CallbackType&& cb) {
       updater.addCallback(cmd, std::move(cb));
  }
  /**
   * @brief Function set sequence
   * \description Use this function to set sequence for id (this can be any number what
   * allows you to indentify sequence for routing)
   * @param id - any number that will be used to route callbacks of sequence
   * @param seq - sequence that must be a std::shared_ptr
   */
  template <class Event,class Check>
  void startSequence(int64_t id, std::shared_ptr<Sequence<Event,Check>> seq) {
      updater.addSequence(id, seq);
  }
  /**
   * @brief Removes sequence for 'user_id'
   * @param id - id that was specified in startSequence
   */
  void stopSequence(int64_t id);
  /**
   * Use this method to get current webhook status. Requires no parameters.
   * On success, returns a WebhookInfo object.
   * If the bot is using getUpdates, will return an object with the url field empty.
   */
  /**
   * @brief Short syntax to reply to message
   * @param msg - message to reply
   * @param message - text to send user
   */


  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &message) const;

  /// stops the bot
  void stop();

  /// TODO: add description
  void start(std::optional<int64_t> timeout = {}, std::optional<int64_t> offset = {}, std::optional<int8_t> limit = {},
             std::optional<std::vector<std::string_view>> allowed_updates = {});
  /**
   * @brief Starts webhook server on 0.0.0.0 address and given port and sets webhook for teleragram bot
   * @param url - url that will be used in setWebhook server
   * @param port - port that server will listen to
   * @param cert_path - path to public certificate (must have .pem format)
   * @param key_path - path to private key path (must have .pem format)
   */
  bool setWebhookServer(const std::string &url, uint16_t port,
                        const std::string &cert_path = "./cert.pem",
                        const std::string &key_path = "./key.pem");

  std::string getUpdatesRawJson(
          std::optional<uint32_t> offset = {}, std::optional<uint8_t> limit = {},
          std::optional<uint32_t> timeout = {},
          std::optional<std::vector<std::string_view>> allowed_updates = {});

  // --------------------- AUTOGENERATED CODE -------------------------------------



/**
	@brief Use this method to receive incoming updates using long polling (wiki). An Array of Update objects is returned.
	@param offset Identifier of the first update to be returned. Must be greater by one than the highest among the identifiers of previously received updates. By default, updates starting with the earliest unconfirmed update are returned. An update is considered confirmed as soon as getUpdates is called with an offset higher than its update_id. The negative offset can be specified to retrieve updates starting from -offset update from the end of the updates queue. All previous updates will forgotten.
	@param limit Limits the number of updates to be retrieved. Values between 1-100 are accepted. Defaults to 100.
	@param timeout Timeout in seconds for long polling. Defaults to 0, i.e. usual short polling. Should be positive, short polling should be used for testing purposes only.
	@param allowed_updates A JSON-serialized list of the update types you want your bot to receive. For example, specify ["message", "edited_channel_post", "callback_query"] to only receive updates of these types. See Update for a complete list of available update types. Specify an empty list to receive all updates regardless of type (default). If not specified, the previous setting will be used.Please note that this parameter doesn't affect updates created before the call to the getUpdates, so unwanted updates may be received for a short period of time.
	@returns std::vector<Update>
*/
std::pair<std::vector<Update>,opt_error> getUpdates ( 
															std::optional<int64_t>	offset={},
															std::optional<int64_t>	limit={},
															std::optional<int64_t>	timeout={},
															const std::optional<std::vector<std::string>>& 	allowed_updates={}
) const;
/**
	@brief Use this method to specify a url and receive incoming updates via an outgoing webhook. Whenever there is an update for the bot, we will send an HTTPS POST request to the specified url, containing a JSON-serialized Update. In case of an unsuccessful request, we will give up after a reasonable amount of attempts. Returns True on success.
	@param url HTTPS url to send updates to. Use an empty string to remove webhook integration
	@param certificate Upload your public key certificate so that the root certificate in use can be checked. See our self-signed guide for details.
	@param max_connections Maximum allowed number of simultaneous HTTPS connections to the webhook for update delivery, 1-100. Defaults to 40. Use lower values to limit the load on your bot's server, and higher values to increase your bot's throughput.
	@param allowed_updates A JSON-serialized list of the update types you want your bot to receive. For example, specify ["message", "edited_channel_post", "callback_query"] to only receive updates of these types. See Update for a complete list of available update types. Specify an empty list to receive all updates regardless of type (default). If not specified, the previous setting will be used.Please note that this parameter doesn't affect updates created before the call to the setWebhook, so unwanted updates may be received for a short period of time.
	@returns bool
*/
std::pair<bool,opt_error> setWebhook ( 
															const std::string& 	url,
															const std::optional<std::string>& 	certificate={},
															std::optional<int64_t>	max_connections={},
															const std::optional<std::vector<std::string>>& 	allowed_updates={}
) const;
/**
	@brief Use this method to remove webhook integration if you decide to switch back to getUpdates. Returns True on success. Requires no parameters.
	@returns bool
*/
std::pair<bool,opt_error> deleteWebhook ( 

) const;
/**
	@brief Use this method to get current webhook status. Requires no parameters. On success, returns a WebhookInfo object. If the bot is using getUpdates, will return an object with the url field empty.
	@returns WebhookInfo
*/
std::pair<WebhookInfo,opt_error> getWebhookInfo ( 

) const;
/**
	@brief A simple method for testing your bot's auth token. Requires no parameters. Returns basic information about the bot in form of a User object.
	@returns User
*/
std::pair<User,opt_error> getMe ( 

) const;
/**
	@brief Use this method to send text messages. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param text Text of the message to be sent, 1-4096 characters after entities parsing
	@param parse_mode Mode for parsing entities in the message text. See formatting options for more details.
	@param disable_web_page_preview Disables link previews for links in this message
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	text,
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	disable_web_page_preview={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to forward messages of any kind. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param from_chat_id Unique identifier for the chat where the original message was sent (or channel username in the format @channelusername)
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param message_id Message identifier in the chat specified in from_chat_id
	@returns Message
*/
std::pair<Message,opt_error> forwardMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::variant<int64_t,std::string>& 	from_chat_id,
															int64_t	message_id,
															std::optional<bool>	disable_notification={}
) const;
/**
	@brief Use this method to send photos. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param photo Photo to send. Pass a file_id as String to send a photo that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get a photo from the Internet, or upload a new photo using multipart/form-data. 
	@param caption Photo caption (may also be used when resending photos by file_id), 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the photo caption. See formatting options for more details.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	photo,
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send audio files, if you want Telegram clients to display them in the music player. Your audio must be in the .MP3 or .M4A format. On success, the sent Message is returned. Bots can currently send audio files of up to 50 MB in size, this limit may be changed in the future.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param audio Audio file to send. Pass a file_id as String to send an audio file that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get an audio file from the Internet, or upload a new one using multipart/form-data. 
	@param caption Audio caption, 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the audio caption. See formatting options for more details.
	@param duration Duration of the audio in seconds
	@param performer Performer
	@param title Track name
	@param thumb Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendAudio ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	audio,
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<int64_t>	duration={},
															const std::optional<std::string>& 	performer={},
															const std::optional<std::string>& 	title={},
															const std::optional<std::string>& 	thumb={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send general files. On success, the sent Message is returned. Bots can currently send files of any type of up to 50 MB in size, this limit may be changed in the future.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param document File to send. Pass a file_id as String to send a file that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. 
	@param thumb Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
	@param caption Document caption (may also be used when resending documents by file_id), 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the document caption. See formatting options for more details.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendDocument ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	document,
															const std::optional<std::string>& 	thumb={},
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send video files, Telegram clients support mp4 videos (other formats may be sent as Document). On success, the sent Message is returned. Bots can currently send video files of up to 50 MB in size, this limit may be changed in the future.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param video Video to send. Pass a file_id as String to send a video that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get a video from the Internet, or upload a new video using multipart/form-data. 
	@param duration Duration of sent video in seconds
	@param width Video width
	@param height Video height
	@param thumb Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
	@param caption Video caption (may also be used when resending videos by file_id), 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the video caption. See formatting options for more details.
	@param supports_streaming Pass True, if the uploaded video is suitable for streaming
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendVideo ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	video,
															std::optional<int64_t>	duration={},
															std::optional<int64_t>	width={},
															std::optional<int64_t>	height={},
															const std::optional<std::string>& 	thumb={},
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	supports_streaming={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send animation files (GIF or H.264/MPEG-4 AVC video without sound). On success, the sent Message is returned. Bots can currently send animation files of up to 50 MB in size, this limit may be changed in the future.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param animation Animation to send. Pass a file_id as String to send an animation that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get an animation from the Internet, or upload a new animation using multipart/form-data. 
	@param duration Duration of sent animation in seconds
	@param width Animation width
	@param height Animation height
	@param thumb Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
	@param caption Animation caption (may also be used when resending animation by file_id), 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the animation caption. See formatting options for more details.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendAnimation ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	animation,
															std::optional<int64_t>	duration={},
															std::optional<int64_t>	width={},
															std::optional<int64_t>	height={},
															const std::optional<std::string>& 	thumb={},
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send audio files, if you want Telegram clients to display the file as a playable voice message. For this to work, your audio must be in an .OGG file encoded with OPUS (other formats may be sent as Audio or Document). On success, the sent Message is returned. Bots can currently send voice messages of up to 50 MB in size, this limit may be changed in the future.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param voice Audio file to send. Pass a file_id as String to send a file that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. 
	@param caption Voice message caption, 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the voice message caption. See formatting options for more details.
	@param duration Duration of the voice message in seconds
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendVoice ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	voice,
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<int64_t>	duration={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief As of v.4.0, Telegram clients support rounded square mp4 videos of up to 1 minute long. Use this method to send video messages. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param video_note Video note to send. Pass a file_id as String to send a video note that exists on the Telegram servers (recommended) or upload a new video using multipart/form-data. . Sending video notes by a URL is currently unsupported
	@param duration Duration of sent video in seconds
	@param length Video width and height, i.e. diameter of the video message
	@param thumb Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendVideoNote ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	video_note,
															std::optional<int64_t>	duration={},
															std::optional<int64_t>	length={},
															const std::optional<std::string>& 	thumb={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send point on the map. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param latitude Latitude of the location
	@param longitude Longitude of the location
	@param live_period Period in seconds for which the location will be updated (see Live Locations, should be between 60 and 86400.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendLocation ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															float	latitude,
															float	longitude,
															std::optional<int64_t>	live_period={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to edit live location messages. A location can be edited until its live_period expires or editing is explicitly disabled by a call to stopMessageLiveLocation. On success, if the edited message was sent by the bot, the edited Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message to edit
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param latitude Latitude of new location
	@param longitude Longitude of new location
	@param reply_markup A JSON-serialized object for a new inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> editMessageLiveLocation ( 
															float	latitude,
															float	longitude,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to stop updating a live location message before live_period expires. On success, if the message was sent by the bot, the sent Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message with live location to stop
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param reply_markup A JSON-serialized object for a new inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> stopMessageLiveLocation ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to send information about a venue. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param latitude Latitude of the venue
	@param longitude Longitude of the venue
	@param title Name of the venue
	@param address Address of the venue
	@param foursquare_id Foursquare identifier of the venue
	@param foursquare_type Foursquare type of the venue, if known. (For example, "arts_entertainment/default", "arts_entertainment/aquarium" or "food/icecream".)
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendVenue ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															float	latitude,
															float	longitude,
															const std::string& 	title,
															const std::string& 	address,
															const std::optional<std::string>& 	foursquare_id={},
															const std::optional<std::string>& 	foursquare_type={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send phone contacts. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param phone_number Contact's phone number
	@param first_name Contact's first name
	@param last_name Contact's last name
	@param vcard Additional data about the contact in the form of a vCard, 0-2048 bytes
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendContact ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	phone_number,
															const std::string& 	first_name,
															const std::optional<std::string>& 	last_name={},
															const std::optional<std::string>& 	vcard={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send a native poll. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param question Poll question, 1-255 characters
	@param options A JSON-serialized list of answer options, 2-10 strings 1-100 characters each
	@param is_anonymous True, if the poll needs to be anonymous, defaults to True
	@param type Poll type, "quiz" or "regular", defaults to "regular"
	@param allows_multiple_answers True, if the poll allows multiple answers, ignored for polls in quiz mode, defaults to False
	@param correct_option_id 0-based identifier of the correct answer option, required for polls in quiz mode
	@param explanation Text that is shown when a user chooses an incorrect answer or taps on the lamp icon in a quiz-style poll, 0-200 characters with at most 2 line feeds after entities parsing
	@param explanation_parse_mode Mode for parsing entities in the explanation. See formatting options for more details.
	@param open_period Amount of time in seconds the poll will be active after creation, 5-600. Can't be used together with close_date.
	@param close_date Point in time (Unix timestamp) when the poll will be automatically closed. Must be at least 5 and no more than 600 seconds in the future. Can't be used together with open_period.
	@param is_closed Pass True, if the poll needs to be immediately closed. This can be useful for poll preview.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendPoll ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	question,
															const std::vector<std::string>& 	options,
															std::optional<bool>	is_anonymous={},
															const std::optional<std::string>& 	type={},
															std::optional<bool>	allows_multiple_answers={},
															std::optional<int64_t>	correct_option_id={},
															const std::optional<std::string>& 	explanation={},
															const std::optional<std::string>& 	explanation_parse_mode={},
															std::optional<int64_t>	open_period={},
															std::optional<int64_t>	close_date={},
															std::optional<bool>	is_closed={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to send an animated emoji that will display a random value. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param emoji Emoji on which the dice throw animation is based. Currently, must be one of "", "", or "". Dice can have values 1-6 for "" and "", and values 1-5 for "". Defaults to ""
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendDice ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::optional<std::string>& 	emoji={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method when you need to tell the user that something is happening on the bot's side. The status is set for 5 seconds or less (when a message arrives from your bot, Telegram clients clear its typing status). Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param action Type of action to broadcast. Choose one, depending on what the user is about to receive: typing for text messages, upload_photo for photos, record_video or upload_video for videos, record_audio or upload_audio for audio files, upload_document for general files, find_location for location data, record_video_note or upload_video_note for video notes.
	@returns bool
*/
std::pair<bool,opt_error> sendChatAction ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	action
) const;
/**
	@brief Use this method to get a list of profile pictures for a user. Returns a UserProfilePhotos object.
	@param user_id Unique identifier of the target user
	@param offset Sequential number of the first photo to be returned. By default, all photos are returned.
	@param limit Limits the number of photos to be retrieved. Values between 1-100 are accepted. Defaults to 100.
	@returns UserProfilePhotos
*/
std::pair<UserProfilePhotos,opt_error> getUserProfilePhotos ( 
															int64_t	user_id,
															std::optional<int64_t>	offset={},
															std::optional<int64_t>	limit={}
) const;
/**
	@brief Use this method to get basic info about a file and prepare it for downloading. For the moment, bots can download files of up to 20MB in size. On success, a File object is returned. The file can then be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>, where <file_path> is taken from the response. It is guaranteed that the link will be valid for at least 1 hour. When the link expires, a new one can be requested by calling getFile again.
	@param file_id File identifier to get info about
	@returns File
*/
std::pair<File,opt_error> getFile ( 
															const std::string& 	file_id
) const;
/**
	@brief Use this method to kick a user from a group, a supergroup or a channel. In the case of supergroups and channels, the user will not be able to return to the group on their own using invite links, etc., unless unbanned first. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns True on success.
	@param chat_id Unique identifier for the target group or username of the target supergroup or channel (in the format @channelusername)
	@param user_id Unique identifier of the target user
	@param until_date Date when the user will be unbanned, unix time. If user is banned for more than 366 days or less than 30 seconds from the current time they are considered to be banned forever
	@returns bool
*/
std::pair<bool,opt_error> kickChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															std::optional<int64_t>	until_date={}
) const;
/**
	@brief Use this method to unban a previously kicked user in a supergroup or channel. The user will not return to the group or channel automatically, but will be able to join via link, etc. The bot must be an administrator for this to work. Returns True on success.
	@param chat_id Unique identifier for the target group or username of the target supergroup or channel (in the format @username)
	@param user_id Unique identifier of the target user
	@returns bool
*/
std::pair<bool,opt_error> unbanChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id
) const;
/**
	@brief Use this method to restrict a user in a supergroup. The bot must be an administrator in the supergroup for this to work and must have the appropriate admin rights. Pass True for all permissions to lift restrictions from a user. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup (in the format @supergroupusername)
	@param user_id Unique identifier of the target user
	@param permissions A JSON-serialized object for new user permissions
	@param until_date Date when restrictions will be lifted for the user, unix time. If user is restricted for more than 366 days or less than 30 seconds from the current time, they are considered to be restricted forever
	@returns std::variant<bool>
*/
std::pair<std::variant<bool>,opt_error> restrictChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															const ChatPermissions& 	permissions,
															std::optional<int64_t>	until_date={}
) const;
/**
	@brief Use this method to promote or demote a user in a supergroup or a channel. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Pass False for all boolean parameters to demote a user. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param user_id Unique identifier of the target user
	@param can_change_info Pass True, if the administrator can change chat title, photo and other settings
	@param can_post_messages Pass True, if the administrator can create channel posts, channels only
	@param can_edit_messages Pass True, if the administrator can edit messages of other users and can pin messages, channels only
	@param can_delete_messages Pass True, if the administrator can delete messages of other users
	@param can_invite_users Pass True, if the administrator can invite new users to the chat
	@param can_restrict_members Pass True, if the administrator can restrict, ban or unban chat members
	@param can_pin_messages Pass True, if the administrator can pin messages, supergroups only
	@param can_promote_members Pass True, if the administrator can add new administrators with a subset of their own privileges or demote administrators that he has promoted, directly or indirectly (promoted by administrators that were appointed by him)
	@returns std::variant<bool>
*/
std::pair<std::variant<bool>,opt_error> promoteChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															std::optional<bool>	can_change_info={},
															std::optional<bool>	can_post_messages={},
															std::optional<bool>	can_edit_messages={},
															std::optional<bool>	can_delete_messages={},
															std::optional<bool>	can_invite_users={},
															std::optional<bool>	can_restrict_members={},
															std::optional<bool>	can_pin_messages={},
															std::optional<bool>	can_promote_members={}
) const;
/**
	@brief Use this method to set a custom title for an administrator in a supergroup promoted by the bot. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup (in the format @supergroupusername)
	@param user_id Unique identifier of the target user
	@param custom_title New custom title for the administrator; 0-16 characters, emoji are not allowed
	@returns bool
*/
std::pair<bool,opt_error> setChatAdministratorCustomTitle ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id,
															const std::string& 	custom_title
) const;
/**
	@brief Use this method to set default chat permissions for all members. The bot must be an administrator in the group or a supergroup for this to work and must have the can_restrict_members admin rights. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup (in the format @supergroupusername)
	@param permissions New default chat permissions
	@returns bool
*/
std::pair<bool,opt_error> setChatPermissions ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const ChatPermissions& 	permissions
) const;
/**
	@brief Use this method to generate a new invite link for a chat; any previously generated link is revoked. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns the new invite link as String on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@returns std::string
*/
std::pair<std::string,opt_error> exportChatInviteLink ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to set a new profile photo for the chat. Photos can't be changed for private chats. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param photo New chat photo, uploaded using multipart/form-data
	@returns bool
*/
std::pair<bool,opt_error> setChatPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	photo
) const;
/**
	@brief Use this method to delete a chat photo. Photos can't be changed for private chats. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@returns bool
*/
std::pair<bool,opt_error> deleteChatPhoto ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to change the title of a chat. Titles can't be changed for private chats. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param title New chat title, 1-255 characters
	@returns bool
*/
std::pair<bool,opt_error> setChatTitle ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	title
) const;
/**
	@brief Use this method to change the description of a group, a supergroup or a channel. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param description New chat description, 0-255 characters
	@returns bool
*/
std::pair<bool,opt_error> setChatDescription ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::optional<std::string>& 	description={}
) const;
/**
	@brief Use this method to pin a message in a group, a supergroup, or a channel. The bot must be an administrator in the chat for this to work and must have the 'can_pin_messages' admin right in the supergroup or 'can_edit_messages' admin right in the channel. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Identifier of a message to pin
	@param disable_notification Pass True, if it is not necessary to send a notification to all chat members about the new pinned message. Notifications are always disabled in channels.
	@returns bool
*/
std::pair<bool,opt_error> pinChatMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id,
															std::optional<bool>	disable_notification={}
) const;
/**
	@brief Use this method to unpin a message in a group, a supergroup, or a channel. The bot must be an administrator in the chat for this to work and must have the 'can_pin_messages' admin right in the supergroup or 'can_edit_messages' admin right in the channel. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@returns bool
*/
std::pair<bool,opt_error> unpinChatMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method for your bot to leave a group, supergroup or channel. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup or channel (in the format @channelusername)
	@returns bool
*/
std::pair<bool,opt_error> leaveChat ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to get up to date information about the chat (current name of the user for one-on-one conversations, current username of a user, group or channel, etc.). Returns a Chat object on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup or channel (in the format @channelusername)
	@returns Chat
*/
std::pair<Chat,opt_error> getChat ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to get a list of administrators in a chat. On success, returns an Array of ChatMember objects that contains information about all chat administrators except other bots. If the chat is a group or a supergroup and no administrators were appointed, only the creator will be returned.
	@param chat_id Unique identifier for the target chat or username of the target supergroup or channel (in the format @channelusername)
	@returns std::vector<ChatMember>
*/
std::pair<std::vector<ChatMember>,opt_error> getChatAdministrators ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to get the number of members in a chat. Returns Int on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup or channel (in the format @channelusername)
	@returns int64_t
*/
std::pair<int64_t,opt_error> getChatMembersCount ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to get information about a member of a chat. Returns a ChatMember object on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup or channel (in the format @channelusername)
	@param user_id Unique identifier of the target user
	@returns ChatMember
*/
std::pair<ChatMember,opt_error> getChatMember ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	user_id
) const;
/**
	@brief Use this method to set a new group sticker set for a supergroup. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Use the field can_set_sticker_set optionally returned in getChat requests to check if the bot can use this method. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup (in the format @supergroupusername)
	@param sticker_set_name Name of the sticker set to be set as the group sticker set
	@returns bool
*/
std::pair<bool,opt_error> setChatStickerSet ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	sticker_set_name
) const;
/**
	@brief Use this method to delete a group sticker set from a supergroup. The bot must be an administrator in the chat for this to work and must have the appropriate admin rights. Use the field can_set_sticker_set optionally returned in getChat requests to check if the bot can use this method. Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target supergroup (in the format @supergroupusername)
	@returns bool
*/
std::pair<bool,opt_error> deleteChatStickerSet ( 
															const std::variant<int64_t,std::string>& 	chat_id
) const;
/**
	@brief Use this method to send answers to callback queries sent from inline keyboards. The answer will be displayed to the user as a notification at the top of the chat screen or as an alert. On success, True is returned.
	@param callback_query_id Unique identifier for the query to be answered
	@param text Text of the notification. If not specified, nothing will be shown to the user, 0-200 characters
	@param show_alert If true, an alert will be shown by the client instead of a notification at the top of the chat screen. Defaults to false.
	@param url URL that will be opened by the user's client. If you have created a Game and accepted the conditions via @Botfather, specify the URL that opens your game - note that this will only work if the query comes from a callback_game button.Otherwise, you may use links like t.me/your_bot?start=XXXX that open your bot with a parameter.
	@param cache_time The maximum amount of time in seconds that the result of the callback query may be cached client-side. Telegram apps will support caching starting in version 3.14. Defaults to 0.
	@returns bool
*/
std::pair<bool,opt_error> answerCallbackQuery ( 
															const std::string& 	callback_query_id,
															const std::optional<std::string>& 	text={},
															std::optional<bool>	show_alert={},
															const std::optional<std::string>& 	url={},
															std::optional<int64_t>	cache_time={}
) const;
/**
	@brief Use this method to change the list of the bot's commands. Returns True on success.
	@param commands A JSON-serialized list of bot commands to be set as the list of the bot's commands. At most 100 commands can be specified.
	@returns bool
*/
std::pair<bool,opt_error> setMyCommands ( 
															const std::vector<BotCommand>& 	commands
) const;
/**
	@brief Use this method to get the current list of the bot's commands. Requires no parameters. Returns Array of BotCommand on success.
	@returns std::vector<BotCommand>
*/
std::pair<std::vector<BotCommand>,opt_error> getMyCommands ( 

) const;
/**
	@brief Use this method to edit text and game messages. On success, if edited message is sent by the bot, the edited Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message to edit
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param text New text of the message, 1-4096 characters after entities parsing
	@param parse_mode Mode for parsing entities in the message text. See formatting options for more details.
	@param disable_web_page_preview Disables link previews for links in this message
	@param reply_markup A JSON-serialized object for an inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> editMessageText ( 
															const std::string& 	text,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<std::string>& 	parse_mode={},
															std::optional<bool>	disable_web_page_preview={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to edit captions of messages. On success, if edited message is sent by the bot, the edited Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message to edit
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param caption New caption of the message, 0-1024 characters after entities parsing
	@param parse_mode Mode for parsing entities in the message caption. See formatting options for more details.
	@param reply_markup A JSON-serialized object for an inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> editMessageCaption ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<std::string>& 	caption={},
															const std::optional<std::string>& 	parse_mode={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to edit animation, audio, document, photo, or video messages. If a message is a part of a message album, then it can be edited only to a photo or a video. Otherwise, message type can be changed arbitrarily. When inline message is edited, new file can't be uploaded. Use previously uploaded file via its file_id or specify a URL. On success, if the edited message was sent by the bot, the edited Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message to edit
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param media A JSON-serialized object for a new media content of the message
	@param reply_markup A JSON-serialized object for a new inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> editMessageMedia ( 
															const InputMedia& 	media,
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to edit only the reply markup of messages. On success, if edited message is sent by the bot, the edited Message is returned, otherwise True is returned.
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Required if inline_message_id is not specified. Identifier of the message to edit
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@param reply_markup A JSON-serialized object for an inline keyboard.
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> editMessageReplyMarkup ( 
															const std::optional<std::variant<int64_t,std::string>>& 	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to stop a poll which was sent by the bot. On success, the stopped Poll with the final results is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Identifier of the original message with the poll
	@param reply_markup A JSON-serialized object for a new message inline keyboard.
	@returns Poll
*/
std::pair<Poll,opt_error> stopPoll ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id,
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to delete a message, including service messages, with the following limitations:- A message can only be deleted if it was sent less than 48 hours ago.- A dice message in a private chat can only be deleted if it was sent more than 24 hours ago.- Bots can delete outgoing messages in private chats, groups, and supergroups.- Bots can delete incoming messages in private chats.- Bots granted can_post_messages permissions can delete outgoing messages in channels.- If the bot is an administrator of a group, it can delete any message there.- If the bot has can_delete_messages permission in a supergroup or a channel, it can delete any message there.Returns True on success.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param message_id Identifier of the message to delete
	@returns bool
*/
std::pair<bool,opt_error> deleteMessage ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															int64_t	message_id
) const;
/**
	@brief Use this method to send static .WEBP or animated .TGS stickers. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat or username of the target channel (in the format @channelusername)
	@param sticker Sticker to send. Pass a file_id as String to send a file that exists on the Telegram servers (recommended), pass an HTTP URL as a String for Telegram to get a .WEBP file from the Internet, or upload a new one using multipart/form-data. 
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup Additional interface options. A JSON-serialized object for an inline keyboard, custom reply keyboard, instructions to remove reply keyboard or to force a reply from the user.
	@returns Message
*/
std::pair<Message,opt_error> sendSticker ( 
															const std::variant<int64_t,std::string>& 	chat_id,
															const std::string& 	sticker,
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>>& 	reply_markup={}
) const;
/**
	@brief Use this method to get a sticker set. On success, a StickerSet object is returned.
	@param name Name of the sticker set
	@returns StickerSet
*/
std::pair<StickerSet,opt_error> getStickerSet ( 
															const std::string& 	name
) const;
/**
	@brief Use this method to upload a .PNG file with a sticker for later use in createNewStickerSet and addStickerToSet methods (can be used multiple times). Returns the uploaded File on success.
	@param user_id User identifier of sticker file owner
	@param png_sticker PNG image with the sticker, must be up to 512 kilobytes in size, dimensions must not exceed 512px, and either width or height must be exactly 512px. 
	@returns File
*/
std::pair<File,opt_error> uploadStickerFile ( 
															int64_t	user_id,
															const std::string& 	png_sticker
) const;
/**
	@brief Use this method to create a new sticker set owned by a user. The bot will be able to edit the sticker set thus created. You must use exactly one of the fields png_sticker or tgs_sticker. Returns True on success.
	@param user_id User identifier of created sticker set owner
	@param name Short name of sticker set, to be used in t.me/addstickers/ URLs (e.g., animals). Can contain only english letters, digits and underscores. Must begin with a letter, can't contain consecutive underscores and must end in "_by_<bot username>". <bot_username> is case insensitive. 1-64 characters.
	@param title Sticker set title, 1-64 characters
	@param png_sticker PNG image with the sticker, must be up to 512 kilobytes in size, dimensions must not exceed 512px, and either width or height must be exactly 512px. Pass a file_id as a String to send a file that already exists on the Telegram servers, pass an HTTP URL as a String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. 
	@param tgs_sticker TGS animation with the sticker, uploaded using multipart/form-data. See https://core.telegram.org/animated_stickers#technical-requirements for technical requirements
	@param emojis One or more emoji corresponding to the sticker
	@param contains_masks Pass True, if a set of mask stickers should be created
	@param mask_position A JSON-serialized object for position where the mask should be placed on faces
	@returns bool
*/
std::pair<bool,opt_error> createNewStickerSet ( 
															int64_t	user_id,
															const std::string& 	name,
															const std::string& 	title,
															const std::string& 	emojis,
															const std::optional<std::string>& 	png_sticker={},
															const std::optional<std::string>& 	tgs_sticker={},
															std::optional<bool>	contains_masks={},
															const std::optional<MaskPosition>& 	mask_position={}
) const;
/**
	@brief Use this method to add a new sticker to a set created by the bot. You must use exactly one of the fields png_sticker or tgs_sticker. Animated stickers can be added to animated sticker sets and only to them. Animated sticker sets can have up to 50 stickers. Static sticker sets can have up to 120 stickers. Returns True on success.
	@param user_id User identifier of sticker set owner
	@param name Sticker set name
	@param png_sticker PNG image with the sticker, must be up to 512 kilobytes in size, dimensions must not exceed 512px, and either width or height must be exactly 512px. Pass a file_id as a String to send a file that already exists on the Telegram servers, pass an HTTP URL as a String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. 
	@param tgs_sticker TGS animation with the sticker, uploaded using multipart/form-data. See https://core.telegram.org/animated_stickers#technical-requirements for technical requirements
	@param emojis One or more emoji corresponding to the sticker
	@param mask_position A JSON-serialized object for position where the mask should be placed on faces
	@returns bool
*/
std::pair<bool,opt_error> addStickerToSet ( 
															int64_t	user_id,
															const std::string& 	name,
															const std::string& 	emojis,
															const std::optional<std::string>& 	png_sticker={},
															const std::optional<std::string>& 	tgs_sticker={},
															const std::optional<MaskPosition>& 	mask_position={}
) const;
/**
	@brief Use this method to move a sticker in a set created by the bot to a specific position. Returns True on success.
	@param sticker File identifier of the sticker
	@param position New sticker position in the set, zero-based
	@returns bool
*/
std::pair<bool,opt_error> setStickerPositionInSet ( 
															const std::string& 	sticker,
															int64_t	position
) const;
/**
	@brief Use this method to delete a sticker from a set created by the bot. Returns True on success.
	@param sticker File identifier of the sticker
	@returns bool
*/
std::pair<bool,opt_error> deleteStickerFromSet ( 
															const std::string& 	sticker
) const;
/**
	@brief Use this method to set the thumbnail of a sticker set. Animated thumbnails can be set for animated sticker sets only. Returns True on success.
	@param name Sticker set name
	@param user_id User identifier of the sticker set owner
	@param thumb A PNG image with the thumbnail, must be up to 128 kilobytes in size and have width and height exactly 100px, or a TGS animation with the thumbnail up to 32 kilobytes in size; see https://core.telegram.org/animated_stickers#technical-requirements for animated sticker technical requirements. Pass a file_id as a String to send a file that already exists on the Telegram servers, pass an HTTP URL as a String for Telegram to get a file from the Internet, or upload a new one using multipart/form-data. . Animated sticker set thumbnail can't be uploaded via HTTP URL.
	@returns bool
*/
std::pair<bool,opt_error> setStickerSetThumb ( 
															const std::string& 	name,
															int64_t	user_id,
															const std::optional<std::string>& 	thumb={}
) const;
/**
	@brief Use this method to send answers to an inline query. On success, True is returned.No more than 50 results per query are allowed.
	@param inline_query_id Unique identifier for the answered query
	@param results A JSON-serialized array of results for the inline query
	@param cache_time The maximum amount of time in seconds that the result of the inline query may be cached on the server. Defaults to 300.
	@param is_personal Pass True, if results may be cached on the server side only for the user that sent the query. By default, results may be returned to any user who sends the same query
	@param next_offset Pass the offset that a client should send in the next query with the same text to receive more results. Pass an empty string if there are no more results or if you don't support pagination. Offset length can't exceed 64 bytes.
	@param switch_pm_text If passed, clients will display a button with specified text that switches the user to a private chat with the bot and sends the bot a start message with the parameter switch_pm_parameter
	@param switch_pm_parameter Deep-linking parameter for the /start message sent to the bot when user presses the switch button. 1-64 characters, only A-Z, a-z, 0-9, _ and - are allowed.Example: An inline bot that sends YouTube videos can ask the user to connect the bot to their YouTube account to adapt search results accordingly. To do this, it displays a 'Connect your YouTube account' button above the results, or even before showing any. The user presses the button, switches to a private chat with the bot and, in doing so, passes a start parameter that instructs the bot to return an oauth link. Once done, the bot can offer a switch_inline button so that the user can easily return to the chat where they wanted to use the bot's inline capabilities.
	@returns bool
*/
std::pair<bool,opt_error> answerInlineQuery ( 
															const std::string& 	inline_query_id,
															const std::vector<InlineQueryResult>& 	results,
															std::optional<int64_t>	cache_time={},
															std::optional<bool>	is_personal={},
															const std::optional<std::string>& 	next_offset={},
															const std::optional<std::string>& 	switch_pm_text={},
															const std::optional<std::string>& 	switch_pm_parameter={}
) const;
/**
	@brief Use this method to send invoices. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target private chat
	@param title Product name, 1-32 characters
	@param description Product description, 1-255 characters
	@param payload Bot-defined invoice payload, 1-128 bytes. This will not be displayed to the user, use for your internal processes.
	@param provider_token Payments provider token, obtained via Botfather
	@param start_parameter Unique deep-linking parameter that can be used to generate this invoice when used as a start parameter
	@param currency Three-letter ISO 4217 currency code, see more on currencies
	@param prices Price breakdown, a JSON-serialized list of components (e.g. product price, tax, discount, delivery cost, delivery tax, bonus, etc.)
	@param provider_data A JSON-serialized data about the invoice, which will be shared with the payment provider. A detailed description of required fields should be provided by the payment provider.
	@param photo_url URL of the product photo for the invoice. Can be a photo of the goods or a marketing image for a service. People like it better when they see what they are paying for.
	@param photo_size Photo size
	@param photo_width Photo width
	@param photo_height Photo height
	@param need_name Pass True, if you require the user's full name to complete the order
	@param need_phone_number Pass True, if you require the user's phone number to complete the order
	@param need_email Pass True, if you require the user's email address to complete the order
	@param need_shipping_address Pass True, if you require the user's shipping address to complete the order
	@param send_phone_number_to_provider Pass True, if user's phone number should be sent to provider
	@param send_email_to_provider Pass True, if user's email address should be sent to provider
	@param is_flexible Pass True, if the final price depends on the shipping method
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup A JSON-serialized object for an inline keyboard. If empty, one 'Pay total price' button will be shown. If not empty, the first button must be a Pay button.
	@returns Message
*/
std::pair<Message,opt_error> sendInvoice ( 
															int64_t	chat_id,
															const std::string& 	title,
															const std::string& 	description,
															const std::string& 	payload,
															const std::string& 	provider_token,
															const std::string& 	start_parameter,
															const std::string& 	currency,
															const std::vector<LabeledPrice>& 	prices,
															const std::optional<std::string>& 	provider_data={},
															const std::optional<std::string>& 	photo_url={},
															std::optional<int64_t>	photo_size={},
															std::optional<int64_t>	photo_width={},
															std::optional<int64_t>	photo_height={},
															std::optional<bool>	need_name={},
															std::optional<bool>	need_phone_number={},
															std::optional<bool>	need_email={},
															std::optional<bool>	need_shipping_address={},
															std::optional<bool>	send_phone_number_to_provider={},
															std::optional<bool>	send_email_to_provider={},
															std::optional<bool>	is_flexible={},
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief If you sent an invoice requesting a shipping address and the parameter is_flexible was specified, the Bot API will send an Update with a shipping_query field to the bot. Use this method to reply to shipping queries. On success, True is returned.
	@param shipping_query_id Unique identifier for the query to be answered
	@param ok Specify True if delivery to the specified address is possible and False if there are any problems (for example, if delivery to the specified address is not possible)
	@param shipping_options Required if ok is True. A JSON-serialized array of available shipping options.
	@param error_message Required if ok is False. Error message in human readable form that explains why it is impossible to complete the order (e.g. "Sorry, delivery to your desired address is unavailable'). Telegram will display this message to the user.
	@returns bool
*/
std::pair<bool,opt_error> answerShippingQuery ( 
															const std::string& 	shipping_query_id,
															bool	ok,
															const std::optional<std::vector<ShippingOption>>& 	shipping_options={},
															const std::optional<std::string>& 	error_message={}
) const;
/**
	@brief Once the user has confirmed their payment and shipping details, the Bot API sends the final confirmation in the form of an Update with the field pre_checkout_query. Use this method to respond to such pre-checkout queries. On success, True is returned. Note: The Bot API must receive an answer within 10 seconds after the pre-checkout query was sent.
	@param pre_checkout_query_id Unique identifier for the query to be answered
	@param ok Specify True if everything is alright (goods are available, etc.) and the bot is ready to proceed with the order. Use False if there are any problems.
	@param error_message Required if ok is False. Error message in human readable form that explains the reason for failure to proceed with the checkout (e.g. "Sorry, somebody just bought the last of our amazing black T-shirts while you were busy filling out your payment details. Please choose a different color or garment!"). Telegram will display this message to the user.
	@returns bool
*/
std::pair<bool,opt_error> answerPreCheckoutQuery ( 
															const std::string& 	pre_checkout_query_id,
															bool	ok,
															const std::optional<std::string>& 	error_message={}
) const;
/**
	@brief Informs a user that some of the Telegram Passport elements they provided contains errors. The user will not be able to re-submit their Passport to you until the errors are fixed (the contents of the field for which you returned the error must change). Returns True on success.
	@param user_id User identifier
	@param errors A JSON-serialized array describing the errors
	@returns bool
*/
std::pair<bool,opt_error> setPassportDataErrors ( 
															int64_t	user_id,
															const std::vector<PassportElementError>& 	errors
) const;
/**
	@brief Use this method to send a game. On success, the sent Message is returned.
	@param chat_id Unique identifier for the target chat
	@param game_short_name Short name of the game, serves as the unique identifier for the game. Set up your games via Botfather.
	@param disable_notification Sends the message silently. Users will receive a notification with no sound.
	@param reply_to_message_id If the message is a reply, ID of the original message
	@param reply_markup A JSON-serialized object for an inline keyboard. If empty, one 'Play game_title' button will be shown. If not empty, the first button must launch the game.
	@returns Message
*/
std::pair<Message,opt_error> sendGame ( 
															int64_t	chat_id,
															const std::string& 	game_short_name,
															std::optional<bool>	disable_notification={},
															std::optional<int64_t>	reply_to_message_id={},
															const std::optional<InlineKeyboardMarkup>& 	reply_markup={}
) const;
/**
	@brief Use this method to set the score of the specified user in a game. On success, if the message was sent by the bot, returns the edited Message, otherwise returns True. Returns an error, if the new score is not greater than the user's current score in the chat and force is False.
	@param user_id User identifier
	@param score New score, must be non-negative
	@param force Pass True, if the high score is allowed to decrease. This can be useful when fixing mistakes or banning cheaters
	@param disable_edit_message Pass True, if the game message should not be automatically edited to include the current scoreboard
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat
	@param message_id Required if inline_message_id is not specified. Identifier of the sent message
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@returns std::variant<bool, Message>
*/
std::pair<std::variant<bool, Message>,opt_error> setGameScore ( 
															int64_t	user_id,
															int64_t	score,
															std::optional<bool>	force={},
															std::optional<bool>	disable_edit_message={},
															std::optional<int64_t>	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={}
) const;
/**
	@brief Use this method to get data for high score tables. Will return the score of the specified user and several of their neighbors in a game. On success, returns an Array of GameHighScore objects.
	@param user_id Target user id
	@param chat_id Required if inline_message_id is not specified. Unique identifier for the target chat
	@param message_id Required if inline_message_id is not specified. Identifier of the sent message
	@param inline_message_id Required if chat_id and message_id are not specified. Identifier of the inline message
	@returns std::vector<GameHighScore>
*/
std::pair<std::vector<GameHighScore>,opt_error> getGameHighScores ( 
															int64_t	user_id,
															std::optional<int64_t>	chat_id={},
															std::optional<int64_t>	message_id={},
															const std::optional<std::string>& 	inline_message_id={}
) const;
};
}
