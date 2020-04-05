#include "telegramtructs.h"

///This object represents a Telegram user or bot.
struct User {
  int64_t id; ///	Unique identifier for this user or bot
  bool is_bot;/// True, if this user is a bot
  std::string first_name;   /// User‘s or bot’s first name
  std::optional<std::string> last_name;/// User‘s or bot’s last name
  std::optional<std::string> username;///. User‘s or bot’s username
  std::optional<std::string> language_code;///  IETF language tag of the user's language
  std::optional<bool>       can_join_groups;///True, if the bot can be invited to groups. Returned only in getMe.
  std::optional<bool>       can_read_all_groups_messages;///True, if privacy mode is disabled for the bot. Returned only in getMe.
  std::optional<bool>       supports_inline_queries;///. True, if the bot supports inline queries. Returned only in getMe.
};
/// This object represents a chat photo.
struct ChatPhoto {
    /**
     * File identifier of small (160x160) chat photo.
     * This file_id can be used only for photo
     * download and only for as long as the photo is not changed.
     */
    std::string small_file_id;
    /**
     * Unique file identifier of small (160x160) chat photo,
     *  which is supposed to be the same over time and for different bots.
     * Can't be used to download or reuse the file.
     */
    std::string small_file_unique_id;
    /**
     * File identifier of big (640x640) chat photo.
     *  This file_id can be used only for photo download
     * and only for as long as the photo is not changed.
     */
    std::string big_file_id;
    /**
     * Unique file identifier of big (640x640) chat photo,
     *  which is supposed to be the same over time and for
     *  different bots. Can't be used to download or reuse the file.
     */
    std::string big_file_unique_id;
};

/// This object represents a chat.
struct Chat {
    /**
     * Unique identifier for this chat.
     * This number may be greater than 32 bits and some programming languages
     * may have difficulty/silent defects in interpreting it.
     * But it is smaller than 52 bits, so a signed 64 bit integer or double-precision
     *  float type are safe for storing this identifier.
     */
    int64_t id;
    std::string type; /// 	Type of chat, can be either “private”, “group”, “supergroup” or “channel”
    std::optional<std::string> title; ///Title, for supergroups, channels and group chats
    std::optional<std::string> username;///Username, for private chats, supergroups and channels if available
    std::optional<std::string> first_name;///First name of the other party in a private chat
    std::optional<std::string> last_name;///Last name of the other party in a private chat
    std::optional<ChatPhoto> photo;///. Chat photo. Returned only in getChat.
    std::optional<std::string> description;///Description, for groups, supergroups and channel chats. Returned only in getChat.
    std::optional<std::string> invite_link; ///Chat invite link, for groups, supergroups and channel chats. Each administrator in a chat generates their own invite links, so the bot must first generate the link using exportChatInviteLink. Returned only in getChat.
    std::optional<Message> pinned_message; ///Pinned message, for groups, supergroups and channels. Returned only in getChat.
    std::optional<ChatPermissions> permissions; /// Default chat member permissions, for groups and supergroups. Returned only in getChat.
    std::optional<int32_t> slow_mode_delay;///For supergroups, the minimum allowed delay between consecutive messages sent by each unpriviledged user. Returned only in getChat.
    std::optional<std::string> sticker_set_name;///For supergroups, name of group sticker set. Returned only in getChat.
    std::optional<bool> can_set_sticket_get;///True, if the bot can change the group sticker set. Returned only in getChat.
};
///This object represents a message.
struct Message {
    int64_t message_id;
    std::optional<User> from;
    uint64_t date;
    Chat chat;
    std::optional<User> forward_from;
    std::optional<User> forward_from_chat;
    std::optional<int64_t> forward_from_message_id;
    std::optional<std::string> forward_signature;
    std::optional<std::string> forward_sender_name;
    std::optional<uint64_t> forward_date;
    std::optional<Message> reply_to_message;
    std::optional<uint64_t> edit_date;
    std::optional<std::string> media_group_id;
    std::optional<std::string> author_signature;
    std::optional<std::string> text;
    std::optional<std::vector<MessageEntity>> entities;
    std::optional<std::vector<MessageEntity>> caption_entities;
    std::optional<Audio> audio;
    std::optional<Document> document;
    std::optional<Animation> animation;
    std::optional<Game> game;
    std::optional<std::vector<PhotoSize>> photo;
    std::optional<Sticker> sticker;
    std::optional<Video> video;
    std::optional<Voice> voice;
    std::optional<VideoNote> video_note;
    std::optional<std::string> caption;
    std::optional<Contact> contact;
    std::optional<Location> location;
    std::optional<Venue> venue;
    std::optional<Poll> poll;
    std::optional<std::vector<User>> new_chat_members;
    std::optional<User> left_chat_member;
    std::optional<std::string> new_chat_title;
    std::optional<std::vector<PhotoSize>> new_chat_photo;
    std::optional<bool> delete_chat_photo;
    std::optional<bool> group_chat_created;
    std::optional<bool> supergroup_chat_created;
    std::optional<bool> channel_chat_created;
    std::optional<int64_t> migrate_to_chat_id;
    std::optional<int64_t> migrate_from_chat_id;
    std::optional<Message> pinned_message;
    std::optional<Invoice> invoice;
    std::optional<SuccessfulPayment> successfull_payment;
    std::optional<std::string> connected_website;
    std::optional<PassportData> passport_data;
    std::optional<InlineKeyboardMarkup> reply_markup;
};
/// This object represents one special entity in a text message. For example, hashtags, usernames, URLs, etc.
struct MessageEntity {
    std::string type;
    int32_t offset;
    int32_t length;
    std::optional<std::string> url;
    std::optional<User> user;
    std::optional<std::string> language;
};
/// This object represents one size of a photo or a file / sticker thumbnail.
struct PhotoSize {
    std::string file_id;
    std::string file_unique_id;
    uint32_t width;
    uint32_t height;
    std::optional<uint64_t> photo_size;
};

/// This object represents an audio file to be treated as music by the Telegram clients.
struct Audio {
    std::string file_id;
    std::string file_unique_id;
    int32_t duration;
    std::optional<std::string> performer;
    std::optional<std::string> title;
    std::optional<std::string> mime_type;
    std::optional<uint32_t> file_size;
    std::optional<PhotoSize> thumb;
};

///This object represents a general file (as opposed to photos, voice messages and audio files).
struct Document {
    std::string file_id;
    std::string file_unique_id;
    std::optional<PhotoSize> thumb;
    std::optional<std::string> file_name;
    std::optional<std::string> mime_type;
    std::optional<uint64_t> file_size;
};
///This object represents a video file.
struct Video {
    std::string file_id;
    std::string file_unique_id;
    uint32_t width;
    uint32_t height;
    uint32_t duration;
    std::optional<PhotoSize> thumb;
    std::optional<std::string> mime_type;
    std::optional<uint64_t> file_size;
};
/// This object represents an animation file (GIF or H.264/MPEG-4 AVC video without sound)
struct Animation {
    std::string file_id;
    std::string file_unique_id;
    uint32_t width;
    uint32_t height;
    uint32_t duration;
    std::optional<std::string> file_name;
    std::optional<PhotoSize> thumb;
    std::optional<std::string> mime_type;
    std::optional<uint64_t> file_size;
};
/// This object represents a voice note.
struct Voice {
    std::string file_id;
    std::string file_unique_id;
    uint32_t duration;
    std::optional<std::string> mime_type;
    std::optional<uint64_t> file_size;
};

///This object represents a video message (available in Telegram apps as of v.4.0).
struct VideoNote {
    std::string file_id;
    std::string file_unique_id;
    uint32_t length;
    uint32_t duration;
    std::optional<PhotoSize> thumb;
    std::optional<uint64_t> file_size;
};
/// This object represents a phone contact.
struct Contact {
    std::string phone_number;
    std::string first_name;
    std::optional<std::string> last_name;
    std::optional<int64_t> user_id;
    std::optional<std::string> vcard;
};

/// This object represents a point on the map.
struct Location {
    float longitude;
    float latitude;
};
/// This object represents a venue.
struct Venue {
    Location location;
    std::string title;
    std::string address;
    std::optional<std::string> foursquare_id;
    std::optional<std::string> foursquare_type;
};

/// This object contains information about one answer option in a poll.
struct PollOption {
    std::string text;
    int32_t voter_count;
};
/// This object represents an answer of a user in a non-anonymous poll.
struct PollAnswer {
    std::string poll_id;
    User user;
    std::vector<int> option_ids;
};
/// This object contains information about a poll.
struct Poll {
    std::string id;
    std::string question;
    std::vector<PollOption> options;
    uint32_t total_voter_count;
    bool is_closed;
    bool is_anonymous;
    std::string type;
    bool allow_multiple_answers;
    std::optional<int16_t> correct_option_id;
};
/// This object represent a user's profile pictures.
struct UserProfilePhotos {
    int32_t total_count;
    std::vector<PhotoSize> photos;
};
/**
  This object represents a file ready to be downloaded.
 The file can be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>.
 It is guaranteed that the link will be valid for at least 1 hour.
 When the link expires, a new one can be requested by calling getFile.
 Maximum file size to download is 20 MB
*/
struct File {
    std::string file_id;
    std::string file_unique_id;
    std::optional<uint64_t> file_size;
    std::optional<std::string> file_path;
};
///This object represents a custom keyboard with reply options
struct ReplyKeyboardMarkup {
    std::vector<std::vector<KeyboardButton>> keyboard;
    std::optional<bool> resize_keyboard;
    std::optional<bool> one_time_keyboard;
    std::optional<bool> selective;
};
/// This object represents type of a poll, which is allowed to be created
///  and sent when the corresponding button is pressed
struct KeyboardButtonPollType {
    std::string type;
};


/**
 * This object represents one button of the reply keyboard.
 * For simple text buttons String can be used instead of this object to specify text of the button.
 * Optional fields request_contact, request_location, and request_poll are mutually exclusive.
 */
struct KeyboardButton {
    std::string text;
    std::optional<bool> request_contact;
    std::optional<bool> request_location;
    std::optional<KeyboardButtonPollType> request_poll;
};

/**
 * Upon receiving a message with this object,
 * Telegram clients will remove the current custom keyboard and display the default
 * letter-keyboard. By default, custom keyboards are displayed until a new keyboard
 * is sent by a bot. An exception is made for one-time keyboards that are hidden
 * immediately after the user presses a button
*/
struct ReplyKeyboardRemove {
    bool remove_keyboard;
    bool selective;
};

/// This object represents an inline keyboard that appears right next to the message it belongs to.
struct InlineKeyboardMarkup {
    std::vector<std::vector<InlineKeyboardButton>> inline_keyboard;
};
/// This object represents one button of an inline keyboard. You must use exactly one of the optional fields.
struct InlineKeyboardButton {
    std::string text;
    std::optional<std::string> url;
    std::optional<LoginUrl> login_url;
    std::optional<std::string> callback_data;
    std::optional<std::string> switch_inline_query;
    std::optional<std::string> switch_inline_query_current_chat;
    std::optional<CallbackGame> callback_game;
    std::optional<bool> pay;
};
/**
 * This object represents a parameter of the inline keyboard button
 * used to automatically authorize a user. Serves as a great replacement
 * for the Telegram Login Widget when the user is coming from Telegram.
 */
struct LoginUrl {
    std::string url;
    std::optional<std::string> forward_text;
    std::optional<std::string> bot_username;
    std::optional<bool> request_write_access;
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
    std::string id;
    User from;
    std::optional<Message> message;
    std::optional<std::string> inline_message_id;
    std::optional<std::string> chat_instance;
    std::optional<std::string> data;
    std::optional<std::string> game_short_name;
};
/**
 * Upon receiving a message with this object, Telegram clients will display
 *  a reply interface to the user (act as if the user has selected the bot‘s message and tapped ’Reply').
 *  This can be extremely useful if you want to create user-friendly step-by-step interfaces without
 *  having to sacrifice privacy mode.
 */
struct ForceReply {
    bool force_reply;
    bool selective;
};
/// This object contains information about one member of a chat.
struct ChatMember {
    User user;
    std::string status;
    std::optional<std::string> custom_title;
    std::optional<uint64_t> until_date;
    std::optional<bool> can_be_edited;
    std::optional<bool> can_post_messages;
    std::optional<bool> can_edit_messages;
    std::optional<bool> can_delete_messages;
    std::optional<bool> can_restrict_messages;
    std::optional<bool> can_promote_messages;
    std::optional<bool> can_change_info;
    std::optional<bool> can_invite_users;
    std::optional<bool> can_pin_messages;
    std::optional<bool> is_member;
    std::optional<bool> can_send_messages;
    std::optional<bool> can_send_media_messages;
    std::optional<bool> can_send_polls;
    std::optional<bool> can_send_other_messages;
    std::optional<bool> can_add_web_page_previews;
};

/// Describes actions that a non-administrator user is allowed to take in a chat
struct ChatPermissions {
    std::optional<bool> can_send_messages;
    std::optional<bool> can_send_media_messages;
    std::optional<bool> can_send_polls;
    std::optional<bool> can_send_other_messages;
    std::optional<bool> can_add_web_page_previews;
    std::optional<bool> can_change_info;
    std::optional<bool> can_invite_users;
    std::optional<bool> can_pin_messages;
};
/// Contains information about why a request was unsuccessful.
struct ResponseParameters {
    std::optional<int64_t> migrate_to_chat_id;
    std::optional<int32_t> retry_after;
};
/// Represents a photo to be sent.
struct InputMediaPhoto {
    std::string type;
    std::string media;
    std::optional<std::string> caption;
    std::optional<std::string> parse_mode;
};
/// Represents a video to be sent.
struct InputMediaVideo {
    std::string type;
    std::string media;
    std::optional<std::variant<InputFile,std::string>> thumb;
    std::optional<std::string> caption;
    std::optional<std::string> parse_mode;
    std::optional<uint32_t> width;
    std::optional<uint32_t> height;
    std::optional<uint32_t> duration;
    std::optional<bool> supports_streaming;
};
///Represents an animation file (GIF or H.264/MPEG-4 AVC video without sound) to be sent.
struct InputMediaAnimation {
    std::string type;
    std::string media;
    std::optional<std::variant<InputFile,std::string>> thumb;
    std::optional<std::string> caption;
    std::optional<std::string> parse_mode;
    std::optional<uint32_t> width;
    std::optional<uint32_t> height;
    std::optional<uint32_t> duration;
};
/// Represents an audio file to be treated as music to be sent.
struct InputMediaAudio {
    std::string type;
    std::string media;
    std::optional<std::variant<InputFile,std::string>> thumb;
    std::optional<std::string> caption;
    std::optional<std::string> parse_mode;
    std::optional<uint32_t> duration;
    std::optional<std::string> performer;
};
/// Represents a general file to be sent.
struct InputMediaDocument {
    std::string type;
    std::string media;
    std::optional<std::variant<InputFile,std::string>> thumb;
    std::optional<std::string> caption;
    std::optional<std::string> parse_mode;
};
