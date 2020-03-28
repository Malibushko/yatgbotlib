#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <variant>
#include "utility/utility.h"
#include "headers/json_parser.h"

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
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(std::optional<uint64_t>,file_size);
    declare_field(std::optional<std::string>,file_path);
};
using InputFile = File;
/**
 * Upon receiving a message with this object, Telegram clients will display
 *  a reply interface to the user (act as if the user has selected the bot‘s message and tapped ’Reply').
 *  This can be extremely useful if you want to create user-friendly step-by-step interfaces without
 *  having to sacrifice privacy mode.
 */
struct ForceReply {
    declare_struct
    declare_field(bool,force_reply);
    declare_field(bool,selective);
};

///This object represents a Telegram user or bot.
struct User {
    declare_struct
    declare_field(int64_t,id); ///	Unique identifier for this user or bot
    declare_field(bool,is_bot);/// True, if this user is a bot
    declare_field(std::string,first_name);   /// User‘s or bot’s first name
    declare_field(std::optional<std::string>,last_name);/// User‘s or bot’s last name
    declare_field(std::optional<std::string>,username);///. User‘s or bot’s username
    declare_field(std::optional<std::string>,language_code);///  IETF language tag of the user's language
    declare_field(std::optional<bool>,can_join_groups);///True, if the bot can be invited to groups. Returned only in getMe.
    declare_field(std::optional<bool>,can_read_all_groups_messages);///True, if privacy mode is disabled for the bot. Returned only in getMe.
    declare_field(std::optional<bool>,supports_inline_queries);///. True, if the bot supports inline queries. Returned only in getMe.
};
/// This object contains information about one member of a chat.
struct ChatMember {
    declare_struct
    declare_field(User,user);
    declare_field(std::string,status);
    declare_field(std::optional<std::string>,custom_title);
    declare_field(std::optional<uint64_t>,until_date);
    declare_field(std::optional<bool>,can_be_edited);
    declare_field(std::optional<bool>,can_post_messages);
    declare_field(std::optional<bool>,can_edit_messages);
    declare_field(std::optional<bool>,can_delete_messages);
    declare_field(std::optional<bool>,can_restrict_messages);
    declare_field(std::optional<bool>,can_promote_messages);
    declare_field(std::optional<bool>,can_change_info);
    declare_field(std::optional<bool>,can_invite_users);
    declare_field(std::optional<bool>,can_pin_messages);
    declare_field(std::optional<bool>,is_member);
    declare_field(std::optional<bool>,can_send_messages);
    declare_field(std::optional<bool>,can_send_media_messages);
    declare_field(std::optional<bool>,can_send_polls);
    declare_field(std::optional<bool>,can_send_other_messages);
    declare_field(std::optional<bool>,can_add_web_page_previews);
};


/// Describes actions that a non-administrator user is allowed to take in a chat
struct ChatPermissions {
    declare_struct
    declare_field(std::optional<bool>,can_send_messages);
    declare_field(std::optional<bool>,can_send_media_messages);
    declare_field(std::optional<bool>,can_send_polls);
    declare_field(std::optional<bool>,can_send_other_messages);
    declare_field(std::optional<bool>,can_add_web_page_previews);
    declare_field(std::optional<bool>,can_change_info);
    declare_field(std::optional<bool>,can_invite_users);
    declare_field(std::optional<bool>,can_pin_messages);
};

/// Contains information about why a request was unsuccessful.
struct ResponseParameters {
    declare_struct
    declare_field(std::optional<int64_t>,migrate_to_chat_id);
    declare_field(std::optional<int32_t>,retry_after);
};
/// Represents a photo to be sent.
struct InputMediaPhoto {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,media);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
};
/// Represents a video to be sent.
using inn_type = std::variant<InputFile,std::string>;
struct InputMediaVideo {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,media);
    // InputMediaPhoto and string
    declare_field(std::optional<std::variant<inn_type>>,thumb);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<uint32_t>,width);
    declare_field(std::optional<uint32_t>,height);
    declare_field(std::optional<uint32_t>,duration);
    declare_field(std::optional<bool>,supports_streaming);
};
///Represents an animation file (GIF or H.264/MPEG-4 AVC video without sound) to be sent.
struct InputMediaAnimation {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,media);
    declare_field(std::optional<inn_type>,thumb);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<uint32_t>,width);
    declare_field(std::optional<uint32_t>,height);
    declare_field(std::optional<uint32_t>,duration);
};
/// Represents an audio file to be treated as music to be sent.
struct InputMediaAudio {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,media);
    declare_field(std::optional<inn_type>,thumb);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<uint32_t>,duration);
    declare_field(std::optional<std::string>,performer);
};
/// Represents a general file to be sent.
struct InputMediaDocument {
    declare_struct;
    declare_field(std::string,type);
    declare_field(std::string,media);
    declare_field(std::optional<inn_type>,thumb);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
};
/// This object represents one size of a photo or a file / sticker thumbnail.
struct PhotoSize {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,width);
    declare_field(uint32_t,height);
    declare_field(std::optional<uint64_t>,photo_size);
};

/// This object represents an audio file to be treated as music by the Telegram clients.
struct Audio {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(int32_t,duration);
    declare_field(std::optional<std::string>,performer);
    declare_field(std::optional<std::string>,title);
    declare_field(std::optional<std::string>,mime_type);
    declare_field(std::optional<uint32_t>,file_size);
    declare_field(std::optional<PhotoSize>,thumb);
};

/// This object represents a chat photo.
struct ChatPhoto {
    declare_struct
    /**
     * File identifier of small (160x160) chat photo.
     * This file_id can be used only for photo
     * download and only for as long as the photo is not changed.
     */
    declare_field(std::string,small_file_id);
    /**
     * Unique file identifier of small (160x160) chat photo,
     *  which is supposed to be the same over time and for different bots.
     Can't be used to download or reuse the file.
     */
    declare_field(std::string,small_file_unique_id);
    /**
     * File identifier of big (640x640) chat photo.
     *  This file_id can be used only for photo download
     * and only for as long as the photo is not changed.
     */
    declare_field(std::string,big_file_id);
    /**
     * Unique file identifier of big (640x640) chat photo,
     *  which is supposed to be the same over time and for
     *  different bots. Can't be used to download or reuse the file.
     */
    declare_field(std::string,big_file_unique_id);
};

/// This object represents a chat.
struct Chat {
    declare_struct
    /**
     * Unique identifier for this chat.
     * This number may be greater than 32 bits and some programming languages
     * may have difficulty/silent defects in interpreting it.
     * But it is smaller than 52 bits, so a signed 64 bit integer or double-precision
     *  float type are safe for storing this identifier.
     */
    declare_field(int64_t,id);
    declare_field(std::string,type); /// 	Type of chat, can be either “private”, “group”, “supergroup” or “channel”
    declare_field(std::optional<std::string>,title); ///Title, for supergroups, channels and group chats
    declare_field(std::optional<std::string>,username);///Username, for private chats, supergroups and channels if available
    declare_field(std::optional<std::string>,first_name);///First name of the other party in a private chat
    declare_field(std::optional<std::string>,last_name);///Last name of the other party in a private chat
    declare_field(std::optional<ChatPhoto>,photo);///. Chat photo. Returned only in getChat.
    declare_field(std::optional<std::string>,description);///Description, for groups, supergroups and channel chats. Returned only in getChat.
    declare_field(std::optional<std::string>,invite_link); ///Chat invite link, for groups, supergroups and channel chats. Each administrator in a chat generates their own invite links, so the bot must first generate the link using exportChatInviteLink. Returned only in getChat.
    declare_field(std::optional<std::unique_ptr<Message>>,pinned_message); ///Pinned message, for groups, supergroups and channels. Returned only in getChat.
    declare_field(std::optional<ChatPermissions>,permissions); /// Default chat member permissions, for groups and supergroups. Returned only in getChat.
    declare_field(std::optional<int32_t>,slow_mode_delay);///For supergroups, the minimum allowed delay between consecutive messages sent by each unpriviledged user. Returned only in getChat.
    declare_field(std::optional<std::string>,sticker_set_name);///For supergroups, name of group sticker set. Returned only in getChat.
    declare_field(std::optional<bool>,can_set_sticket_get);///True, if the bot can change the group sticker set. Returned only in getChat.
};

/// This object represents one special entity in a text message. For example, hashtags, usernames, URLs, etc.
struct MessageEntity {
    declare_struct
    declare_field(std::string,type);
    declare_field(int32_t,offset);
    declare_field(int32_t,length);
    declare_field(std::optional<std::string>,url);
    declare_field(std::optional<User>,user);
    declare_field(std::optional<std::string>,language);
};
///This object represents a general file (as opposed to photos, voice messages and audio files).
struct Document {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(std::optional<PhotoSize>,thumb);
    declare_field(std::optional<std::string>,file_name);
    declare_field(std::optional<std::string>,mime_type);
    declare_field(std::optional<uint64_t>,file_size);
};
///This object represents a video file.
struct Video {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,width);
    declare_field(uint32_t,height);
    declare_field(uint32_t,duration);
    declare_field(std::optional<PhotoSize>,thumb);
    declare_field(std::optional<std::string>,mime_type);
    declare_field(std::optional<uint64_t>,file_size);
};
/// This object represents an animation file (GIF or H.264/MPEG-4 AVC video without sound)
struct Animation {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,width);
    declare_field(uint32_t,height);
    declare_field(uint32_t,duration);
    declare_field(std::optional<std::string>,file_name);
    declare_field(std::optional<PhotoSize>,thumb);
    declare_field(std::optional<std::string>,mime_type);
    declare_field(std::optional<uint64_t>,file_size);
};
/// This object represents a voice note.
struct Voice {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,duration);
    declare_field(std::optional<std::string>,mime_type);
    declare_field(std::optional<uint64_t>,file_size);
};

///This object represents a video message (available in Telegram apps as of v.4.0).
struct VideoNote {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,length);
    declare_field(uint32_t,duration);
    declare_field(std::optional<PhotoSize>,thumb);
    declare_field(std::optional<uint64_t>,file_size);
};
/// This object represents a phone contact.
struct Contact {
    declare_struct
    declare_field(std::string,phone_number);
    declare_field(std::string,first_name);
    declare_field(std::optional<std::string>,last_name);
    declare_field(std::optional<int64_t>,user_id);
    declare_field(std::optional<std::string>,vcard);
};

/// This object represents a point on the map.
struct Location {
    declare_struct
    declare_field(float,longitude);
    declare_field(float,latitude);
};
/// This object represents a venue.
struct Venue {
    declare_struct
    declare_field(Location,location);
    declare_field(std::string,title);
    declare_field(std::string,address);
    declare_field(std::optional<std::string>,foursquare_id);
    declare_field(std::optional<std::string>,foursquare_type);
};

/// This object contains information about one answer option in a poll.
struct PollOption {
    declare_struct
    declare_field(std::string,text);
    declare_field(int32_t,voter_count);
};
/// This object represents an answer of a user in a non-anonymous poll.
struct PollAnswer {
    declare_struct
    declare_field(std::string,poll_id);
    declare_field(User,user);
    declare_field(std::vector<int>,option_ids);
};

/// This object contains information about a poll.
struct Poll {
    declare_struct
    declare_field(std::string,id);
    declare_field(std::string,question);
    declare_field(std::vector<PollOption>,options);
    declare_field(uint32_t,total_voter_count);
    declare_field(bool,is_closed);
    declare_field(bool,is_anonymous);
    declare_field(std::string,type);
    declare_field(bool,allow_multiple_answers);
    declare_field(std::optional<int16_t>,correct_option_id);
};
/// This object represent a user's profile pictures.
struct UserProfilePhotos {
    declare_struct
    declare_field(int32_t,total_count);
    declare_field(std::vector<std::vector<PhotoSize>>,photos);
};

///This object represents a custom keyboard with reply options
struct ReplyKeyboardMarkup {
    declare_struct
    declare_field(std::vector<std::vector<KeyboardButton>>,keyboard);
    declare_field(std::optional<bool>,resize_keyboard);
    declare_field(std::optional<bool>,one_time_keyboard);
    declare_field(std::optional<bool>,selective);
};
/// This object represents type of a poll, which is allowed to be created
///  and sent when the corresponding button is pressed
struct KeyboardButtonPollType {
    declare_struct
    declare_field(std::string,type);
};

/**
 * This object represents one button of the reply keyboard.
 * For simple text buttons String can be used instead of this object to specify text of the button.
 * Optional fields request_contact, request_location, and request_poll are mutually exclusive.
 */
struct KeyboardButton {
    declare_struct
    declare_field(std::string,text);
    declare_field(std::optional<bool>,request_contact);
    declare_field(std::optional<bool>,request_location);
    declare_field(std::optional<KeyboardButtonPollType>,request_poll);
};

/**
 * Upon receiving a message with this object,
 * Telegram clients will remove the current custom keyboard and display the default
 * letter-keyboard. By default, custom keyboards are displayed until a new keyboard
 * is sent by a bot. An exception is made for one-time keyboards that are hidden
 * immediately after the user presses a button
*/
struct ReplyKeyboardRemove {
    declare_struct
    declare_field(bool,remove_keyboard);
    declare_field(bool,selective);
};

/// This object represents an inline keyboard that appears right next to the message it belongs to.
struct InlineKeyboardMarkup {
    declare_struct
    declare_field(std::vector<std::vector<InlineKeyboardButton>>,inline_keyboard);
};

/**
 * This object represents a parameter of the inline keyboard button
 * used to automatically authorize a user. Serves as a great replacement
 * for the Telegram Login Widget when the user is coming from Telegram.
 */
struct LoginUrl {
    declare_struct
    declare_field(std::string,url);
    declare_field(std::optional<std::string>,forward_text);
    declare_field(std::optional<std::string>,bot_username);
    declare_field(std::optional<bool>,request_write_access);
};
/// This object represents one row of the high scores table for a game.
struct GameHighScore {
    declare_struct
    declare_field(uint32_t,position);
    declare_field(User,user);
    declare_field(uint64_t,score);
};

/// This object represents one button of an inline keyboard. You must use exactly one of the optional fields.
struct InlineKeyboardButton {
    declare_struct
    declare_field(std::string,text);
    declare_field(std::optional<std::string>,url);
    declare_field(std::optional<LoginUrl>,login_url);
    declare_field(std::optional<std::string>,callback_data);
    declare_field(std::optional<std::string>,switch_inline_query);
    declare_field(std::optional<std::string>,switch_inline_query_current_chat);
    declare_field(std::optional<CallbackGame>,callback_game);
    declare_field(std::optional<bool>,pay);
};
/// This object represents a game.
/// Use BotFather to create and edit games, their short names will act as unique identifiers.

struct Game {
    declare_struct
    declare_field(std::string,title);
    declare_field(std::string,description);
    declare_field(std::vector<PhotoSize>,photo);
    declare_field(std::optional<std::string>,text);
    declare_field(std::optional<std::vector<MessageEntity>>,text_entities);
    declare_field(std::optional<Animation>,animation);
};

/// This object contains basic information about an invoice.

struct Invoice {
    declare_struct
    declare_field(uint32_t,total_amount);
    declare_field(std::string,title);
    declare_field(std::string,description);
    declare_field(std::string,start_parameter);
    declare_field(std::string,currency);
};
/// This object describes the position on faces where a mask should be placed by default.

struct MaskPosition {
    declare_struct
    declare_field(std::string,point);
    declare_field(float,x_shift);
    declare_field(float,y_shift);
    declare_field(float,scale);
};
/// This object represents a sticker.

struct Sticker {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint32_t,width);
    declare_field(uint32_t,height);
    declare_field(bool,is_animated);
    declare_field(std::optional<PhotoSize>,thumb);
    declare_field(std::optional<std::string>,emoji);
    declare_field(std::optional<std::string>,set_name);
    declare_field(std::optional<MaskPosition>,mask_position);
    declare_field(std::optional<uint32_t>,file_size);
};
/// This object represents a sticker set.
struct StickerSet {
    declare_struct
    declare_field(std::string,name);
    declare_field(std::string,title);
    declare_field(bool,is_animated);
    declare_field(bool,contains_masks);
    declare_field(std::vector<Sticker>,stickers);
};
/// This object represents a portion of the price for goods or services.
struct LabeledPrice {
    declare_struct
    declare_field(std::string,label);
    declare_field(uint64_t,amount);
};
/// This object represents one shipping option.
struct ShippingOption {
    declare_struct;
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(std::vector<LabeledPrice>,prices);
};

/// This object represents a shipping address.
struct ShippingAddress {
    declare_struct
    declare_field(std::string,country_code);    // NOLINT
    declare_field(std::string,state);           // NOLINT
    declare_field(std::string,city);            // NOLINT
    declare_field(std::string,street_line1);    // NOLINT
    declare_field(std::string,street_line2);    // NOLINT
    declare_field(std::string,post_code);       // NOLINT
};
/// This object represents information about an order.
struct OrderInfo {
    declare_struct
    declare_field(std::optional<std::string>,name);
    declare_field(std::optional<std::string>,phone_number);
    declare_field(std::optional<std::string>,email);
    declare_field(std::optional<ShippingAddress>,shipping_address);
};

/// This object contains basic information about a successful payment.
struct SuccessfulPayment {
    declare_struct
    declare_field(std::string,currency);
    declare_field(int32_t,total_amount);
    declare_field(std::string,invoice_payload);
    declare_field(std::optional<std::string>,shipping_option_id);
    declare_field(std::optional<OrderInfo>,order_info);
    declare_field(std::string,telegram_payment_charge_id);
    declare_field(std::string,provider_payment_charge_id);
};
/// This object contains information about an incoming pre-checkout query.
struct PreCheckoutQuery {
    declare_struct
    declare_field(std::string,id);
    declare_field(User,from);
    declare_field(std::string,currency);
    declare_field(uint32_t,total_amount);
    declare_field(std::string,invoice_payload);
    declare_field(std::optional<std::string>,shipping_option_id);
    declare_field(std::optional<OrderInfo>,order_info);
};
/// This object represents a file uploaded to Telegram Passport.
/// Currently all Telegram Passport files are in JPEG format when decrypted and don't exceed 10MB.
struct PassportFile {
    declare_struct
    declare_field(std::string,file_id);
    declare_field(std::string,file_unique_id);
    declare_field(uint64_t,file_size);
    declare_field(uint64_t,file_date);
};

/// Contains information about documents or other Telegram Passport elements shared with the bot by the user.

struct EncryptedPassportElement {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,hash);
    declare_field(std::optional<std::string>,data);
    declare_field(std::optional<std::string>,phone_number);
    declare_field(std::optional<std::string>,email);
    declare_field(std::optional<std::vector<PassportFile>>,files);
    declare_field(std::optional<PassportFile>,front_side);
    declare_field(std::optional<PassportFile>,reverse_side);
    declare_field(std::optional<PassportFile>,selfie);
    declare_field(std::optional<std::vector<PassportFile>>,translation);
};

/** Contains data required for decrypting and authenticating EncryptedPassportElement.
  * See the Telegram Passport Documentation for a complete description of the data decryption and
  * authentication processes.
  */
struct EncryptedCredentials {
    declare_struct
    declare_field(std::string,data);
    declare_field(std::string,hash);
    declare_field(std::string,secret);
};


/// Contains information about Telegram Passport data shared with the bot by the user.
struct PassportData {
    declare_struct
    declare_field(std::vector<EncryptedPassportElement>,data);
    declare_field(EncryptedCredentials,credentials);
};
/// Represents an issue in one of the data fields that was provided by the user.
/// The error is considered resolved when the field's value changes.
struct PassportElementErrorDataField {
    declare_struct
    declare_field(const std::string_view,source{"data"});
    declare_field(std::string,type);
    declare_field(std::string,field_name);
    declare_field(std::string,data_hash);
    declare_field(std::string,message);
};
/// Represents an issue with the front side of a document.
/// The error is considered resolved when the file with the front side of the document changes.
struct PassportElementErrorFrontSide {
    declare_struct
    declare_field(const std::string_view,source {"front_side"});
    declare_field(std::string,type);
    declare_field(std::string,file_hash);
    declare_field(std::string,message);
};
///Represents an issue with the reverse side of a document.
/// The error is considered resolved when the file with reverse side of the document changes.
struct PassportElementErrorReverseSide {
    declare_struct
    declare_field(const std::string_view,source {"reverse_side"});
    declare_field(std::string,type);
    declare_field(std::string,file_hash);
    declare_field(std::string,message);
};
/// Represents an issue with the selfie with a document.
/// The error is considered resolved when the file with the selfie changes
struct PassportElementErrorSelfie {
    declare_struct
    declare_field(const std::string_view,source {"selfie"});
    declare_field(std::string,type);
    declare_field(std::string,file_hash);
    declare_field(std::string,message);
};
/// Represents an issue with a document scan.
/// The error is considered resolved when the file with the document scan changes.
struct PassportElementErrorFile {
    declare_struct
    declare_field(std::string,source {"selfie"});
    declare_field(std::string,type);
    declare_field(std::string,file_hash);
    declare_field(std::string,message);
};
/// Represents an issue with a list of scans.
/// The error is considered resolved when the list of files containing the scans changes.
struct PassportElementErrorFiles {
    declare_struct;
    declare_field(const std::string_view,source {"files"});
    declare_field(std::string,type);
    declare_field(std::vector<std::string>,file_hashes);
    declare_field(std::string,message);
};
/// Represents an issue with one of the files that constitute the translation of a document.
/// The error is considered resolved when the file changes.
struct PassportElementErrorTranslationFile {
    declare_struct
    declare_field(const std::string_view,source {"translation_file"});
    declare_field(std::string,type);
    declare_field(std::string,file_hashe);
    declare_field(std::string,message);
};
struct PassportElementErrorTranslationFiles {
    declare_struct
    declare_field(const std::string_view,source {"translation_files"});
    declare_field(std::string,type);
    declare_field(std::vector<std::string>,file_hashes);
    declare_field(std::string,message);
};

/// Represents an issue in an unspecified place.
/// The error is considered resolved when new data is added.
struct PassportElementErrorUnspecified {
    declare_struct
    declare_field(const std::string_view,source {"unspecified"});
    declare_field(std::string,type);
    declare_field(std::vector<std::string>,file_hashes);
    declare_field(std::string,message);
};

/// This object represents a message.
struct Message {
    declare_struct
    declare_field(int64_t,message_id);
    declare_field(std::optional<User>,from);
    declare_field(uint64_t,date);
    declare_field(Chat,chat);
    declare_field(std::optional<User>,forward_from);
    declare_field(std::optional<User>,forward_from_chat);
    declare_field(std::optional<int64_t>,forward_from_message_id);
    declare_field(std::optional<std::string>,forward_signature);
    declare_field(std::optional<std::string>,forward_sender_name);
    declare_field(std::optional<uint64_t>,forward_date);
    declare_field(std::optional<std::unique_ptr<Message>>,reply_to_message);
    declare_field(std::optional<uint64_t>,edit_date);
    declare_field(std::optional<std::string>,media_group_id);
    declare_field(std::optional<std::string>,author_signature);
    declare_field(std::optional<std::string>,text);
    declare_field(std::optional<std::vector<MessageEntity>>,entities);
    declare_field(std::optional<std::vector<MessageEntity>>,caption_entities);
    declare_field(std::optional<Audio>,audio);
    declare_field(std::optional<Document>,document);
    declare_field(std::optional<Animation>,animation);
    declare_field(std::optional<Game>,game);
    declare_field(std::optional<std::vector<PhotoSize>>,photo);
    declare_field(std::optional<Sticker>,sticker);
    declare_field(std::optional<Video>,video);
    declare_field(std::optional<Voice>,voice);
    declare_field(std::optional<VideoNote>,video_note);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<Contact>,contact);
    declare_field(std::optional<Location>,location);
    declare_field(std::optional<Venue>,venue);
    declare_field(std::optional<Poll>,poll);
    declare_field(std::optional<std::vector<User>>,new_chat_members);
    declare_field(std::optional<User>,left_chat_member);
    declare_field(std::optional<std::string>,new_chat_title);
    declare_field(std::optional<std::vector<PhotoSize>>,new_chat_photo);
    declare_field(std::optional<bool>,delete_chat_photo);
    declare_field(std::optional<bool>,group_chat_created);
    declare_field(std::optional<bool>,supergroup_chat_created);
    declare_field(std::optional<bool>,channel_chat_created);
    declare_field(std::optional<int64_t>,migrate_to_chat_id);
    declare_field(std::optional<int64_t>,migrate_from_chat_id);
    declare_field(std::optional<std::unique_ptr<Message>>,pinned_message);
    declare_field(std::optional<Invoice>,invoice);
    declare_field(std::optional<SuccessfulPayment>,successfull_payment);
    declare_field(std::optional<std::string>,connected_website);
    declare_field(std::optional<PassportData>,passport_data);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
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
    declare_struct
    declare_field(std::string,id);
    declare_field(User,from);
    declare_field(std::optional<Message>,message);
    declare_field(std::optional<std::string>,inline_message_id);
    declare_field(std::optional<std::string>,chat_instance);
    declare_field(std::optional<std::string>,data);
    declare_field(std::optional<std::string>,game_short_name);
};
/// This object represents an incoming inline query.
/// When the user sends an empty query, your bot could return some default or trending results.
struct InlineQuery {
    declare_struct
    declare_field(std::string,id);
    declare_field(User,from);
    declare_field(std::optional<Location>,location);
    declare_field(std::string,query);
    declare_field(std::string,offset);
};

/// Represents the content of a text message to be sent as the result of an inline query.
struct InputTextMessageContent {
    declare_struct
    declare_field(std::string,message_text);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<bool>,disable_web_page_preview);
};
/// Represents the content of a location message to be sent as the result of an inline query.
struct InputLocationMessageContent{
    declare_struct
    declare_field(float,latitude);
    declare_field(float,longitude);
    declare_field(std::optional<uint32_t>,live_period);
};
/// Represents the content of a venue message to be sent as the result of an inline query.
struct InputVenueMessageContent {
    declare_struct
    declare_field(float,latitude);
    declare_field(float,longitude);
    declare_field(std::string,title);
    declare_field(std::string,address);
    declare_field(std::optional<std::string>,foursquare_id);
    declare_field(std::optional<std::string>,foursquare_type);
};
/// Represents the content of a contact message to be sent as the result of an inline query.
struct InputContactMessageContent {
    declare_struct
    declare_field(std::string,phone_number);
    declare_field(std::string,first_name);
    declare_field(std::optional<std::string>,last_name);
    declare_field(std::optional<std::string>,vcard);
};
/// Represents a result of an inline query that was chosen by the user and sent to their chat partner.
struct ChosenInlineResult {
    declare_struct
    declare_field(std::string,result_id);
    declare_field(std::string,query);
    declare_field(User,from);
    declare_field(std::optional<Location>,location);
    declare_field(std::optional<std::string>,inline_message_id);
};
using InputMessageContent = std::variant<InputTextMessageContent,InputLocationMessageContent,
InputVenueMessageContent,InputContactMessageContent>;
/// Represents a link to an article or web page.
struct InlineQueryResultArticle {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(InputMessageContent,input_message_content);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<std::string>,url);
    declare_field(std::optional<bool>,hide_url);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<std::string>,thumb_url);
    declare_field(std::optional<uint32_t>,thumb_width);
    declare_field(std::optional<uint32_t>,thumb_height);
};
/**
* Represents a link to a photo. By default,
* this photo will be sent by the user with optional caption.
* Alternatively, you can use input_message_content to send a message with the specified content instead of the photo.
* This object represents an incoming update.
*/
struct InlineQueryResultPhoto {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,photo_url);
    declare_field(std::string,thumb_url);
    declare_field(std::optional<uint32_t>,photo_width);
    declare_field(std::optional<uint32_t>,photo_height);
    declare_field(std::optional<std::string>,time);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to a video animation (H.264/MPEG-4 AVC video without sound).
 * By default, this animated MPEG-4 file will be sent by the user with optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified content
 * instead of the animation.
 */
struct InlineQueryResultMpeg4Gif {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,mpeg4_url);
    declare_field(std::optional<uint32_t>,mpeg4_width);
    declare_field(std::optional<uint32_t>,mpeg4_height);
    declare_field(std::optional<uint32_t>,duration);
    declare_field(std::string,thumb_url);
    declare_field(std::optional<std::string>,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to a page containing an embedded video player or a video file.
 * By default, this video file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the video.
 */
struct InlineQueryResultVideo {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,video_url);
    declare_field(std::string,mime_type);
    declare_field(std::string,thumb_url);
    declare_field(std::string,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<uint32_t>,video_width);
    declare_field(std::optional<uint32_t>,video_height);
    declare_field(std::optional<uint32_t>,video_duration);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
};
/**
 * Represents a link to an MP3 audio file. By default, this audio file will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with the specified content
 * instead of the audio.
 */
struct InlineQueryResultAudio {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,audio_url);
    declare_field(std::string,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<std::string>,performer);
    declare_field(std::optional<uint32_t>,audio_duration);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
};
/**
  Represents a link to a voice recording in an .ogg container encoded with OPUS.
  By default, this voice recording will be sent by the user.
  Alternatively, you can use input_message_content to send a message with the specified
  content instead of the the voice message.
  */
struct InlineQueryResultVoice {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,voice_url);
    declare_field(std::string,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<uint32_t>,voice_duration);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
};
/**
  * Represents a link to a file. By default, this file will be sent by the user with an optional caption.
  * Alternatively, you can use input_message_content to send a message with the specified content instead
  * of the file. Currently, only .PDF and .ZIP files can be sent using this method.
  */
struct InlineQueryResultDocument {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::string,document_url);
    declare_field(std::string,mime_type);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
    declare_field(std::optional<std::string>,thumb_url);
    declare_field(std::optional<uint32_t>,thumb_width);
    declare_field(std::optional<uint32_t>,thumb_height);
};
/**
  * Represents a location on a map.
  * By default, the location will be sent by the user.
  * Alternatively, you can use input_message_content to send a message with the specified
  * content instead of the location.
  */
struct InlineQueryResultLocation {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(float,latitude);
    declare_field(float,longitude);
    declare_field(std::string,title);
    declare_field(std::optional<uint32_t>,live_period);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
    declare_field(std::optional<std::string>,thumb_url);
    declare_field(std::optional<uint32_t>,thumb_width);
    declare_field(std::optional<uint32_t>,thumb_height);
};
/**
 * Represents a venue.
 * By default, the venue will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with
 * the specified content instead of the venue.
 */
struct InlineQueryResultVenue {
    declare_struct;
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(float,latitude);
    declare_field(float,longitude);
    declare_field(std::string,title);
    declare_field(std::string,address);
    declare_field(std::optional<std::string>,foursquare_id);
    declare_field(std::optional<std::string>,fourquare_type);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
    declare_field(std::optional<std::string>,thumb_url);
    declare_field(std::optional<uint32_t>,thumb_width);
    declare_field(std::optional<uint32_t>,thumb_height);
};
/**
 * Represents a contact with a phone number.
 * By default, this contact will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with
 * the specified content instead of the contact.
 */
struct InlineQueryResultContact {
    declare_struct;
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,phone_number);
    declare_field(std::string,first_name);
    declare_field(std::optional<std::string>,last_name);
    declare_field(std::optional<std::string>,vcard);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputMessageContent>,input_message_content);
    declare_field(std::optional<std::string>,thumb_url);
    declare_field(std::optional<uint32_t>,thumb_widt);
    declare_field(std::optional<uint32_t>,thumb_height);
};
/// Represents a Game.
struct InlineQueryResultGame {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,game_short_name);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
};
/**
 * Represents a link to a photo stored on the Telegram servers.
 * By default, this photo will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the photo
 */
struct InlineQueryResultCachedPhoto {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,photo_file_id);
    declare_field(std::optional<std::string>,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to an animated GIF file stored on the Telegram servers.
 * By default, this animated GIF file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with specified content
 * instead of the animation.
 */
struct InlineQueryResultCachedGif {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,gif_file_id);
    declare_field(std::optional<std::string>,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a video animation (H.264/MPEG-4 AVC video without sound)
 * stored on the Telegram servers. By default, this animated MPEG-4 file will be sent by the user
 * with an optional caption. Alternatively, you can use input_message_content to send a message
 * with the specified content instead of the animation.
 */
struct InlineQueryResultCachedMpeg4Gif {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,mpeg4_file_id);
    declare_field(std::optional<std::string>,title);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a sticker stored on the Telegram servers.
 * By default, this sticker will be sent by the user.
 * Alternatively, you can use input_message_content to send a message
 * with the specified content instead of the sticker.
 */
struct InlineQueryResultCachedSticker {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,sticker_file_id);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/// This object contains information about an incoming shipping query.
struct ShippingQuery {
    declare_struct
    declare_field(std::string,id);
    declare_field(User,from);
    declare_field(std::string,invoice_payload);
    declare_field(ShippingAddress,shipping_address);
};

/**
 * Represents a link to a file stored on the Telegram servers.
 * By default, this file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the
 * specified content instead of the file.
 */
struct InlineQueryResultCachedDocument {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(std::string,document_file_id);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a video file stored on the Telegram servers.
 * By default, this video file will be sent by the user with an optional caption.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the video.
 */
struct InlineQueryResultCachedVideo {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(std::string,video_file_id);
    declare_field(std::optional<std::string>,description);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to a voice message stored on the Telegram servers.
 * By default, this voice message will be sent by the user.
 * Alternatively, you can use input_message_content to send a message with the specified
 * content instead of the voice message
 */
struct InlineQueryResultCachedVoice {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,title);
    declare_field(std::string,voice_file_id);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/**
 * Represents a link to an MP3 audio file stored on the Telegram servers.
 * By default, this audio file will be sent by the user. Alternatively,
 * you can use input_message_content to send a message with the specified content instead of the audio.
 */
struct InlineQueryResultCachedAudio {
    declare_struct
    declare_field(std::string,type);
    declare_field(std::string,id);
    declare_field(std::string,audio_file_id);
    declare_field(std::optional<std::string>,caption);
    declare_field(std::optional<std::string>,parse_mode);
    declare_field(std::optional<InlineKeyboardMarkup>,reply_markup);
    declare_field(std::optional<InputTextMessageContent>,input_message_content);
};
/// At most one of the optional parameters can be present in any given update.
struct Update {
    declare_struct
    declare_field(int64_t,update_id);
    declare_field(std::optional<Message>,message);
    declare_field(std::optional<Message>,edited_message);
    declare_field(std::optional<Message>,channel_post);
    declare_field(std::optional<Message>,edited_channel_post);
    declare_field(std::optional<InlineQuery>,inline_query);
    declare_field(std::optional<ChosenInlineResult>,chosen_inline_result);
    declare_field(std::optional<CallbackQuery>,callback_query);
    declare_field(std::optional<ShippingQuery>,shipping_query);
    declare_field(std::optional<PreCheckoutQuery>,pre_checkout_query);
    declare_field(std::optional<Poll>,poll);
    declare_field(std::optional<PollAnswer>,poll_answer);
};
/// Contains information about the current status of a webhook.
struct WebhookInfo {
    declare_struct
    declare_field(bool,has_custom_certificate);
    declare_field(std::string,url);
    declare_field(uint32_t,pending_update_count);
    declare_field(std::optional<uint64_t>,last_error_date);
    declare_field(std::optional<std::string>,last_error_message);
    declare_field(std::optional<uint32_t>,max_connections);
    declare_field(std::optional<std::vector<std::string>>,allowed_updates);
};
}

