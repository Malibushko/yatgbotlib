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

using opt_string = std::optional<std::string>;
using opt_bool = std::optional<bool>;
using opt_int64 = std::optional<int64_t>;
using opt_uint64 = std::optional<uint64_t>;
using opt_uint32 = std::optional<uint32_t>;
using opt_int32 = std::optional<int32_t>;
using opt_uint8 = std::optional<uint8_t>;
using opt_string_ref = std::optional<std::string> &;
using opt_string_view = std::optional<std::string_view>;
using opt_error = std::optional<utility::Error>;
using reply_markups = std::variant<InlineKeyboardMarkup, ReplyKeyboardMarkup,
                                   ReplyKeyboardRemove, ForceReply>;
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
  Bot(const std::string &token) noexcept;
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
  void onInlineResult(std::string_view cmd, ChosenInlineResultCallback &&cb);
  /**
   * @brief set callback for CallbackQuery
   * @param cmd - data that will trigger the callback
   * @param cb - callback function that must follow void(CallbackQuery&&) signature
   */
  void onCallback(std::string_view cmd, QueryCallback &&cb);
  /**
   * @brief set callback for Message (preferrably commands like /help)
   * @param cmd - text -in message that will trigger the callback
   * @param cb - callback that must follow void(Message&&) signature
   */
  void onCommand(std::string_view cmd, MessageCallback &&cb);

  /**
   * @brief Templated version that can be used instead of 'on<command>' function set
   * @param cmd - command or data that will trigger the callback
   * @param cb - callback, must be one either one of MessageCallback, QueryCallback, InlineQueryCallback,
     ChosenInlineResultCallback,ShippingQueryCallback,PreCheckoutQueryCallback;
   */

  template<class CallbackType>
  void onEvent(std::string_view cmd, CallbackType&& cb) {
       updater.addCallback(cmd, std::move(cb));
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
      auto s = std::make_shared<Sequences>();
      *s = *seq;
      updater.addSequence(id, s);
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
  std::pair<WebhookInfo, opt_error> getWebhookInfo() const;
  void start(opt_uint64 timeout = {}, opt_uint64 offset = {},
             opt_uint8 limit = {},
             std::optional<std::vector<std::string_view>> allowed_updates = {});
  /// stops the bot
  void stop();
  /// A simple method for testing your bot's auth token.
  /// Requires no parameters. Returns basic information about the bot in form of
  /// a User object.
  std::pair<User, opt_error> getMe() const;

  using IntOrString = std::variant<int64_t, std::string>;
  using ReplyMarkups = std::variant<InlineKeyboardMarkup, ReplyKeyboardMarkup,
                                    ReplyKeyboardRemove, ForceReply>;

  /// Use this method to send text messages. On success, the sent Message is
  /// returned.
  std::pair<Message, opt_error> sendMessage(
      IntOrString chat_id, const std::string &text, std::optional<ParseMode> parse_mode = {},
      opt_bool disable_web_page_preview = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
   * @brief Short syntax to reply to message
   * @param msg - message to reply
   * @param message - text to send user
   */
  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &message) const;
  /**
   * @brief Short syntax to reply to message with text and markup
   * @param msg - message to reply
   * @param message - text to send user
   * @param markup - markup to send to user
   */
  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &text,
                                      const ReplyMarkups &markup) const;
  /**
    Use this method to receive incoming updates using long polling
    An Array of Update objects is returned.
    */
  std::pair<std::vector<Update>, opt_error> getUpdates(
      std::optional<uint32_t> offset = {}, opt_uint8 limit = {},
      opt_uint32 timeout = {},
      std::optional<std::vector<std::string_view>> allowed_updates = {}) const;
  /**
   * This method sends the request to telegram servers, but do not parse it
   */
  std::string getUpdatesRawJson(
      opt_uint32 offset = {}, opt_uint8 limit = {}, opt_uint32 timeout = {},
      std::optional<std::vector<std::string_view>> allowed_updates = {});

  /// Use this method to forward messages of any kind. On success, the sent
  /// Message is returned.
  std::pair<Message, opt_error>
  forwardMessage(IntOrString chat_id, IntOrString from_chat_id,
                 int64_t message_id, opt_bool disable_notification = {}) const;
  /// Use this method to send photos. On success, the sent Message is returned.
  std::pair<Message, opt_error>
  sendPhoto(IntOrString chat_id, const std::string &photo,
            std::string_view caption = {}, std::optional<ParseMode> parse_mode = {},
            opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
 Use this method to send audio files, if you want Telegram clients to display them in the music player.
 Your audio must be in the .MP3 or .M4A format. On success, the sent Message is returned.
 Bots can currently send audio files of up to 50 MB in size, this limit may be changed in the future.
 For sending voice messages, use the sendVoice method instead.
  */
  std::pair<Message, opt_error> sendAudio(
      IntOrString chat_id, std::string audio, opt_string_view caption = {},
      std::optional<ParseMode> parse_mode = {}, std::optional<int> duration = {},
      opt_string_view performer = {}, opt_string_view title = {},
      const std::optional<std::string> &thumb = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
   * Use this method to specify a url and receive incoming updates via an outgoing webhook.
   * Whenever there is an update for the bot, we will send an HTTPS POST request to the specified url,
   * containing a JSON-serialized Update. In case of an unsuccessful request, we will give up after
   * a reasonable amount of attempts. Returns True on success.
   */
  std::pair<bool, opt_error> setWebhook(
      std::string_view url, const std::optional<std::string> &certificate = {},
      const std::optional<uint32_t> max_connections = {},
      const std::optional<std::vector<std::string_view>> &allowed_updates = {})
      const;
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
  /**
    * only work for gif, pdf and zip files.
    */
  std::pair<Message, opt_error>
  sendDocument(IntOrString chat_id, const std::string &document,
               const std::optional<std::string> thumb = {},
               opt_string_view caption = {}, std::optional<ParseMode> parse_mode = {},
               opt_bool disable_notification = {},
               opt_int64 reply_to_message_id = {},
               std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    Use this method to send video files, Telegram clients support mp4 videos
    (other formats may be sent as Document).
    On success, the sent Message is returned.
    Bots can currently send video files of up to 50 MB in size, this limit may be changed in the future
    */
  std::pair<Message, opt_error>
  sendVideo(IntOrString chat_id, const std::string &video,
            opt_int32 duration = {}, opt_int32 width = {},
            opt_int32 height = {}, const std::optional<std::string> &thumb = {},
            opt_string_view caption = {}, std::optional<ParseMode> parse_mode = {},
            opt_bool supports_streaming = {},
            opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    * Use this method to send animation files (GIF or H.264/MPEG-4 AVC video without sound).
    * On success, the sent Message is returned.
    * Bots can currently send animation files of up to 50 MB in size, this limit may be changed
    * in the future.
    */
  std::pair<Message, opt_error> sendAnimation(
      IntOrString chat_id, const std::string &animation,
      opt_int32 duration = {}, opt_int32 width = {}, opt_int32 height = {},
      const std::optional<std::string> &thumb = {},
      opt_string_view caption = {}, std::optional<ParseMode> parse_mode = {},
      opt_bool supports_streaming = {}, opt_bool disable_nofitication = {},
      opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    * Use this method to send audio files, if you want Telegram clients to display the
    * file as a playable voice message. For this to work, your audio must be in an .OGG
    * file encoded with OPUS (other formats may be sent as Audio or Document).
    * On success, the sent Message is returned.
    * Bots can currently send voice messages of up to 50 MB in size, this limit may be changed in the future.
    */
  std::pair<Message, opt_error>
  sendVoice(IntOrString chat_id, const std::string &voice,
            opt_string_view caption = {}, std::optional<ParseMode> parse_mode = {},
            opt_int32 duration = {}, opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    As of v.4.0, Telegram clients support rounded square mp4 videos of up to 1 minute long.
    Use this method to send video messages. On success, the sent Message is returned.
    */
  std::pair<Message, opt_error>
  sendVideoNote(IntOrString chat_id, const std::string &video_note,
                opt_int32 duration = {}, opt_int32 length = {},
                const std::optional<std::string> &thumb = {},
                opt_bool disable_notification = {},
                opt_int64 reply_to_message_id = {},
                std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    * Use this method to send point on the map.
    * On success, the sent Message is returned.
    */
  std::pair<Message, opt_error>
  sendLocation(IntOrString chat_id, float latitude, float longitude,
               opt_int32 live_period = {}, opt_bool disable_notification = {},
               opt_int64 reply_to_message_id = {},
               std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
   * Use this method to edit live location messages.
   * A location can be edited until its live_period expires or editing is explicitly disabled by a call to stopMessageLiveLocation. On success,
   * if the edited message was sent by the bot, the edited Message is returned, otherwise True is returned.
   */
  std::pair<Message, opt_error> editMessageLiveLocation(
      IntOrString chat_id, int64_t message_id, float latitude, float longitude,
      opt_string_view inline_message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
 /**
   * Use this method to edit live location messages.
   * A location can be edited until its live_period expires or editing is explicitly
   * disabled by a call to stopMessageLiveLocation. On success,
   * if the edited message was sent by the bot, the edited Message is returned, otherwise True is returned.
   */
  std::pair<Message, opt_error> editMessageLiveLocation(
      float latitude, float longitude, std::string_view inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
  /**
    * Use this method to stop updating a live location message before live_period expires.
    * On success, if the message was sent by the bot, the sent Message is returned,
    * otherwise True is returned.
    */
  std::pair<Message, opt_error> stopMessageLiveLocation(
      IntOrString chat_id, int64_t message_id,
      opt_string_view inline_message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
  /**
    * Use this method to stop updating a live location message before live_period expires.
    * On success, if the message was sent by the bot, the sent Message is returned,
    * otherwise True is returned.
    */
  std::pair<Message, opt_error> stopMessageLiveLocation(
      std::string_view inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  /// Use this method to send information about a venue. On success, the sent Message is returned.
  std::pair<Message, opt_error> sendVenue(
      IntOrString chat_id, float latitude, float longitude,
      std::string_view title, std::string_view address,
      opt_string_view foursquare_id = {}, opt_string_view foursquare_type = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;
  /// Use this method to send phone contacts. On success, the sent Message is returned.
  std::pair<Message, opt_error>
  sendContact(IntOrString chat_id, std::string_view phone_number,
              std::string_view first_name, opt_string_view last_name = {},
              opt_string_view vcard = {}, opt_bool disable_notification = {},
              opt_int64 reply_to_message_id = {},
              std::optional<ReplyMarkups> reply_markup = {}) const;
  /// Use this method to send a native poll. On success, the sent Message is returned.
  std::pair<Message, opt_error>
  sendPoll(IntOrString chat_id, std::string_view question,
           const std::vector<std::string_view> &options,
           opt_bool is_anonymous = {}, opt_bool allows_multiple_answers = {},
           opt_string_view type = {}, opt_int32 correct_option_id = {},
           opt_bool is_closed = {}, opt_bool disable_notification = {},
           opt_int64 reply_to_message_id = {},
           std::optional<ReplyMarkups> reply_markup = {}) const;
  /**
    Use this method when you need to tell the user that something is happening on the bot's side.
    The status is set for 5 seconds or less (when a message arrives from your bot,
    Telegram clients clear its typing status).
    Returns True on success.
   */
  std::pair<bool, opt_error> sendChatAction(IntOrString chat_id,
                                            ChatAction action) const;
  /// Use this method to get a list of profile pictures for a user. Returns a UserProfilePhotos object.
  std::pair<UserProfilePhotos, opt_error>
  getUserProfilePhotos(int64_t user_id, opt_int32 offset = {},
                       opt_int32 limit = {}) const;

  /**
    Use this method to get basic info about a file and prepare it for downloading.
    For the moment, bots can download files of up to 20MB in size.
    On success, a File object is returned.
    The file can then be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>,
    where <file_path> is taken from the response.
    It is guaranteed that the link will be valid for at least 1 hour.
    When the link expires, a new one can be requested by calling getFile again.
   */
  std::pair<File, opt_error> getFile(std::string_view &file_id) const;

  /**
    Use this method to kick a user from a group, a supergroup or a channel.
    In the case of supergroups and channels, the user will not be able to return to the group
    on their own using invite links, etc., unless unbanned first.
    The bot must be an administrator in the chat for this to work and
    must have the appropriate admin rights. Returns True on success.
    */
  std::pair<bool, opt_error> kickChatMember(IntOrString chat_id,
                                            int64_t user_id,
                                            opt_int32 until_date = {}) const;
  /**
    Use this method to unban a previously kicked user in a supergroup or channel.
    The user will not return to the group or channel automatically, but will be able to join via link, etc.
    The bot must be an administrator for this to work. Returns True on success.
   */
  std::pair<bool, opt_error> unbanChatMember(IntOrString chat_id,
                                             int64_t user_id) const;
  /**
    Use this method to restrict a user in a supergroup.
    The bot must be an administrator in the supergroup for this to work and must
    have the appropriate admin rights. Pass True for all permissions to lift restrictions from a user.
    Returns True on success.
    */
  std::pair<bool, opt_error>
  restrictChatMember(IntOrString chat_id, int64_t user_id,
                     const ChatPermissions &perms,
                     opt_int64 until_date = {}) const;
  /**
    Use this method to promote or demote a user in a supergroup or a channel.
    The bot must be an administrator in the chat for this to work and must have the
    appropriate admin rights. Pass False for all boolean parameters to demote a user.
    Returns True on success.
    */
  std::pair<bool, opt_error> promoteChatMember(
      IntOrString chat_id, int64_t user_id, opt_bool can_change_info = {},
      opt_bool can_post_messages = {}, opt_bool can_edit_messages = {},
      opt_bool can_delete_messages = {}, opt_bool can_invite_users = {},
      opt_bool can_restrict_members = {}, opt_bool can_pin_messages = {},
      opt_bool can_promote_members = {});

  /// Use this method to set a custom title for an administrator in a supergroup promoted by the bot. Returns True on success.
  std::pair<bool, opt_error>
  setChatAdministratorCustomTitle(IntOrString chat_id, int64_t user_id,
                                  std::string_view custom_title) const;
  /**
    se this method to set default chat permissions for all members.
    The bot must be an administrator in the group or a supergroup for this to work
    and must have the can_restrict_members admin rights. Returns True on success.
   */
  std::pair<bool, opt_error>
  setChatPermissions(IntOrString chat_id,
                     const ChatPermissions &permissions) const;

  std::pair<std::string, opt_error>
  exportChatInviteLink(IntOrString chat_id) const;

  std::pair<bool, opt_error> setChatPhoto(IntOrString chat_id,
                                          const std::string &photo) const;

  std::pair<bool, opt_error> deleteChatPhoto(IntOrString chat_id) const;

  std::pair<bool, opt_error> setChatTitle(IntOrString chat_id,
                                          std::string_view title) const;

  std::pair<bool, opt_error>
  setChatDescription(IntOrString chat_id, opt_string_view description) const;

  std::pair<bool, opt_error>
  pinChatMessage(IntOrString chat_id, int64_t message_id,
                 opt_bool disable_notification = {}) const;

  std::pair<bool, opt_error> unpinChatMessage(IntOrString chat_id) const;

  std::pair<bool, opt_error> leaveChat(IntOrString chat_id) const;

  std::pair<Chat, opt_error> getChat(IntOrString chat_id) const;

  std::pair<std::vector<ChatMember>, opt_error>
  getChatAdministrators(IntOrString chat_id) const;

  std::pair<uint32_t, opt_error> getChatMembersCount(IntOrString chat_id) const;

  std::pair<ChatMember, opt_error> getChatMember(IntOrString chat_id,
                                                 uint32_t user_id) const;

  std::pair<bool, opt_error>
  setChatStickerSet(IntOrString chat_id,
                    std::string_view sticker_set_name) const;

  std::pair<bool, opt_error> deleteChatStickerSet(IntOrString chat_id) const;

  std::pair<bool, opt_error>
  answerCallbackQuery(std::string_view callback_query_id,
                      opt_string_view text = {}, opt_bool show_alert = {},
                      opt_string_view url = {},
                      opt_int32 cache_time = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  editMessageText(IntOrString chat_id, std::string_view text,
                  opt_int64 message_id, opt_string_view inline_message_id = {},
                  std::optional<ParseMode> parse_mode = {},
                  opt_bool disable_web_page_preview = {},
                  std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  editMessageText(std::string_view text, const std::string &inline_message_id,
                  std::optional<IntOrString> chat_id = {},
                  opt_int64 message_id = {}, std::optional<ParseMode> parse_mode = {},
                  opt_bool disable_web_page_preview = {},
                  std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
  std::pair<std::variant<bool, Message>, opt_error>

  editMessageCaption(IntOrString chat_id, opt_int64 message_id,
                     opt_string_view inline_message_id = {},
                     opt_string_view caption = {},
                     std::optional<ParseMode> parse_mode = {},
                     std::optional<InlineKeyboardMarkup> reply_markup = {});
  std::pair<std::variant<bool, Message>, opt_error>

  editMessageCaption(
      const std::string &inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      opt_string_view caption = {}, std::optional<ParseMode> parse_mode = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  using InputMedia =
      std::variant<InputMediaAnimation, InputMediaDocument, InputMediaAudio,
                   InputMediaPhoto, InputMediaVideo>;

  std::pair<std::variant<bool, Message>, opt_error>
  editMessageMedia(InputMedia media, opt_int64 inline_message_id,
                   std::optional<IntOrString> chat_id = {},
                   opt_int64 message_id = {},
                   std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  editMessageMedia(InputMedia media, IntOrString chat_id, int64_t message_id,
                   opt_string_view inline_message_id = {},
                   std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error> editMessageReplyMarkup(
      std::string_view inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error> editMessageReplyMarkup(
      IntOrString chat_id, int64_t message_id,
      std::string_view inline_message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<bool, opt_error> deleteMessage(IntOrString chat_id,
                                           int64_t message_id) const;

  std::pair<Poll, opt_error>
  stopPoll(IntOrString chat_id, int64_t message_id,
           std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendSticker(IntOrString chat_id, const std::string &sticker,
              opt_bool disable_notification, opt_int64 reply_to_message_id,
              std::optional<ReplyMarkups> reply_markup) const;

  std::pair<StickerSet, opt_error> getStickerSet(const std::string &name) const;

  std::pair<File, opt_error>
  uploadStickerFile(int64_t user_id, const std::string &png_sticker) const;

  std::pair<bool, opt_error> createNewStickerSet(
      int64_t user_id, std::string_view name, std::string_view title,
      const std::string &png_sticker, std::string_view emojis,
      opt_bool contains_masks = {},
      const std::optional<MaskPosition> &mask_position = {}) const;

  std::pair<bool, opt_error>
  addStickerToSet(int64_t user_id, std::string_view name,
                  const std::string &png_sticker, std::string_view emojis,
                  const std::optional<MaskPosition> &mask_position = {}) const;

  std::pair<bool, opt_error> setStickerPositionInSet(std::string_view sticker,
                                                     int32_t position) const;

  std::pair<bool, opt_error>
  deleteStickerFromSet(const std::string &sticker) const;

  std::pair<Message, opt_error> sendInvoice(
      int64_t chat_id, std::string_view title, std::string_view description,
      std::string_view payload, std::string_view provider_token,
      std::string_view start_parameter, std::string_view currency,
      const std::vector<LabeledPrice> &prices,
      opt_string_view provider_data = {}, opt_string_view photo_url = {},
      opt_int32 photo_size = {}, opt_int32 photo_width = {},
      opt_int32 photo_height = {}, opt_bool need_name = {},
      opt_bool need_phone_number = {}, opt_bool need_email = {},
      opt_bool need_shipping_address = {},
      opt_bool send_phone_number_to_provider = {},
      opt_bool send_email_to_provider = {}, opt_bool is_flexible = {},
      opt_bool disable_notification = {}, opt_int32 reply_to_message_id = {},
      const std::optional<InlineKeyboardMarkup> &reply_markup = {}) const;

  std::pair<bool, opt_error>
  answerShippingQuery(std::string_view shipping_query_id, std::false_type,
                      std::string_view error_message) const;

  std::pair<bool, opt_error> answerShippingQuery(
      std::string_view shipping_query_id, std::true_type,
      const std::vector<ShippingOption> &shipping_options) const;

  std::pair<bool, opt_error>
  answerPreCheckoutQuery(std::string_view pre_checkout_query_id, bool ok = true,
                         opt_string_view error_message = {}) const;

  std::pair<bool, opt_error>
  answerPreCheckoutQuery(std::string_view pre_checkout_query_id,
                         std::false_type, std::string_view error_message) const;

  using PassportElementError =
      std::variant<PassportElementErrorDataField, PassportElementErrorFrontSide,
                   PassportElementErrorReverseSide, PassportElementErrorSelfie,
                   PassportElementErrorFile, PassportElementErrorFiles,
                   PassportElementErrorTranslationFile,
                   PassportElementErrorTranslationFiles,
                   PassportElementErrorUnspecified>;

  std::pair<bool, opt_error>
  setPassportDataErrors(int64_t user_id,
                        const std::vector<PassportElementError> &errors) const;

  std::pair<Message, opt_error>
  sendGame(int64_t chat_id, std::string_view game_short_name,
           opt_bool disable_notification = {}, opt_int32 duration = {},
           std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  setGameScore(int64_t user_id, int32_t score,
               std::string_view inline_message_id, opt_int64 chat_id = {},
               opt_int64 message_id = {}, opt_bool force = {},
               opt_bool disable_edit_message = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  setGameScore(int64_t user_id, uint32_t score, int64_t chat_id,
               int64_t message_id, opt_string_view inline_message_id = {},
               opt_bool force = {}, opt_bool disable_edit_message = {}) const;

  std::pair<bool, opt_error>
  getGameHighScores(int64_t user_id, int64_t chat_id, int64_t message_id,
                    opt_string_view inline_message_id = {}) const;

  std::pair<bool, opt_error>
  getGameHighScores(int64_t user_id, std::string_view inline_message_id,
                    opt_int64 chat_id = {}, opt_int64 message_id = {}) const;

  std::pair<Message,opt_error>
  sendDice(IntOrString chat_id,opt_bool disable_notification = {},
           opt_int64 reply_to_message_id = {},
           std::optional<ReplyMarkups> reply_markup = {});

  std::pair<bool, opt_error> deleteWebhook() const;
};

} // namespace telegram
