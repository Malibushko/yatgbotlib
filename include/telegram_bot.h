#pragma once
#include <string>
#include <type_traits>
#include <queue>
#include <cassert>
#include <optional>
#include <variant>
#include "telegram_structs.h"
#include "headers/update_manager.h"
#include "headers/apimanager.h"

namespace telegram {

using o_string = std::optional<std::string>;
using o_bool = std::optional<bool>;
using o_int64 = std::optional<int64_t>;
using o_uint64 = std::optional<uint64_t>;
using oref_string = std::optional<std::string>&;
using o_sv = std::optional<std::string_view>;
using reply_markups = std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>;
using o_error = std::optional<error>;

class Bot {
    std::unique_ptr<api_manager> api;
    update_manager updater;
    bool stopPolling = false;
    bool webhookSet = false;
public:
    Bot(const std::string& token) noexcept;
    void onUpdate(update_callback && cb);
    void onInlineResult(std::string_view cmd,chosen_inline_callback&& cb);
    void onCallback(std::string_view cmd,query_callback&& cb);
    void onCommand(std::string_view cmd,msg_callback&& cb);
    void startSequence(int64_t user_id,std::shared_ptr<sequence<msg_callback>> seq);
    void stopSequence(int64_t user_id);
    std::pair<WebhookInfo,o_error> getWebhookInfo() const;
    void start(std::optional<uint32_t> timeout = {},
               std::optional<uint32_t> offset = {},
               std::optional<uint8_t> limit = {},
               std::optional<std::vector<std::string_view>> allowed_updates = {});

    void stop();
    /// A simple method for testing your bot's auth token.
    /// Requires no parameters. Returns basic information about the bot in form of a User object.
    std::pair<User,o_error> getMe() const;

    /// Use this method to send text messages. On success, the sent Message is returned.
    using IntOrString = std::variant<int64_t,std::string>;
    using ReplyMarkups = std::variant<InlineKeyboardMarkup,ReplyKeyboardMarkup,ReplyKeyboardRemove,ForceReply>;
    std::pair<Message,o_error> sendMessage(IntOrString chat_id,const std::string& text,
                     o_string parse_mode = {},
                     o_bool disable_web_page_preview = {},
                     o_bool disable_notification = {},
                     o_int64 reply_to_message_id = {},
                     std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> reply(const Message& msg,const std::string& message) const;
    std::pair<Message,o_error> reply(const Message& msg,const std::string& text,const ReplyMarkups& markup) const;

    std::pair<std::vector<Update>,o_error> getUpdates(std::optional<uint32_t> offset = {},
                    std::optional<uint8_t> limit = {},
                    std::optional<uint32_t> timeout = {},
                    std::optional<std::vector<std::string_view>> allowed_updates = {}) const;

    std::string getUpdatesRawJson(std::optional<uint32_t> offset = {},
                                  std::optional<uint8_t> limit = {},
                                  std::optional<uint32_t> timeout = {},
                                  std::optional<std::vector<std::string_view>> allowed_updates = {});

    /// Use this method to forward messages of any kind. On success, the sent Message is returned.
    std::pair<Message,o_error> forwardMessage(IntOrString chat_id,IntOrString from_chat_id,
                        int64_t message_id,o_bool disable_notification = {}) const;

    std::pair<Message,o_error> sendPhoto(IntOrString chat_id,const std::string& photo,
                   std::string_view caption= {},std::string_view parse_mode = {},
                   o_bool disable_notification= {},o_int64 reply_to_message_id= {},
                   std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> sendAudio(IntOrString chat_id,std::string audio,
                   o_sv caption = {},o_sv parse_mode = {},
                   std::optional<int> duration= {},o_sv performer = {},
                   o_sv title = {},const std::optional<std::string>& thumb = {},
                   o_bool disable_notification = {},o_int64 reply_to_message_id = {},
                   std::optional<ReplyMarkups> reply_markup= {}) const;

    std::pair<bool,o_error> setWebhook(std::string_view  url,const std::optional<std::string>& certificate = {},
                    const std::optional<uint32_t> max_connections = {},
                    const std::optional<std::vector<std::string_view>> & allowed_updates = {}) const;

    bool setWebhookServer(const std::string& url,uint16_t port,const std::string& cert_path = "./cert.pem",
                          const std::string& key_path = "./key.pem");

    std::pair<Message,o_error> sendDocument(IntOrString chat_id,const std::string& document,
                      const std::optional<std::string> thumb = {},o_sv caption = {},
                      o_sv parse_mode = {},o_bool disable_notification = {},
                      o_int64 reply_to_message_id = {},
                      std::optional<ReplyMarkups> reply_markup = {}) const;

    using o_int32 = std::optional<int32_t>;
    std::pair<Message,o_error> sendVideo(IntOrString chat_id,const std::string& video,
                   o_int32 duration = {},o_int32 width= {},o_int32 height = {},
                   const std::optional<std::string>& thumb = {},o_sv caption = {},
                   o_sv parse_mode = {},o_bool supports_streaming = {},
                   o_bool disable_notification = {},o_int64 reply_to_message_id = {},
                   std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> sendAnimation(IntOrString chat_id,const std::string& animation,
                       o_int32 duration = {},o_int32 width= {},o_int32 height = {},
                       const std::optional<std::string>& thumb = {},o_sv caption = {},
                       o_sv parse_mode = {},o_bool supports_streaming = {},
                       o_bool disable_nofitication = {},o_int64 reply_to_message_id = {},
                       std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> sendVoice(IntOrString chat_id,const std::string& voice,
                   o_sv caption,o_sv parse_mode,
                   o_int32 duration,o_bool disable_notification,
                   o_int64 reply_to_message_id,
                   std::optional<ReplyMarkups> reply_markup) const;

    std::pair<Message,o_error> sendVideoNote(IntOrString chat_id,const std::string& video_note,
                       o_int32 duration = {},o_int32 length= {},
                       const std::optional<std::string>& thumb = {},
                       o_bool disable_notification = {},
                       o_int64 reply_to_message_id = {},
                       std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> sendLocation(IntOrString chat_id,float latitude,float longitude,
                      o_int32 live_period = {},o_bool disable_notification = {},
                      o_int64 reply_to_message_id = {},std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> editMessageLiveLocation(IntOrString chat_id,
                                 int64_t message_id,
                                 float latitude,float longitude,
                                 o_sv inline_message_id = {},
                                 std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<Message,o_error> editMessageLiveLocation(float latitude,float longitude,
                                 std::string_view inline_message_id,
                                 std::optional<IntOrString> chat_id = {},o_int64 message_id = {},
                                 std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<Message,o_error> stopMessageLiveLocation(IntOrString chat_id,
                                 int64_t message_id,
                                 o_sv inline_message_id = {},
                                 std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<Message,o_error> stopMessageLiveLocation(std::string_view  inline_message_id,
                                 std::optional<IntOrString> chat_id = {},
                                 o_int64 message_id = {},
                                 std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<Message,o_error> sendVenue(IntOrString chat_id,float latitude,float longitude,
                   std::string_view title,std::string_view address,
                   o_sv foursquare_id={},o_sv foursquare_type={},
                   o_bool disable_notification={},o_int64 reply_to_message_id={},
                   std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<Message,o_error> sendContact(IntOrString chat_id,std::string_view phone_number,
                     std::string_view first_name,o_sv last_name = {},
                     o_sv vcard = {},o_bool disable_notification = {},
                     o_int64 reply_to_message_id = {},std::optional<ReplyMarkups> reply_markup = {}) const
    ;
    std::pair<Message,o_error> sendPoll(IntOrString chat_id,std::string_view  question,
                  const std::vector<std::string_view>& options,
                  o_bool is_anonymous = {},o_bool allows_multiple_answers = {},
                  o_sv type = {},o_int32 correct_option_id = {},
                  o_bool is_closed = {},o_bool disable_notification = {},
                  o_int64 reply_to_message_id = {},std::optional<ReplyMarkups> reply_markup = {}) const;

    std::pair<bool,o_error> sendChatAction(IntOrString chat_id,std::string_view  action) const;

    std::pair<UserProfilePhotos,o_error> getUserProfilePhotos(int64_t user_id,
                              o_int32 offset = {},
                              o_int32 limit = {}) const;
    std::pair<File,o_error> getFile(std::string_view  & file_id) const;

    std::pair<bool,o_error>  kickChatMember(IntOrString chat_id,int64_t user_id,o_int32 until_date = {}) const;

    std::pair<bool,o_error> unbanChatMember(IntOrString chat_id,int64_t user_id) const;

    std::pair<bool,o_error> restrictChatMember(IntOrString chat_id,int64_t user_id,const ChatPermissions& perms,
                            o_int64 until_date = {}) const;

    std::pair<bool,o_error> promoteChatMember(IntOrString chat_id,int64_t user_id,
                           o_bool can_change_info = {},
                           o_bool can_post_messages = {},
                           o_bool can_edit_messages = {},
                           o_bool can_delete_messages = {},
                           o_bool can_invite_users = {},
                           o_bool can_restrict_members = {},
                           o_bool can_pin_messages = {},
                           o_bool can_promote_members = {});

    std::pair<bool,o_error> setChatAdministratorCustomTitle(IntOrString chat_id,int64_t user_id,
                                         std::string_view  custom_title) const;

    std::pair<bool,o_error> setChatPermissions(IntOrString chat_id,const ChatPermissions& permissions) const;

    std::pair<std::string,o_error> exportChatInviteLink(IntOrString chat_id) const;

    std::pair<bool,o_error> setChatPhoto(IntOrString chat_id,const std::string& photo) const;

    std::pair<bool,o_error> deleteChatPhoto(IntOrString chat_id) const;

    std::pair<bool,o_error> setChatTitle(IntOrString chat_id,std::string_view title) const;

    std::pair<bool,o_error> setChatDescription(IntOrString chat_id,o_sv description) const;

    std::pair<bool,o_error> pinChatMessage(IntOrString chat_id,int64_t message_id,o_bool disable_notification = {}) const;

    std::pair<bool,o_error> unpinChatMessage(IntOrString chat_id) const;

    std::pair<bool,o_error> leaveChat(IntOrString chat_id) const;

    std::pair<Chat,o_error> getChat(IntOrString chat_id) const;

    std::pair<std::vector<ChatMember>,o_error>  getChatAdministrators(IntOrString chat_id) const;

    std::pair<uint32_t,o_error>   getChatMembersCount(IntOrString chat_id) const;

    std::pair<ChatMember,o_error> getChatMember(IntOrString chat_id,uint32_t user_id) const;

    std::pair<bool,o_error> setChatStickerSet(IntOrString chat_id,std::string_view sticker_set_name) const;

    std::pair<bool,o_error> deleteChatStickerSet(IntOrString chat_id) const;

    std::pair<bool,o_error> answerCallbackQuery(std::string_view callback_query_id,
                             o_sv text ={},
                             o_bool show_alert ={},
                             o_sv url = {},
                             o_int32 cache_time = {}) const;

    std::pair<std::variant<bool,Message>,o_error> editMessageText(IntOrString chat_id,
                         std::string_view text,
                         o_int64 message_id,
                         o_sv inline_message_id = {},
                         o_sv parse_mode = {},
                         o_bool disable_web_page_preview = {},
                         std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<std::variant<bool,Message>,o_error>
    editMessageText(std::string_view  text,const std::string& inline_message_id,
                         std::optional<IntOrString> chat_id = {},
                         o_int64 message_id = {},
                         o_sv parse_mode = {},
                         o_bool disable_web_page_preview = {},
                         std::optional<InlineKeyboardMarkup> reply_markup = {}) const;
    std::pair<std::variant<bool,Message>,o_error>

    editMessageCaption(IntOrString chat_id,
                            o_int64 message_id,
                            o_sv inline_message_id = {},
                            o_sv caption = {},
                            o_sv parse_mode = {},
                            std::optional<InlineKeyboardMarkup> reply_markup = {});
    std::pair<std::variant<bool,Message>,o_error>

    editMessageCaption(const std::string& inline_message_id,std::optional<IntOrString> chat_id = {},
                            o_int64 message_id = {},
                            o_sv caption = {},
                            o_sv parse_mode = {},
                            std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    using InputMedia = std::variant<InputMediaAnimation,InputMediaDocument,InputMediaAudio,
    InputMediaPhoto,InputMediaVideo>;

    std::pair<std::variant<bool,Message>,o_error>
    editMessageMedia(InputMedia media,
                          o_int64 inline_message_id,
                          std::optional<IntOrString> chat_id = {},
                          o_int64 message_id = {},
                          std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<std::variant<bool,Message>,o_error>
    editMessageMedia(InputMedia media,
                          IntOrString chat_id,
                          int64_t message_id,
                          o_sv inline_message_id = {},
                          std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<std::variant<bool,Message>,o_error>
    editMessageReplyMarkup(std::string_view inline_message_id,std::optional<IntOrString> chat_id = {},
                                o_int64 message_id = {},
                                std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<std::variant<bool,Message>,o_error>
    editMessageReplyMarkup(IntOrString chat_id,
                                int64_t message_id,
                                std::string_view  inline_message_id = {},
                                std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<bool,o_error> deleteMessage(IntOrString chat_id,int64_t message_id) const;

    std::pair<Poll,o_error> stopPoll(IntOrString chat_id,int64_t message_id,
                  std::optional<InlineKeyboardMarkup> reply_markup = {}) const;

    std::pair<Message,o_error> sendSticker(IntOrString chat_id,const std::string& sticker,
                     o_bool disable_notification,
                     o_int64 reply_to_message_id,
                     std::optional<ReplyMarkups> reply_markup) const;

    std::pair<StickerSet,o_error> getStickerSet(const std::string& name) const;

    std::pair<File,o_error> uploadStickerFile(int64_t user_id,const std::string& png_sticker) const;

    std::pair<bool,o_error> createNewStickerSet(int64_t user_id,std::string_view  name,
                             std::string_view  title,const std::string& png_sticker,
                             std::string_view  emojis,
                             o_bool contains_masks = {},
                             const std::optional<MaskPosition>& mask_position = {})const;

    std::pair<bool,o_error> addStickerToSet (int64_t user_id,std::string_view  name,
                          const std::string& png_sticker,
                          std::string_view emojis,
                          const std::optional<MaskPosition>& mask_position = {}) const;

    std::pair<bool,o_error> setStickerPositionInSet(std::string_view sticker,
                                 int32_t position) const;

    std::pair<bool,o_error>deleteStickerFromSet(const std::string& sticker) const;

    std::pair<Message,o_error> sendInvoice(int64_t chat_id,
                     std::string_view  title,
                     std::string_view  description,
                     std::string_view  payload,
                     std::string_view  provider_token,
                     std::string_view  start_parameter,
                     std::string_view  currency,
                     const std::vector<LabeledPrice>& prices,
                     o_sv provider_data = {},
                     o_sv photo_url = {},
                     o_int32 photo_size = {},
                     o_int32 photo_width = {},
                     o_int32 photo_height = {},
                     o_bool need_name = {},
                     o_bool need_phone_number = {},
                     o_bool need_email = {},
                     o_bool need_shipping_address = {},
                     o_bool send_phone_number_to_provider = {},
                     o_bool send_email_to_provider = {},
                     o_bool is_flexible = {},
                     o_bool disable_notification = {},
                     o_int32 reply_to_message_id = {},
                     const std::optional<InlineKeyboardMarkup>& reply_markup = {}) const;

    std::pair<bool,o_error> answerShippingQuery(std::string_view  shipping_query_id,std::false_type,
                             std::string_view  error_message) const;

    std::pair<bool,o_error> answerShippingQuery(std::string_view  shipping_query_id,
                             std::true_type,
                             const std::vector<ShippingOption>& shipping_options)const;

    std::pair<bool,o_error> answerPreCheckoutQuery(std::string_view pre_checkout_query_id,
                                bool ok = true,
                                o_sv error_message = {}) const;

    std::pair<bool,o_error> answerPreCheckoutQuery(std::string_view  pre_checkout_query_id,
                                std::false_type,
                                std::string_view  error_message) const;

    using PassportElementError = std::variant<PassportElementErrorDataField,
                                              PassportElementErrorFrontSide,
                                              PassportElementErrorReverseSide,
                                              PassportElementErrorSelfie,
                                              PassportElementErrorFile,
                                              PassportElementErrorFiles,
                                              PassportElementErrorTranslationFile,
                                              PassportElementErrorTranslationFiles,
                                              PassportElementErrorUnspecified>;

    std::pair<bool,o_error> setPassportDataErrors(int64_t user_id,
                               const std::vector<PassportElementError>& errors) const;

    std::pair<Message,o_error> sendGame(int64_t chat_id,std::string_view game_short_name,
                  o_bool disable_notification = {},
                  o_int32 duration = {},
                  std::optional<InlineKeyboardMarkup> reply_markup = {})const;

    std::pair<std::variant<bool,Message>,o_error> setGameScore(int64_t user_id,int32_t score,
                      std::string_view inline_message_id,
                      o_int64 chat_id = {},
                      o_int64 message_id = {},
                      o_bool force = {},
                      o_bool disable_edit_message = {}) const;

    std::pair<std::variant<bool,Message>,o_error> setGameScore(int64_t user_id,uint32_t score,
                      int64_t chat_id,
                      int64_t message_id,
                      o_sv inline_message_id = {},
                      o_bool force = {},
                      o_bool disable_edit_message = {}) const;

    std::pair<bool,o_error> getGameHighScores(int64_t user_id,
                           int64_t chat_id,
                           int64_t message_id,
                           o_sv inline_message_id = {}) const;

    std::pair<bool,o_error> getGameHighScores(int64_t user_id,std::string_view inline_message_id,
                           o_int64 chat_id = {},
                           o_int64 message_id = {}) const;

    std::pair<bool,o_error> deleteWebhook() const;
};

}
