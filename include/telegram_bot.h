#pragma once
#include <cassert>
#include <optional>
#include <queue>
#include <string>
#include <type_traits>
#include <variant>

#include "telegram_structs.h"
#include "headers/apimanager.h"
#include "headers/update_manager.h"

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
using opt_error = std::optional<error>;
using reply_markups = std::variant<InlineKeyboardMarkup, ReplyKeyboardMarkup,
                                   ReplyKeyboardRemove, ForceReply>;

class Bot {
  std::unique_ptr<ApiManager> api;
  UpdateManager updater;
  bool stopPolling = false;
  bool webhookSet = false;

public:
  Bot(const std::string &token) noexcept;
  void onUpdate(UpdateCallback &&cb);
  void onInlineResult(std::string_view cmd, ChosenInlineResultCallback &&cb);
  void onCallback(std::string_view cmd, QueryCallback &&cb);
  void onCommand(std::string_view cmd, MessageCallback &&cb);
  void startSequence(int64_t user_id,
                     std::shared_ptr<Sequence<MessageCallback>> seq);
  void stopSequence(int64_t user_id);
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
      IntOrString chat_id, const std::string &text, opt_string parse_mode = {},
      opt_bool disable_web_page_preview = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &message) const;
  std::pair<Message, opt_error> reply(const Message &msg,
                                      const std::string &text,
                                      const ReplyMarkups &markup) const;

  std::pair<std::vector<Update>, opt_error> getUpdates(
      std::optional<uint32_t> offset = {}, opt_uint8 limit = {},
      opt_uint32 timeout = {},
      std::optional<std::vector<std::string_view>> allowed_updates = {}) const;

  std::string getUpdatesRawJson(
      opt_uint32 offset = {}, opt_uint8 limit = {}, opt_uint32 timeout = {},
      std::optional<std::vector<std::string_view>> allowed_updates = {});

  /// Use this method to forward messages of any kind. On success, the sent
  /// Message is returned.
  std::pair<Message, opt_error>
  forwardMessage(IntOrString chat_id, IntOrString from_chat_id,
                 int64_t message_id, opt_bool disable_notification = {}) const;

  std::pair<Message, opt_error>
  sendPhoto(IntOrString chat_id, const std::string &photo,
            std::string_view caption = {}, std::string_view parse_mode = {},
            opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error> sendAudio(
      IntOrString chat_id, std::string audio, opt_string_view caption = {},
      opt_string_view parse_mode = {}, std::optional<int> duration = {},
      opt_string_view performer = {}, opt_string_view title = {},
      const std::optional<std::string> &thumb = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<bool, opt_error> setWebhook(
      std::string_view url, const std::optional<std::string> &certificate = {},
      const std::optional<uint32_t> max_connections = {},
      const std::optional<std::vector<std::string_view>> &allowed_updates = {})
      const;

  bool setWebhookServer(const std::string &url, uint16_t port,
                        const std::string &cert_path = "./cert.pem",
                        const std::string &key_path = "./key.pem");

  std::pair<Message, opt_error>
  sendDocument(IntOrString chat_id, const std::string &document,
               const std::optional<std::string> thumb = {},
               opt_string_view caption = {}, opt_string_view parse_mode = {},
               opt_bool disable_notification = {},
               opt_int64 reply_to_message_id = {},
               std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendVideo(IntOrString chat_id, const std::string &video,
            opt_int32 duration = {}, opt_int32 width = {},
            opt_int32 height = {}, const std::optional<std::string> &thumb = {},
            opt_string_view caption = {}, opt_string_view parse_mode = {},
            opt_bool supports_streaming = {},
            opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error> sendAnimation(
      IntOrString chat_id, const std::string &animation,
      opt_int32 duration = {}, opt_int32 width = {}, opt_int32 height = {},
      const std::optional<std::string> &thumb = {},
      opt_string_view caption = {}, opt_string_view parse_mode = {},
      opt_bool supports_streaming = {}, opt_bool disable_nofitication = {},
      opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendVoice(IntOrString chat_id, const std::string &voice,
            opt_string_view caption = {}, opt_string_view parse_mode = {},
            opt_int32 duration = {}, opt_bool disable_notification = {},
            opt_int64 reply_to_message_id = {},
            std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendVideoNote(IntOrString chat_id, const std::string &video_note,
                opt_int32 duration = {}, opt_int32 length = {},
                const std::optional<std::string> &thumb = {},
                opt_bool disable_notification = {},
                opt_int64 reply_to_message_id = {},
                std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendLocation(IntOrString chat_id, float latitude, float longitude,
               opt_int32 live_period = {}, opt_bool disable_notification = {},
               opt_int64 reply_to_message_id = {},
               std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error> editMessageLiveLocation(
      IntOrString chat_id, int64_t message_id, float latitude, float longitude,
      opt_string_view inline_message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<Message, opt_error> editMessageLiveLocation(
      float latitude, float longitude, std::string_view inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<Message, opt_error> stopMessageLiveLocation(
      IntOrString chat_id, int64_t message_id,
      opt_string_view inline_message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<Message, opt_error> stopMessageLiveLocation(
      std::string_view inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<Message, opt_error> sendVenue(
      IntOrString chat_id, float latitude, float longitude,
      std::string_view title, std::string_view address,
      opt_string_view foursquare_id = {}, opt_string_view foursquare_type = {},
      opt_bool disable_notification = {}, opt_int64 reply_to_message_id = {},
      std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<Message, opt_error>
  sendContact(IntOrString chat_id, std::string_view phone_number,
              std::string_view first_name, opt_string_view last_name = {},
              opt_string_view vcard = {}, opt_bool disable_notification = {},
              opt_int64 reply_to_message_id = {},
              std::optional<ReplyMarkups> reply_markup = {}) const;
  std::pair<Message, opt_error>
  sendPoll(IntOrString chat_id, std::string_view question,
           const std::vector<std::string_view> &options,
           opt_bool is_anonymous = {}, opt_bool allows_multiple_answers = {},
           opt_string_view type = {}, opt_int32 correct_option_id = {},
           opt_bool is_closed = {}, opt_bool disable_notification = {},
           opt_int64 reply_to_message_id = {},
           std::optional<ReplyMarkups> reply_markup = {}) const;

  std::pair<bool, opt_error> sendChatAction(IntOrString chat_id,
                                            std::string_view action) const;

  std::pair<UserProfilePhotos, opt_error>
  getUserProfilePhotos(int64_t user_id, opt_int32 offset = {},
                       opt_int32 limit = {}) const;
  std::pair<File, opt_error> getFile(std::string_view &file_id) const;

  std::pair<bool, opt_error> kickChatMember(IntOrString chat_id,
                                            int64_t user_id,
                                            opt_int32 until_date = {}) const;

  std::pair<bool, opt_error> unbanChatMember(IntOrString chat_id,
                                             int64_t user_id) const;

  std::pair<bool, opt_error>
  restrictChatMember(IntOrString chat_id, int64_t user_id,
                     const ChatPermissions &perms,
                     opt_int64 until_date = {}) const;

  std::pair<bool, opt_error> promoteChatMember(
      IntOrString chat_id, int64_t user_id, opt_bool can_change_info = {},
      opt_bool can_post_messages = {}, opt_bool can_edit_messages = {},
      opt_bool can_delete_messages = {}, opt_bool can_invite_users = {},
      opt_bool can_restrict_members = {}, opt_bool can_pin_messages = {},
      opt_bool can_promote_members = {});

  std::pair<bool, opt_error>
  setChatAdministratorCustomTitle(IntOrString chat_id, int64_t user_id,
                                  std::string_view custom_title) const;

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
                  opt_string_view parse_mode = {},
                  opt_bool disable_web_page_preview = {},
                  std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

  std::pair<std::variant<bool, Message>, opt_error>
  editMessageText(std::string_view text, const std::string &inline_message_id,
                  std::optional<IntOrString> chat_id = {},
                  opt_int64 message_id = {}, opt_string_view parse_mode = {},
                  opt_bool disable_web_page_preview = {},
                  std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
  std::pair<std::variant<bool, Message>, opt_error>

  editMessageCaption(IntOrString chat_id, opt_int64 message_id,
                     opt_string_view inline_message_id = {},
                     opt_string_view caption = {},
                     opt_string_view parse_mode = {},
                     std::optional<InlineKeyboardMarkup> reply_markup = {});
  std::pair<std::variant<bool, Message>, opt_error>

  editMessageCaption(
      const std::string &inline_message_id,
      std::optional<IntOrString> chat_id = {}, opt_int64 message_id = {},
      opt_string_view caption = {}, opt_string_view parse_mode = {},
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

  std::pair<bool, opt_error> deleteWebhook() const;
};

} // namespace telegram
