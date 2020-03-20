#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <variant>
#include "json_parser.h"

namespace telegram {
struct Message;
struct KeyboardButton;
struct InlineKeyboardButton;
struct Bot;
/// this struct is not defined in telegram documentation
struct CallbackGame {};
/**
  This object represents a file ready to be downloaded.
 The file can be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>.
 It is guaranteed that the link will be valid for at least 1 hour.
 When the link expires, a new one can be requested by calling getFile.
 Maximum file size to download is 20 MB
*/
struct File {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
    DECLARE_FIELD(std::optional<std::string>,file_path);
};
using InputFile = File;
/**
 * Upon receiving a message with this object, Telegram clients will display
 *  a reply interface to the user (act as if the user has selected the bot‘s message and tapped ’Reply').
 *  This can be extremely useful if you want to create user-friendly step-by-step interfaces without
 *  having to sacrifice privacy mode.
 */
struct ForceReply {
    DECLARE_STRUCT
    DECLARE_FIELD(bool,force_reply);
    DECLARE_FIELD(bool,selective);
};

///This object represents a Telegram user or bot.
struct User {
    DECLARE_STRUCT
    DECLARE_FIELD(int64_t,id); ///	Unique identifier for this user or bot
    DECLARE_FIELD(bool,is_bot);/// True, if this user is a bot
    DECLARE_FIELD(std::string,first_name);   /// User‘s or bot’s first name
    DECLARE_FIELD(std::optional<std::string>,last_name);/// User‘s or bot’s last name
    DECLARE_FIELD(std::optional<std::string>,username);///. User‘s or bot’s username
    DECLARE_FIELD(std::optional<std::string>,language_code);///  IETF language tag of the user's language
    DECLARE_FIELD(std::optional<bool>,can_join_groups);///True, if the bot can be invited to groups. Returned only in getMe.
    DECLARE_FIELD(std::optional<bool>,can_read_all_groups_messages);///True, if privacy mode is disabled for the bot. Returned only in getMe.
    DECLARE_FIELD(std::optional<bool>,supports_inline_queries);///. True, if the bot supports inline queries. Returned only in getMe.
};
/// This object contains information about one member of a chat.
struct ChatMember {
    DECLARE_STRUCT
    DECLARE_FIELD(User,user);
    DECLARE_FIELD(std::string,status);
    DECLARE_FIELD(std::optional<std::string>,custom_title);
    DECLARE_FIELD(std::optional<uint64_t>,until_date);
    DECLARE_FIELD(std::optional<bool>,can_be_edited);
    DECLARE_FIELD(std::optional<bool>,can_post_messages);
    DECLARE_FIELD(std::optional<bool>,can_edit_messages);
    DECLARE_FIELD(std::optional<bool>,can_delete_messages);
    DECLARE_FIELD(std::optional<bool>,can_restrict_messages);
    DECLARE_FIELD(std::optional<bool>,can_promote_messages);
    DECLARE_FIELD(std::optional<bool>,can_change_info);
    DECLARE_FIELD(std::optional<bool>,can_invite_users);
    DECLARE_FIELD(std::optional<bool>,can_pin_messages);
    DECLARE_FIELD(std::optional<bool>,is_member);
    DECLARE_FIELD(std::optional<bool>,can_send_messages);
    DECLARE_FIELD(std::optional<bool>,can_send_media_messages);
    DECLARE_FIELD(std::optional<bool>,can_send_polls);
    DECLARE_FIELD(std::optional<bool>,can_send_other_messages);
    DECLARE_FIELD(std::optional<bool>,can_add_web_page_previews);
};


/// Describes actions that a non-administrator user is allowed to take in a chat
struct ChatPermissions {
    DECLARE_STRUCT
    DECLARE_FIELD(std::optional<bool>,can_send_messages);
    DECLARE_FIELD(std::optional<bool>,can_send_media_messages);
    DECLARE_FIELD(std::optional<bool>,can_send_polls);
    DECLARE_FIELD(std::optional<bool>,can_send_other_messages);
    DECLARE_FIELD(std::optional<bool>,can_add_web_page_previews);
    DECLARE_FIELD(std::optional<bool>,can_change_info);
    DECLARE_FIELD(std::optional<bool>,can_invite_users);
    DECLARE_FIELD(std::optional<bool>,can_pin_messages);
};

/// Contains information about why a request was unsuccessful.
struct ResponseParameters {
    DECLARE_STRUCT
    DECLARE_FIELD(std::optional<int64_t>,migrate_to_chat_id);
    DECLARE_FIELD(std::optional<int32_t>,retry_after);
};
/// Represents a photo to be sent.
struct InputMediaPhoto {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,media);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
};
/// Represents a video to be sent.
using inn_type = std::variant<InputFile,std::string>;
struct InputMediaVideo {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,media);
    // InputMediaPhoto and string
    DECLARE_FIELD(std::optional<std::variant<inn_type>>,thumb);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<uint32_t>,width);
    DECLARE_FIELD(std::optional<uint32_t>,height);
    DECLARE_FIELD(std::optional<uint32_t>,duration);
    DECLARE_FIELD(std::optional<bool>,supports_streaming);
};
///Represents an animation file (GIF or H.264/MPEG-4 AVC video without sound) to be sent.
struct InputMediaAnimation {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,media);
    DECLARE_FIELD(std::optional<inn_type>,thumb);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<uint32_t>,width);
    DECLARE_FIELD(std::optional<uint32_t>,height);
    DECLARE_FIELD(std::optional<uint32_t>,duration);
};
/// Represents an audio file to be treated as music to be sent.
struct InputMediaAudio {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,media);
    DECLARE_FIELD(std::optional<inn_type>,thumb);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<uint32_t>,duration);
    DECLARE_FIELD(std::optional<std::string>,performer);
};
/// Represents a general file to be sent.
struct InputMediaDocument {
    DECLARE_STRUCT;
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,media);
    DECLARE_FIELD(std::optional<inn_type>,thumb);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
};
/// This object represents one size of a photo or a file / sticker thumbnail.
struct PhotoSize {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,width);
    DECLARE_FIELD(uint32_t,height);
    DECLARE_FIELD(std::optional<uint64_t>,photo_size);
};

/// This object represents an audio file to be treated as music by the Telegram clients.
struct Audio {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(int32_t,duration);
    DECLARE_FIELD(std::optional<std::string>,performer);
    DECLARE_FIELD(std::optional<std::string>,title);
    DECLARE_FIELD(std::optional<std::string>,mime_type);
    DECLARE_FIELD(std::optional<uint32_t>,file_size);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
};

/// This object represents a chat photo.
struct ChatPhoto {
    DECLARE_STRUCT
    /**
     * File identifier of small (160x160) chat photo.
     * This file_id can be used only for photo
     * download and only for as long as the photo is not changed.
     */
    DECLARE_FIELD(std::string,small_file_id);
    /**
     * Unique file identifier of small (160x160) chat photo,
     *  which is supposed to be the same over time and for different bots.
     Can't be used to download or reuse the file.
     */
    DECLARE_FIELD(std::string,small_file_unique_id);
    /**
     * File identifier of big (640x640) chat photo.
     *  This file_id can be used only for photo download
     * and only for as long as the photo is not changed.
     */
    DECLARE_FIELD(std::string,big_file_id);
    /**
     * Unique file identifier of big (640x640) chat photo,
     *  which is supposed to be the same over time and for
     *  different bots. Can't be used to download or reuse the file.
     */
    DECLARE_FIELD(std::string,big_file_unique_id);
};

/// This object represents a chat.
struct Chat {
    DECLARE_STRUCT
    /**
     * Unique identifier for this chat.
     * This number may be greater than 32 bits and some programming languages
     * may have difficulty/silent defects in interpreting it.
     * But it is smaller than 52 bits, so a signed 64 bit integer or double-precision
     *  float type are safe for storing this identifier.
     */
    DECLARE_FIELD(int64_t,id);
    DECLARE_FIELD(std::string,type); /// 	Type of chat, can be either “private”, “group”, “supergroup” or “channel”
    DECLARE_FIELD(std::optional<std::string>,title); ///Title, for supergroups, channels and group chats
    DECLARE_FIELD(std::optional<std::string>,username);///Username, for private chats, supergroups and channels if available
    DECLARE_FIELD(std::optional<std::string>,first_name);///First name of the other party in a private chat
    DECLARE_FIELD(std::optional<std::string>,last_name);///Last name of the other party in a private chat
    DECLARE_FIELD(std::optional<ChatPhoto>,photo);///. Chat photo. Returned only in getChat.
    DECLARE_FIELD(std::optional<std::string>,description);///Description, for groups, supergroups and channel chats. Returned only in getChat.
    DECLARE_FIELD(std::optional<std::string>,invite_link); ///Chat invite link, for groups, supergroups and channel chats. Each administrator in a chat generates their own invite links, so the bot must first generate the link using exportChatInviteLink. Returned only in getChat.
    DECLARE_FIELD(std::optional<std::unique_ptr<Message>>,pinned_message); ///Pinned message, for groups, supergroups and channels. Returned only in getChat.
    DECLARE_FIELD(std::optional<ChatPermissions>,permissions); /// Default chat member permissions, for groups and supergroups. Returned only in getChat.
    DECLARE_FIELD(std::optional<int32_t>,slow_mode_delay);///For supergroups, the minimum allowed delay between consecutive messages sent by each unpriviledged user. Returned only in getChat.
    DECLARE_FIELD(std::optional<std::string>,sticker_set_name);///For supergroups, name of group sticker set. Returned only in getChat.
    DECLARE_FIELD(std::optional<bool>,can_set_sticket_get);///True, if the bot can change the group sticker set. Returned only in getChat.
};

/// This object represents one special entity in a text message. For example, hashtags, usernames, URLs, etc.
struct MessageEntity {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(int32_t,offset);
    DECLARE_FIELD(int32_t,length);
    DECLARE_FIELD(std::optional<std::string>,url);
    DECLARE_FIELD(std::optional<User>,user);
    DECLARE_FIELD(std::optional<std::string>,language);
};
///This object represents a general file (as opposed to photos, voice messages and audio files).
struct Document {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
    DECLARE_FIELD(std::optional<std::string>,file_name);
    DECLARE_FIELD(std::optional<std::string>,mime_type);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
};
///This object represents a video file.
struct Video {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,width);
    DECLARE_FIELD(uint32_t,height);
    DECLARE_FIELD(uint32_t,duration);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
    DECLARE_FIELD(std::optional<std::string>,mime_type);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
};
/// This object represents an animation file (GIF or H.264/MPEG-4 AVC video without sound)
struct Animation {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,width);
    DECLARE_FIELD(uint32_t,height);
    DECLARE_FIELD(uint32_t,duration);
    DECLARE_FIELD(std::optional<std::string>,file_name);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
    DECLARE_FIELD(std::optional<std::string>,mime_type);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
};
/// This object represents a voice note.
struct Voice {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,duration);
    DECLARE_FIELD(std::optional<std::string>,mime_type);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
};

///This object represents a video message (available in Telegram apps as of v.4.0).
struct VideoNote {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,length);
    DECLARE_FIELD(uint32_t,duration);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
    DECLARE_FIELD(std::optional<uint64_t>,file_size);
};
/// This object represents a phone contact.
struct Contact {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,phone_number);
    DECLARE_FIELD(std::string,first_name);
    DECLARE_FIELD(std::optional<std::string>,last_name);
    DECLARE_FIELD(std::optional<int64_t>,user_id);
    DECLARE_FIELD(std::optional<std::string>,vcard);
};

/// This object represents a point on the map.
struct Location {
    DECLARE_STRUCT
    DECLARE_FIELD(float,longitude);
    DECLARE_FIELD(float,latitude);
};
/// This object represents a venue.
struct Venue {
    DECLARE_STRUCT
    DECLARE_FIELD(Location,location);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,address);
    DECLARE_FIELD(std::optional<std::string>,foursquare_id);
    DECLARE_FIELD(std::optional<std::string>,foursquare_type);
};

/// This object contains information about one answer option in a poll.
struct PollOption {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,text);
    DECLARE_FIELD(int32_t,voter_count);
};
/// This object represents an answer of a user in a non-anonymous poll.
struct PollAnswer {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,poll_id);
    DECLARE_FIELD(User,user);
    DECLARE_FIELD(std::vector<int>,option_ids);
};

/// This object contains information about a poll.
struct Poll {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,question);
    DECLARE_FIELD(std::vector<PollOption>,options);
    DECLARE_FIELD(uint32_t,total_voter_count);
    DECLARE_FIELD(bool,is_closed);
    DECLARE_FIELD(bool,is_anonymous);
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(bool,allow_multiple_answers);
    DECLARE_FIELD(std::optional<int16_t>,correct_option_id);
};
/// This object represent a user's profile pictures.
struct UserProfilePhotos {
    DECLARE_STRUCT
    DECLARE_FIELD(int32_t,total_count);
    DECLARE_FIELD(std::vector<std::vector<PhotoSize>>,photos);
};

///This object represents a custom keyboard with reply options
struct ReplyKeyboardMarkup {
    DECLARE_STRUCT
    DECLARE_FIELD(std::vector<std::vector<KeyboardButton>>,keyboard);
    DECLARE_FIELD(std::optional<bool>,resize_keyboard);
    DECLARE_FIELD(std::optional<bool>,one_time_keyboard);
    DECLARE_FIELD(std::optional<bool>,selective);
};
/// This object represents type of a poll, which is allowed to be created
///  and sent when the corresponding button is pressed
struct KeyboardButtonPollType {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
};

/**
 * This object represents one button of the reply keyboard.
 * For simple text buttons String can be used instead of this object to specify text of the button.
 * Optional fields request_contact, request_location, and request_poll are mutually exclusive.
 */
struct KeyboardButton {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,text);
    DECLARE_FIELD(std::optional<bool>,request_contact);
    DECLARE_FIELD(std::optional<bool>,request_location);
    DECLARE_FIELD(std::optional<KeyboardButtonPollType>,request_poll);
};

/**
 * Upon receiving a message with this object,
 * Telegram clients will remove the current custom keyboard and display the default
 * letter-keyboard. By default, custom keyboards are displayed until a new keyboard
 * is sent by a bot. An exception is made for one-time keyboards that are hidden
 * immediately after the user presses a button
*/
struct ReplyKeyboardRemove {
    DECLARE_STRUCT
    DECLARE_FIELD(bool,remove_keyboard);
    DECLARE_FIELD(bool,selective);
};

/// This object represents an inline keyboard that appears right next to the message it belongs to.
struct InlineKeyboardMarkup {
    DECLARE_STRUCT
    DECLARE_FIELD(std::vector<std::vector<InlineKeyboardButton>>,inline_keyboard);
};

/**
 * This object represents a parameter of the inline keyboard button
 * used to automatically authorize a user. Serves as a great replacement
 * for the Telegram Login Widget when the user is coming from Telegram.
 */
struct LoginUrl {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,url);
    DECLARE_FIELD(std::optional<std::string>,forward_text);
    DECLARE_FIELD(std::optional<std::string>,bot_username);
    DECLARE_FIELD(std::optional<bool>,request_write_access);
};
/// This object represents one row of the high scores table for a game.
struct GameHighScore {
    DECLARE_STRUCT
    DECLARE_FIELD(uint32_t,position);
    DECLARE_FIELD(User,user);
    DECLARE_FIELD(uint64_t,score);
};

/// This object represents one button of an inline keyboard. You must use exactly one of the optional fields.
struct InlineKeyboardButton {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,text);
    DECLARE_FIELD(std::optional<std::string>,url);
    DECLARE_FIELD(std::optional<LoginUrl>,login_url);
    DECLARE_FIELD(std::optional<std::string>,callback_data);
    DECLARE_FIELD(std::optional<std::string>,switch_inline_query);
    DECLARE_FIELD(std::optional<std::string>,switch_inline_query_current_chat);
    DECLARE_FIELD(std::optional<CallbackGame>,callback_game);
    DECLARE_FIELD(std::optional<bool>,pay);
};
/// This object represents a game.
/// Use BotFather to create and edit games, their short names will act as unique identifiers.

struct Game {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,description);
    DECLARE_FIELD(std::vector<PhotoSize>,photo);
    DECLARE_FIELD(std::optional<std::string>,text);
    DECLARE_FIELD(std::optional<std::vector<MessageEntity>>,text_entities);
    DECLARE_FIELD(std::optional<Animation>,animation);
};

/// This object contains basic information about an invoice.

struct Invoice {
    DECLARE_STRUCT
    DECLARE_FIELD(uint32_t,total_amount);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,description);
    DECLARE_FIELD(std::string,start_parameter);
    DECLARE_FIELD(std::string,currency);
};
/// This object describes the position on faces where a mask should be placed by default.

struct MaskPosition {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,point);
    DECLARE_FIELD(float,x_shift);
    DECLARE_FIELD(float,y_shift);
    DECLARE_FIELD(float,scale);
};
/// This object represents a sticker.

struct Sticker {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint32_t,width);
    DECLARE_FIELD(uint32_t,height);
    DECLARE_FIELD(bool,is_animated);
    DECLARE_FIELD(std::optional<PhotoSize>,thumb);
    DECLARE_FIELD(std::optional<std::string>,emoji);
    DECLARE_FIELD(std::optional<std::string>,set_name);
    DECLARE_FIELD(std::optional<MaskPosition>,mask_position);
    DECLARE_FIELD(std::optional<uint32_t>,file_size);
};
/// This object represents a sticker set.
struct StickerSet {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,name);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(bool,is_animated);
    DECLARE_FIELD(bool,contains_masks);
    DECLARE_FIELD(std::vector<Sticker>,stickers);
};
/// This object represents a portion of the price for goods or services.
struct LabeledPrice {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,label);
    DECLARE_FIELD(uint64_t,amount);
};
/// This object represents one shipping option.
struct ShippingOption {
    DECLARE_STRUCT;
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::vector<LabeledPrice>,prices);
};

/// This object represents a shipping address.
struct ShippingAddress {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,country_code);    // NOLINT
    DECLARE_FIELD(std::string,state);           // NOLINT
    DECLARE_FIELD(std::string,city);            // NOLINT
    DECLARE_FIELD(std::string,street_line1);    // NOLINT
    DECLARE_FIELD(std::string,street_line2);    // NOLINT
    DECLARE_FIELD(std::string,post_code);       // NOLINT
};
/// This object represents information about an order.
struct OrderInfo {
    DECLARE_STRUCT
    DECLARE_FIELD(std::optional<std::string>,name);
    DECLARE_FIELD(std::optional<std::string>,phone_number);
    DECLARE_FIELD(std::optional<std::string>,email);
    DECLARE_FIELD(std::optional<ShippingAddress>,shipping_address);
};

/// This object contains basic information about a successful payment.
struct SuccessfulPayment {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,currency);
    DECLARE_FIELD(int32_t,total_amount);
    DECLARE_FIELD(std::string,invoice_payload);
    DECLARE_FIELD(std::optional<std::string>,shipping_option_id);
    DECLARE_FIELD(std::optional<OrderInfo>,order_info);
    DECLARE_FIELD(std::string,telegram_payment_charge_id);
    DECLARE_FIELD(std::string,provider_payment_charge_id);
};
/// This object contains information about an incoming pre-checkout query.
struct PreCheckoutQuery {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(User,from);
    DECLARE_FIELD(std::string,currency);
    DECLARE_FIELD(uint32_t,total_amount);
    DECLARE_FIELD(std::string,invoice_payload);
    DECLARE_FIELD(std::optional<std::string>,shipping_option_id);
    DECLARE_FIELD(std::optional<OrderInfo>,order_info);
};
/// This object represents a file uploaded to Telegram Passport.
/// Currently all Telegram Passport files are in JPEG format when decrypted and don't exceed 10MB.
struct PassportFile {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,file_id);
    DECLARE_FIELD(std::string,file_unique_id);
    DECLARE_FIELD(uint64_t,file_size);
    DECLARE_FIELD(uint64_t,file_date);
};

/// Contains information about documents or other Telegram Passport elements shared with the bot by the user.

struct EncryptedPassportElement {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,hash);
    DECLARE_FIELD(std::optional<std::string>,data);
    DECLARE_FIELD(std::optional<std::string>,phone_number);
    DECLARE_FIELD(std::optional<std::string>,email);
    DECLARE_FIELD(std::optional<std::vector<PassportFile>>,files);
    DECLARE_FIELD(std::optional<PassportFile>,front_side);
    DECLARE_FIELD(std::optional<PassportFile>,reverse_side);
    DECLARE_FIELD(std::optional<PassportFile>,selfie);
    DECLARE_FIELD(std::optional<std::vector<PassportFile>>,translation);
};

/** Contains data required for decrypting and authenticating EncryptedPassportElement.
  * See the Telegram Passport Documentation for a complete description of the data decryption and
  * authentication processes.
  */
struct EncryptedCredentials {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,data);
    DECLARE_FIELD(std::string,hash);
    DECLARE_FIELD(std::string,secret);
};


/// Contains information about Telegram Passport data shared with the bot by the user.
struct PassportData {
    DECLARE_STRUCT
    DECLARE_FIELD(std::vector<EncryptedPassportElement>,data);
    DECLARE_FIELD(EncryptedCredentials,credentials);
};
/// Represents an issue in one of the data fields that was provided by the user.
/// The error is considered resolved when the field's value changes.
struct PassportElementErrorDataField {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source{"data"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,field_name);
    DECLARE_FIELD(std::string,data_hash);
    DECLARE_FIELD(std::string,message);
};
/// Represents an issue with the front side of a document.
/// The error is considered resolved when the file with the front side of the document changes.
struct PassportElementErrorFrontSide {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"front_side"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,file_hash);
    DECLARE_FIELD(std::string,message);
};
///Represents an issue with the reverse side of a document.
/// The error is considered resolved when the file with reverse side of the document changes.
struct PassportElementErrorReverseSide {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"reverse_side"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,file_hash);
    DECLARE_FIELD(std::string,message);
};
/// Represents an issue with the selfie with a document.
/// The error is considered resolved when the file with the selfie changes
struct PassportElementErrorSelfie {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"selfie"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,file_hash);
    DECLARE_FIELD(std::string,message);
};
/// Represents an issue with a document scan.
/// The error is considered resolved when the file with the document scan changes.
struct PassportElementErrorFile {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,source {"selfie"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,file_hash);
    DECLARE_FIELD(std::string,message);
};
/// Represents an issue with a list of scans.
/// The error is considered resolved when the list of files containing the scans changes.
struct PassportElementErrorFiles {
    DECLARE_STRUCT;
    DECLARE_FIELD(const std::string_view,source {"files"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::vector<std::string>,file_hashes);
    DECLARE_FIELD(std::string,message);
};
/// Represents an issue with one of the files that constitute the translation of a document.
/// The error is considered resolved when the file changes.
struct PassportElementErrorTranslationFile {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"translation_file"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,file_hashe);
    DECLARE_FIELD(std::string,message);
};
struct PassportElementErrorTranslationFiles {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"translation_files"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::vector<std::string>,file_hashes);
    DECLARE_FIELD(std::string,message);
};

/// Represents an issue in an unspecified place.
/// The error is considered resolved when new data is added.
struct PassportElementErrorUnspecified {
    DECLARE_STRUCT
    DECLARE_FIELD(const std::string_view,source {"unspecified"});
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::vector<std::string>,file_hashes);
    DECLARE_FIELD(std::string,message);
};

/// This object represents a message.
struct Message {
    DECLARE_STRUCT
    DECLARE_FIELD(int64_t,message_id);
    DECLARE_FIELD(std::optional<User>,from);
    DECLARE_FIELD(uint64_t,date);
    DECLARE_FIELD(Chat,chat);
    DECLARE_FIELD(std::optional<User>,forward_from);
    DECLARE_FIELD(std::optional<User>,forward_from_chat);
    DECLARE_FIELD(std::optional<int64_t>,forward_from_message_id);
    DECLARE_FIELD(std::optional<std::string>,forward_signature);
    DECLARE_FIELD(std::optional<std::string>,forward_sender_name);
    DECLARE_FIELD(std::optional<uint64_t>,forward_date);
    DECLARE_FIELD(std::optional<std::unique_ptr<Message>>,reply_to_message);
    DECLARE_FIELD(std::optional<uint64_t>,edit_date);
    DECLARE_FIELD(std::optional<std::string>,media_group_id);
    DECLARE_FIELD(std::optional<std::string>,author_signature);
    DECLARE_FIELD(std::optional<std::string>,text);
    DECLARE_FIELD(std::optional<std::vector<MessageEntity>>,entities);
    DECLARE_FIELD(std::optional<std::vector<MessageEntity>>,caption_entities);
    DECLARE_FIELD(std::optional<Audio>,audio);
    DECLARE_FIELD(std::optional<Document>,document);
    DECLARE_FIELD(std::optional<Animation>,animation);
    DECLARE_FIELD(std::optional<Game>,game);
    DECLARE_FIELD(std::optional<std::vector<PhotoSize>>,photo);
    DECLARE_FIELD(std::optional<Sticker>,sticker);
    DECLARE_FIELD(std::optional<Video>,video);
    DECLARE_FIELD(std::optional<Voice>,voice);
    DECLARE_FIELD(std::optional<VideoNote>,video_note);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<Contact>,contact);
    DECLARE_FIELD(std::optional<Location>,location);
    DECLARE_FIELD(std::optional<Venue>,venue);
    DECLARE_FIELD(std::optional<Poll>,poll);
    DECLARE_FIELD(std::optional<std::vector<User>>,new_chat_members);
    DECLARE_FIELD(std::optional<User>,left_chat_member);
    DECLARE_FIELD(std::optional<std::string>,new_chat_title);
    DECLARE_FIELD(std::optional<std::vector<PhotoSize>>,new_chat_photo);
    DECLARE_FIELD(std::optional<bool>,delete_chat_photo);
    DECLARE_FIELD(std::optional<bool>,group_chat_created);
    DECLARE_FIELD(std::optional<bool>,supergroup_chat_created);
    DECLARE_FIELD(std::optional<bool>,channel_chat_created);
    DECLARE_FIELD(std::optional<int64_t>,migrate_to_chat_id);
    DECLARE_FIELD(std::optional<int64_t>,migrate_from_chat_id);
    DECLARE_FIELD(std::optional<std::unique_ptr<Message>>,pinned_message);
    DECLARE_FIELD(std::optional<Invoice>,invoice);
    DECLARE_FIELD(std::optional<SuccessfulPayment>,successfull_payment);
    DECLARE_FIELD(std::optional<std::string>,connected_website);
    DECLARE_FIELD(std::optional<PassportData>,passport_data);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
};
/**
  This object represents an incoming callback query from a callback button in an inline keyboard.
  If the button that originated the query was attached to a message sent by the bot,
  the field message will be present.
  If the button was attached to a message sent via the bot (in inline mode),
  the field inline_message_id will be present.
  Exactly one of the fields data or game_short_name will be present.
*/
struct CallbackQuery {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(User,from);
    DECLARE_FIELD(std::optional<Message>,message);
    DECLARE_FIELD(std::optional<std::string>,inline_message_id);
    DECLARE_FIELD(std::optional<std::string>,chat_instance);
    DECLARE_FIELD(std::optional<std::string>,data);
    DECLARE_FIELD(std::optional<std::string>,game_short_name);
};
/// This object represents an incoming inline query.
/// When the user sends an empty query, your bot could return some default or trending results.
struct InlineQuery {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(User,from);
    DECLARE_FIELD(std::optional<Location>,location);
    DECLARE_FIELD(std::string,query);
    DECLARE_FIELD(std::string,offset);
};

/// Represents the content of a text message to be sent as the result of an inline query.
struct InputTextMessageContent {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,message_text);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<bool>,disable_web_page_preview);
};
/// Represents the content of a location message to be sent as the result of an inline query.
struct InputLocationMessageContent{
    DECLARE_STRUCT
    DECLARE_FIELD(float,latitude);
    DECLARE_FIELD(float,longitude);
    DECLARE_FIELD(std::optional<uint32_t>,live_period);
};
/// Represents the content of a venue message to be sent as the result of an inline query.
struct InputVenueMessageContent {
    DECLARE_STRUCT
    DECLARE_FIELD(float,latitude);
    DECLARE_FIELD(float,longitude);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,address);
    DECLARE_FIELD(std::optional<std::string>,foursquare_id);
    DECLARE_FIELD(std::optional<std::string>,foursquare_type);
};
/// Represents the content of a contact message to be sent as the result of an inline query.
struct InputContactMessageContent {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,phone_number);
    DECLARE_FIELD(std::string,first_name);
    DECLARE_FIELD(std::optional<std::string>,last_name);
    DECLARE_FIELD(std::optional<std::string>,vcard);
};
/// Represents a result of an inline query that was chosen by the user and sent to their chat partner.
struct ChosenInlineResult {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,result_id);
    DECLARE_FIELD(std::string,query);
    DECLARE_FIELD(User,from);
    DECLARE_FIELD(std::optional<Location>,location);
    DECLARE_FIELD(std::optional<std::string>,inline_message_id);
};
using InputMessageContent = std::variant<InputTextMessageContent,InputLocationMessageContent,
InputVenueMessageContent,InputContactMessageContent>;
/// Represents a link to an article or web page.
struct InlineQueryResultArticle {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(InputMessageContent,input_message_content);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<std::string>,url);
    DECLARE_FIELD(std::optional<bool>,hide_url);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<std::string>,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_width);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_height);
};
/**
* Represents a link to a photo. By default,
* this photo will be sent by the user with optional caption.
* Alternatively, you can use input_message_content to send a message with the specified content instead of the photo.
* This object represents an incoming update.
*/
struct InlineQueryResultPhoto {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,photo_url);
    DECLARE_FIELD(std::string,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,photo_width);
    DECLARE_FIELD(std::optional<uint32_t>,photo_height);
    DECLARE_FIELD(std::optional<std::string>,time);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to a video animation (H.264/MPEG-4 AVC video without sound).
 * By default, this animated MPEG-4 file will be sent by the user with optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified content
 * instead of the animation.
 */
struct InlineQueryResultMpeg4Gif {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,mpeg4_url);
    DECLARE_FIELD(std::optional<uint32_t>,mpeg4_width);
    DECLARE_FIELD(std::optional<uint32_t>,mpeg4_height);
    DECLARE_FIELD(std::optional<uint32_t>,duration);
    DECLARE_FIELD(std::string,thumb_url);
    DECLARE_FIELD(std::optional<std::string>,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to a page containing an embedded video player or a video file.
 * By default, this video file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the video.
 */
struct InlineQueryResultVideo {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,video_url);
    DECLARE_FIELD(std::string,mime_type);
    DECLARE_FIELD(std::string,thumb_url);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<uint32_t>,video_width);
    DECLARE_FIELD(std::optional<uint32_t>,video_height);
    DECLARE_FIELD(std::optional<uint32_t>,video_duration);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to an MP3 audio file. By default, this audio file will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with the specified content
 * instead of the audio.
 */
struct InlineQueryResultAudio {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,audio_url);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<std::string>,performer);
    DECLARE_FIELD(std::optional<uint32_t>,audio_duration);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
};
/**
  Represents a link to a voice recording in an .ogg container encoded with OPUS.
  By default, this voice recording will be sent by the user.
  Alternatively, you can use input_message_content to send a message with the specified
  content instead of the the voice message.
  */
struct InlineQueryResultVoice {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,voice_url);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<uint32_t>,voice_duration);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
};
/**
  * Represents a link to a file. By default, this file will be sent by the user with an optional caption.
  * Alternatively, you can use input_message_content to send a message with the specified content instead
  * of the file. Currently, only .PDF and .ZIP files can be sent using this method.
  */
struct InlineQueryResultDocument {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::string,document_url);
    DECLARE_FIELD(std::string,mime_type);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
    DECLARE_FIELD(std::optional<std::string>,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_width);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_height);
};
/**
  * Represents a location on a map.
  * By default, the location will be sent by the user.
  * Alternatively, you can use input_message_content to send a message with the specified
  * content instead of the location.
  */
struct InlineQueryResultLocation {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(float,latitude);
    DECLARE_FIELD(float,longitude);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::optional<uint32_t>,live_period);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
    DECLARE_FIELD(std::optional<std::string>,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_width);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_height);
};
/**
 * Represents a venue.
 * By default, the venue will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with
 * the specified content instead of the venue.
 */
struct InlineQueryResultVenue {
    DECLARE_STRUCT;
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(float,latitude);
    DECLARE_FIELD(float,longitude);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,address);
    DECLARE_FIELD(std::optional<std::string>,foursquare_id);
    DECLARE_FIELD(std::optional<std::string>,fourquare_type);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
    DECLARE_FIELD(std::optional<std::string>,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_width);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_height);
};
/**
 * Represents a contact with a phone number.
 * By default, this contact will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with
 * the specified content instead of the contact.
 */
struct InlineQueryResultContact {
    DECLARE_STRUCT;
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,phone_number);
    DECLARE_FIELD(std::string,first_name);
    DECLARE_FIELD(std::optional<std::string>,last_name);
    DECLARE_FIELD(std::optional<std::string>,vcard);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputMessageContent>,input_message_content);
    DECLARE_FIELD(std::optional<std::string>,thumb_url);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_widt);
    DECLARE_FIELD(std::optional<uint32_t>,thumb_height);
};
/// Represents a Game.
struct InlineQueryResultGame {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,game_short_name);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
};
/**
 * Represents a link to a photo stored on the Telegram servers.
 * By default, this photo will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the photo
 */
struct InlineQueryResultCachedPhoto {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,photo_file_id);
    DECLARE_FIELD(std::optional<std::string>,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to an animated GIF file stored on the Telegram servers.
 * By default, this animated GIF file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with specified content
 * instead of the animation.
 */
struct InlineQueryResultCachedGif {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,gif_file_id);
    DECLARE_FIELD(std::optional<std::string>,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a video animation (H.264/MPEG-4 AVC video without sound)
 * stored on the Telegram servers. By default, this animated MPEG-4 file will be sent by the user
 * with an optional caption. Alternatively, you can use input_message_content to send a message
 * with the specified content instead of the animation.
 */
struct InlineQueryResultCachedMpeg4Gif {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,mpeg4_file_id);
    DECLARE_FIELD(std::optional<std::string>,title);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a sticker stored on the Telegram servers.
 * By default, this sticker will be sent by the user.
 * Alternatively, you can use input_message_content to send a message
 * with the specified content instead of the sticker.
 */
struct InlineQueryResultCachedSticker {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,sticker_file_id);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/// This object contains information about an incoming shipping query.
struct ShippingQuery {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(User,from);
    DECLARE_FIELD(std::string,invoice_payload);
    DECLARE_FIELD(ShippingAddress,shipping_address);
};

/**
 * Represents a link to a file stored on the Telegram servers.
 * By default, this file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the
 * specified content instead of the file.
 */
struct InlineQueryResultCachedDocument {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,document_file_id);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a video file stored on the Telegram servers.
 * By default, this video file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the video.
 */
struct InlineQueryResultCachedVideo {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,video_file_id);
    DECLARE_FIELD(std::optional<std::string>,description);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a voice message stored on the Telegram servers.
 * By default, this voice message will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the voice message
 */
struct InlineQueryResultCachedVoice {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,title);
    DECLARE_FIELD(std::string,voice_file_id);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to an MP3 audio file stored on the Telegram servers.
 * By default, this audio file will be sent by the user. Alternatively,
 * you can use input_message_content to send a message with the specified content instead of the audio.
 */
struct InlineQueryResultCachedAudio {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,type);
    DECLARE_FIELD(std::string,id);
    DECLARE_FIELD(std::string,audio_file_id);
    DECLARE_FIELD(std::optional<std::string>,caption);
    DECLARE_FIELD(std::optional<std::string>,parse_mode);
    DECLARE_FIELD(std::optional<InlineKeyboardMarkup>,reply_markup);
    DECLARE_FIELD(std::optional<InputTextMessageContent>,input_message_content);
};
/// At most one of the optional parameters can be present in any given update.
struct Update {
    DECLARE_STRUCT
    DECLARE_FIELD(int64_t,update_id);
    DECLARE_FIELD(std::optional<Message>,message);
    DECLARE_FIELD(std::optional<Message>,edited_message);
    DECLARE_FIELD(std::optional<Message>,channel_post);
    DECLARE_FIELD(std::optional<Message>,edited_channel_post);
    DECLARE_FIELD(std::optional<InlineQuery>,inline_query);
    DECLARE_FIELD(std::optional<ChosenInlineResult>,chosen_inline_result);
    DECLARE_FIELD(std::optional<CallbackQuery>,callback_query);
    DECLARE_FIELD(std::optional<ShippingQuery>,shipping_query);
    DECLARE_FIELD(std::optional<PreCheckoutQuery>,pre_checkout_query);
    DECLARE_FIELD(std::optional<Poll>,poll);
    DECLARE_FIELD(std::optional<PollAnswer>,poll_answer);
};
/// Contains information about the current status of a webhook.
struct WebhookInfo {
    DECLARE_STRUCT
    DECLARE_FIELD(bool,has_custom_certificate);
    DECLARE_FIELD(std::string,url);
    DECLARE_FIELD(uint32_t,pending_update_count);
    DECLARE_FIELD(std::optional<uint64_t>,last_error_date);
    DECLARE_FIELD(std::optional<std::string>,last_error_message);
    DECLARE_FIELD(std::optional<uint32_t>,max_connections);
    DECLARE_FIELD(std::optional<std::vector<std::string>>,allowed_updates);
};
}

