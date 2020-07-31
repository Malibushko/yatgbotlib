#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <variant>
#include <memory>
#include <string_view>

namespace telegram {
/**
  * Main idea of these macros is to provide additional
  * information about structure fields (e.g field names)
  * so we would have possibility to find out field name
  * with the following syntax
  *
  * T::field_info<INDEX>::name
  *
  * What for? - To iterate over structures with magic_get (boost::pfr)
  * If you have not worked with magic_get,
  * know that it allows you to get a link to the structure field by index.
  *
  * So now it must be clear, that using only index we can find out
  * field name and field data and then serialize struct like that:
  * size_t index = N;
  * jsonDocument[Structure::field_info<index>::name] = boost::pfr::get<index>(StructureObject);
  * see implemementation details in json_parser.h
  */

/**
 * This code is based on loopholes and will only be activated if
 * __COUNTER__ is not defined (it IS on msvc, gcc and clang)
 */
#ifndef __COUNTER__
constexpr static int MAX_DEPTH = 64;
template<uint64_t N>
struct flag {
    friend constexpr int adl_flag(flag<N>);
};
template<uint64_t N> struct depth {};

template<uint64_t N>
struct mark {
    friend constexpr int adl_flag (flag<N>) {
        return N;
    }

    static constexpr int value = N;
};
template <uint64_t D, uint64_t N, class = char[noexcept( adl_flag(flag<N>()) ) ? +1 : -1]>
int64_t constexpr binary_search_flag(int,  depth<D>, flag<N>,
                                     int next_flag = binary_search_flag(0, depth<D-1>(), flag<N + (1 << (D - 1))>())) {
    return next_flag;
}

template <uint64_t D, uint64_t N>
int64_t constexpr binary_search_flag(float, depth<D>, flag<N>,
                                     int next_flag = binary_search_flag(0, depth<D-1>(), flag<N - (uint64_t{1} << (D - 1))>())) {
    return next_flag;
}

template <uint64_t N, class = char[noexcept( adl_flag(flag<N>()) ) ? +1 : -1]>
int64_t constexpr binary_search_flag(int,   depth<0>, flag<N>) {
    return N + 1;
}

template <uint64_t N>
int64_t constexpr binary_search_flag(float, depth<0>, flag<N>) {
    return N;
}

template<int64_t next_flag = binary_search_flag(0, depth<MAX_DEPTH-1>(),
         flag<uint64_t{1} << (MAX_DEPTH-1)>())>
int constexpr counter_id(int value = mark<next_flag>::value) {
    return value;
}


#define declare_field(type, field_name) \
    type field_name; \
    template<typename Dummy__ >       \
    struct field_info<counter_id()-current_counter-1,Dummy__> \
{ constexpr static std::string_view name = #field_name;}

#define declare_struct template<size_t N,class Dummy = void> struct field_info; \
    static constexpr bool is_parsable = true;\
    static constexpr int current_counter = counter_id();

#else
/**
  * Macro declare field based on arguments
  * Also it adds field_info specialization to structure declaration
  * field_info contains info about field_name
  */
#ifndef declare_field
#define declare_field(type, field_name) \
    type field_name; \
    template<typename Dummy__ >       \
    struct field_info<__COUNTER__-current_counter-1,Dummy__> \
    { constexpr static std::string_view name = #field_name;}
#endif
/**
 * Declares field_info class (which is specialized by field_info)
 * And remembers current value of __COUNTER__ as offset
 */

#ifndef declare_struct
#define declare_struct template<size_t N,class Dummy = void> struct field_info; \
    static constexpr bool is_parsable = true;\
    static constexpr int current_counter = __COUNTER__;
#endif
#endif


struct Update;
struct WebhookInfo;
struct User;
struct Chat;
struct Message;
struct MessageEntity;
struct PhotoSize;
struct Animation;
struct Audio;
struct Document;
struct Video;
struct VideoNote;
struct Voice;
struct Contact;
struct Dice;
struct PollOption;
struct PollAnswer;
struct Poll;
struct Location;
struct Venue;
struct UserProfilePhotos;
struct File;
struct ReplyKeyboardMarkup;
struct KeyboardButton;
struct KeyboardButtonPollType;
struct ReplyKeyboardRemove;
struct InlineKeyboardMarkup;
struct InlineKeyboardButton;
struct LoginUrl;
struct CallbackQuery;
struct ForceReply;
struct ChatPhoto;
struct ChatMember;
struct ChatPermissions;
struct BotCommand;
struct ResponseParameters;
struct InputMedia;
struct InputMediaPhoto;
struct InputMediaVideo;
struct InputMediaAnimation;
struct InputMediaAudio;
struct InputMediaDocument;
struct Stickers;
struct Sticker;
struct StickerSet;
struct MaskPosition;
struct InlineQuery;
struct InlineQueryResult;
struct InlineQueryResultArticle;
struct InlineQueryResultPhoto;
struct InlineQueryResultGif;
struct InlineQueryResultMpeg4Gif;
struct InlineQueryResultVideo;
struct InlineQueryResultAudio;
struct InlineQueryResultVoice;
struct InlineQueryResultDocument;
struct InlineQueryResultLocation;
struct InlineQueryResultVenue;
struct InlineQueryResultContact;
struct InlineQueryResultGame;
struct InlineQueryResultCachedPhoto;
struct InlineQueryResultCachedGif;
struct InlineQueryResultCachedMpeg4Gif;
struct InlineQueryResultCachedSticker;
struct InlineQueryResultCachedDocument;
struct InlineQueryResultCachedVideo;
struct InlineQueryResultCachedVoice;
struct InlineQueryResultCachedAudio;
struct InputTextMessageContent;
struct InputLocationMessageContent;
struct InputVenueMessageContent;
struct InputContactMessageContent;
struct ChosenInlineResult;
struct Payments;
struct LabeledPrice;
struct Invoice;
struct ShippingAddress;
struct OrderInfo;
struct ShippingOption;
struct SuccessfulPayment;
struct ShippingQuery;
struct PreCheckoutQuery;
struct PassportData;
struct PassportFile;
struct EncryptedPassportElement;
struct EncryptedCredentials;
struct PassportElementError;
struct PassportElementErrorDataField;
struct PassportElementErrorFrontSide;
struct PassportElementErrorReverseSide;
struct PassportElementErrorSelfie;
struct PassportElementErrorFile;
struct PassportElementErrorFiles;
struct PassportElementErrorTranslationFile;
struct PassportElementErrorTranslationFiles;
struct PassportElementErrorUnspecified;
struct Games;
struct Game;
struct CallbackGame;
struct GameHighScore;
using InputFile = File;
using InputMessageContent = std::variant<InputTextMessageContent,InputLocationMessageContent,InputVenueMessageContent,InputContactMessageContent>;
/// Contains information about the current status of a webhook.
struct WebhookInfo {
declare_struct
declare_field(std::string,url); /// Webhook URL, may be empty if webhook is not set up
declare_field(bool,has_custom_certificate); /// True, if a custom certificate was provided for webhook certificate checks
declare_field(int64_t,pending_update_count); /// Number of updates awaiting delivery
declare_field(std::optional<int64_t>,last_error_date); /// Optional. Unix time for the most recent error that happened when trying to deliver an update via webhook
declare_field(std::optional<std::string>,last_error_message); /// Optional. Error message in human-readable format for the most recent error that happened when trying to deliver an update via webhook
declare_field(std::optional<int64_t>,max_connections); /// Optional. Maximum allowed number of simultaneous HTTPS connections to the webhook for update delivery
declare_field(std::optional<std::vector<std::string>>,allowed_updates); /// Optional. A list of update types the bot is subscribed to. Defaults to all update types
};
/// This object represents a Telegram user or bot.
struct User {
declare_struct
declare_field(int64_t,id); /// Unique identifier for this user or bot
declare_field(bool,is_bot); /// True, if this user is a bot
declare_field(std::string,first_name); /// User's or bot's first name
declare_field(std::optional<std::string>,last_name); /// Optional. User's or bot's last name
declare_field(std::optional<std::string>,username); /// Optional. User's or bot's username
declare_field(std::optional<std::string>,language_code); /// Optional. IETF language tag of the user's language
declare_field(std::optional<bool>,can_join_groups); /// Optional. True, if the bot can be invited to groups. Returned only in getMe.
declare_field(std::optional<bool>,can_read_all_group_messages); /// Optional. True, if privacy mode is disabled for the bot. Returned only in getMe.
declare_field(std::optional<bool>,supports_inline_queries); /// Optional. True, if the bot supports inline queries. Returned only in getMe.
};
/// This object represents one special entity in a text message. For example, hashtags, usernames, URLs, etc.
struct MessageEntity {
declare_struct
declare_field(std::string,type); /// Type of the entity. Can be "mention" (@username), "hashtag" (#hashtag), "cashtag" ($USD), "bot_command" (/start@jobs_bot), "url" (https://telegram.org), "email" (do-not-reply@telegram.org), "phone_number" (+1-212-555-0123), "bold" (bold text), "italic" (italic text), "underline" (underlined text), "strikethrough" (strikethrough text), "code" (monowidth string), "pre" (monowidth block), "text_link" (for clickable text URLs), "text_mention" (for users without usernames)
declare_field(int64_t,offset); /// Offset in UTF-16 code units to the start of the entity
declare_field(int64_t,length); /// Length of the entity in UTF-16 code units
declare_field(std::optional<std::string>,url); /// Optional. For "text_link" only, url that will be opened after user taps on the text
declare_field(std::optional<User>,user); /// Optional. For "text_mention" only, the mentioned user
declare_field(std::optional<std::string>,language); /// Optional. For "pre" only, the programming language of the entity text
};
/// This object represents one size of a photo or a file / sticker thumbnail.
struct PhotoSize {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,width); /// Photo width
declare_field(int64_t,height); /// Photo height
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents an animation file (GIF or H.264/MPEG-4 AVC video without sound).
struct Animation {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,width); /// Video width as defined by sender
declare_field(int64_t,height); /// Video height as defined by sender
declare_field(int64_t,duration); /// Duration of the video in seconds as defined by sender
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Animation thumbnail as defined by sender
declare_field(std::optional<std::string>,file_name); /// Optional. Original animation filename as defined by sender
declare_field(std::optional<std::string>,mime_type); /// Optional. MIME type of the file as defined by sender
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents an audio file to be treated as music by the Telegram clients.
struct Audio {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,duration); /// Duration of the audio in seconds as defined by sender
declare_field(std::optional<std::string>,performer); /// Optional. Performer of the audio as defined by sender or by audio tags
declare_field(std::optional<std::string>,title); /// Optional. Title of the audio as defined by sender or by audio tags
declare_field(std::optional<std::string>,mime_type); /// Optional. MIME type of the file as defined by sender
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Thumbnail of the album cover to which the music file belongs
};
/// This object represents a general file (as opposed to photos, voice messages and audio files).
struct Document {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Document thumbnail as defined by sender
declare_field(std::optional<std::string>,file_name); /// Optional. Original filename as defined by sender
declare_field(std::optional<std::string>,mime_type); /// Optional. MIME type of the file as defined by sender
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents a video file.
struct Video {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,width); /// Video width as defined by sender
declare_field(int64_t,height); /// Video height as defined by sender
declare_field(int64_t,duration); /// Duration of the video in seconds as defined by sender
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Video thumbnail
declare_field(std::optional<std::string>,mime_type); /// Optional. Mime type of a file as defined by sender
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents a video message (available in Telegram apps as of v.4.0).
struct VideoNote {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,length); /// Video width and height (diameter of the video message) as defined by sender
declare_field(int64_t,duration); /// Duration of the video in seconds as defined by sender
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Video thumbnail
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents a voice note.
struct Voice {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,duration); /// Duration of the audio in seconds as defined by sender
declare_field(std::optional<std::string>,mime_type); /// Optional. MIME type of the file as defined by sender
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents a phone contact.
struct Contact {
declare_struct
declare_field(std::string,phone_number); /// Contact's phone number
declare_field(std::string,first_name); /// Contact's first name
declare_field(std::optional<std::string>,last_name); /// Optional. Contact's last name
declare_field(std::optional<int64_t>,user_id); /// Optional. Contact's user identifier in Telegram
declare_field(std::optional<std::string>,vcard); /// Optional. Additional data about the contact in the form of a vCard
};
/// This object represents an animated emoji that displays a random value.
struct Dice {
declare_struct
declare_field(std::string,emoji); /// Emoji on which the dice throw animation is based
declare_field(int64_t,value); /// Value of the dice, 1-6 for "" and "" base emoji, 1-5 for "" base emoji
};
/// This object contains information about one answer option in a poll.
struct PollOption {
declare_struct
declare_field(std::string,text); /// Option text, 1-100 characters
declare_field(int64_t,voter_count); /// Number of users that voted for this option
};
/// This object represents an answer of a user in a non-anonymous poll.
struct PollAnswer {
declare_struct
declare_field(std::string,poll_id); /// Unique poll identifier
declare_field(User,user); /// The user, who changed the answer to the poll
declare_field(std::vector<int64_t>,option_ids); /// 0-based identifiers of answer options, chosen by the user. May be empty if the user retracted their vote.
};
/// This object contains information about a poll.
struct Poll {
declare_struct
declare_field(std::string,id); /// Unique poll identifier
declare_field(std::string,question); /// Poll question, 1-255 characters
declare_field(std::vector<PollOption>,options); /// List of poll options
declare_field(int64_t,total_voter_count); /// Total number of users that voted in the poll
declare_field(bool,is_closed); /// True, if the poll is closed
declare_field(bool,is_anonymous); /// True, if the poll is anonymous
declare_field(std::string,type); /// Poll type, currently can be "regular" or "quiz"
declare_field(bool,allows_multiple_answers); /// True, if the poll allows multiple answers
declare_field(std::optional<int64_t>,correct_option_id); /// Optional. 0-based identifier of the correct answer option. Available only for polls in the quiz mode, which are closed, or was sent (not forwarded) by the bot or to the private chat with the bot.
declare_field(std::optional<std::string>,explanation); /// Optional. Text that is shown when a user chooses an incorrect answer or taps on the lamp icon in a quiz-style poll, 0-200 characters
declare_field(std::optional<std::vector<MessageEntity>>,explanation_entities); /// Optional. Special entities like usernames, URLs, bot commands, etc. that appear in the explanation
declare_field(std::optional<int64_t>,open_period); /// Optional. Amount of time in seconds the poll will be active after creation
declare_field(std::optional<int64_t>,close_date); /// Optional. Point in time (Unix timestamp) when the poll will be automatically closed
};
/// This object represents a point on the map.
struct Location {
declare_struct
declare_field(float,longitude); /// Longitude as defined by sender
declare_field(float,latitude); /// Latitude as defined by sender
};
/// This object represents a venue.
struct Venue {
declare_struct
declare_field(Location,location); /// Venue location
declare_field(std::string,title); /// Name of the venue
declare_field(std::string,address); /// Address of the venue
declare_field(std::optional<std::string>,foursquare_id); /// Optional. Foursquare identifier of the venue
declare_field(std::optional<std::string>,foursquare_type); /// Optional. Foursquare type of the venue. (For example, "arts_entertainment/default", "arts_entertainment/aquarium" or "food/icecream".)
};
/// This object represent a user's profile pictures.
struct UserProfilePhotos {
declare_struct
declare_field(int64_t,total_count); /// Total number of profile pictures the target user has
declare_field(std::vector<std::vector<PhotoSize>>,photos); /// Requested profile pictures (in up to 4 sizes each)
};
/// This object represents a file ready to be downloaded. The file can be downloaded via the link https://api.telegram.org/file/bot<token>/<file_path>. It is guaranteed that the link will be valid for at least 1 hour. When the link expires, a new one can be requested by calling getFile.
struct File {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(std::optional<int64_t>,file_size); /// Optional. File size, if known
declare_field(std::optional<std::string>,file_path); /// Optional. File path. Use https://api.telegram.org/file/bot<token>/<file_path> to get the file.
};
/// This object represents a custom keyboard with reply options (see Introduction to bots for details and examples).
struct ReplyKeyboardMarkup {
declare_struct
declare_field(std::vector<std::vector<KeyboardButton>>,keyboard); /// Array of button rows, each represented by an Array of KeyboardButton objects
declare_field(std::optional<bool>,resize_keyboard); /// Optional. Requests clients to resize the keyboard vertically for optimal fit (e.g., make the keyboard smaller if there are just two rows of buttons). Defaults to false, in which case the custom keyboard is always of the same height as the app's standard keyboard.
declare_field(std::optional<bool>,one_time_keyboard); /// Optional. Requests clients to hide the keyboard as soon as it's been used. The keyboard will still be available, but clients will automatically display the usual letter-keyboard in the chat - the user can press a special button in the input field to see the custom keyboard again. Defaults to false.
declare_field(std::optional<bool>,selective); /// Optional. Use this parameter if you want to show the keyboard to specific users only. Targets: 1) users that are @mentioned in the text of the Message object; 2) if the bot's message is a reply (has reply_to_message_id), sender of the original message.Example: A user requests to change the bot's language, bot replies to the request with a keyboard to select the new language. Other users in the group don't see the keyboard.
};
/// This object represents type of a poll, which is allowed to be created and sent when the corresponding button is pressed.
struct KeyboardButtonPollType {
declare_struct
declare_field(std::optional<std::string>,type); /// Optional. If quiz is passed, the user will be allowed to create only polls in the quiz mode. If regular is passed, only regular polls will be allowed. Otherwise, the user will be allowed to create a poll of any type.
};
/// Upon receiving a message with this object, Telegram clients will remove the current custom keyboard and display the default letter-keyboard. By default, custom keyboards are displayed until a new keyboard is sent by a bot. An exception is made for one-time keyboards that are hidden immediately after the user presses a button (see ReplyKeyboardMarkup).
struct ReplyKeyboardRemove {
declare_struct
declare_field(bool,remove_keyboard); /// Requests clients to remove the custom keyboard (user will not be able to summon this keyboard; if you want to hide the keyboard from sight but keep it accessible, use one_time_keyboard in ReplyKeyboardMarkup)
declare_field(std::optional<bool>,selective); /// Optional. Use this parameter if you want to remove the keyboard for specific users only. Targets: 1) users that are @mentioned in the text of the Message object; 2) if the bot's message is a reply (has reply_to_message_id), sender of the original message.Example: A user votes in a poll, bot returns confirmation message in reply to the vote and removes the keyboard for that user, while still showing the keyboard with poll options to users who haven't voted yet.
};
/// This object represents an inline keyboard that appears right next to the message it belongs to.
struct InlineKeyboardMarkup {
declare_struct
declare_field(std::vector<std::vector<InlineKeyboardButton>>,inline_keyboard); /// Array of button rows, each represented by an Array of InlineKeyboardButton objects
};
/// This object represents a parameter of the inline keyboard button used to automatically authorize a user. Serves as a great replacement for the Telegram Login Widget when the user is coming from Telegram. All the user needs to do is tap/click a button and confirm that they want to log in:
struct LoginUrl {
declare_struct
declare_field(std::string,url); /// An HTTP URL to be opened with user authorization data added to the query string when the button is pressed. If the user refuses to provide authorization data, the original URL without information about the user will be opened. The data added is the same as described in Receiving authorization data.NOTE: You must always check the hash of the received data to verify the authentication and the integrity of the data as described in Checking authorization.
declare_field(std::optional<std::string>,forward_text); /// Optional. New text of the button in forwarded messages.
declare_field(std::optional<std::string>,bot_username); /// Optional. Username of a bot, which will be used for user authorization. See Setting up a bot for more details. If not specified, the current bot's username will be assumed. The url's domain must be the same as the domain linked with the bot. See Linking your domain to the bot for more details.
declare_field(std::optional<bool>,request_write_access); /// Optional. Pass True to request the permission for your bot to send messages to the user.
};
/// Upon receiving a message with this object, Telegram clients will display a reply interface to the user (act as if the user has selected the bot's message and tapped 'Reply'). This can be extremely useful if you want to create user-friendly step-by-step interfaces without having to sacrifice privacy mode.
struct ForceReply {
declare_struct
declare_field(bool,force_reply); /// Shows reply interface to the user, as if they manually selected the bot's message and tapped 'Reply'
declare_field(std::optional<bool>,selective); /// Optional. Use this parameter if you want to force reply from specific users only. Targets: 1) users that are @mentioned in the text of the Message object; 2) if the bot's message is a reply (has reply_to_message_id), sender of the original message.
};
/// This object represents a chat photo.
struct ChatPhoto {
declare_struct
declare_field(std::string,small_file_id); /// File identifier of small (160x160) chat photo. This file_id can be used only for photo download and only for as long as the photo is not changed.
declare_field(std::string,small_file_unique_id); /// Unique file identifier of small (160x160) chat photo, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(std::string,big_file_id); /// File identifier of big (640x640) chat photo. This file_id can be used only for photo download and only for as long as the photo is not changed.
declare_field(std::string,big_file_unique_id); /// Unique file identifier of big (640x640) chat photo, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
};
/// This object contains information about one member of a chat.
struct ChatMember {
declare_struct
declare_field(User,user); /// Information about the user
declare_field(std::string,status); /// The member's status in the chat. Can be "creator", "administrator", "member", "restricted", "left" or "kicked"
declare_field(std::optional<std::string>,custom_title); /// Optional. Owner and administrators only. Custom title for this user
declare_field(std::optional<int64_t>,until_date); /// Optional. Restricted and kicked only. Date when restrictions will be lifted for this user; unix time
declare_field(std::optional<bool>,can_be_edited); /// Optional. Administrators only. True, if the bot is allowed to edit administrator privileges of that user
declare_field(std::optional<bool>,can_post_messages); /// Optional. Administrators only. True, if the administrator can post in the channel; channels only
declare_field(std::optional<bool>,can_edit_messages); /// Optional. Administrators only. True, if the administrator can edit messages of other users and can pin messages; channels only
declare_field(std::optional<bool>,can_delete_messages); /// Optional. Administrators only. True, if the administrator can delete messages of other users
declare_field(std::optional<bool>,can_restrict_members); /// Optional. Administrators only. True, if the administrator can restrict, ban or unban chat members
declare_field(std::optional<bool>,can_promote_members); /// Optional. Administrators only. True, if the administrator can add new administrators with a subset of their own privileges or demote administrators that he has promoted, directly or indirectly (promoted by administrators that were appointed by the user)
declare_field(std::optional<bool>,can_change_info); /// Optional. Administrators and restricted only. True, if the user is allowed to change the chat title, photo and other settings
declare_field(std::optional<bool>,can_invite_users); /// Optional. Administrators and restricted only. True, if the user is allowed to invite new users to the chat
declare_field(std::optional<bool>,can_pin_messages); /// Optional. Administrators and restricted only. True, if the user is allowed to pin messages; groups and supergroups only
declare_field(std::optional<bool>,is_member); /// Optional. Restricted only. True, if the user is a member of the chat at the moment of the request
declare_field(std::optional<bool>,can_send_messages); /// Optional. Restricted only. True, if the user is allowed to send text messages, contacts, locations and venues
declare_field(std::optional<bool>,can_send_media_messages); /// Optional. Restricted only. True, if the user is allowed to send audios, documents, photos, videos, video notes and voice notes
declare_field(std::optional<bool>,can_send_polls); /// Optional. Restricted only. True, if the user is allowed to send polls
declare_field(std::optional<bool>,can_send_other_messages); /// Optional. Restricted only. True, if the user is allowed to send animations, games, stickers and use inline bots
declare_field(std::optional<bool>,can_add_web_page_previews); /// Optional. Restricted only. True, if the user is allowed to add web page previews to their messages
};
/// Describes actions that a non-administrator user is allowed to take in a chat.
struct ChatPermissions {
declare_struct
declare_field(std::optional<bool>,can_send_messages); /// Optional. True, if the user is allowed to send text messages, contacts, locations and venues
declare_field(std::optional<bool>,can_send_media_messages); /// Optional. True, if the user is allowed to send audios, documents, photos, videos, video notes and voice notes, implies can_send_messages
declare_field(std::optional<bool>,can_send_polls); /// Optional. True, if the user is allowed to send polls, implies can_send_messages
declare_field(std::optional<bool>,can_send_other_messages); /// Optional. True, if the user is allowed to send animations, games, stickers and use inline bots, implies can_send_media_messages
declare_field(std::optional<bool>,can_add_web_page_previews); /// Optional. True, if the user is allowed to add web page previews to their messages, implies can_send_media_messages
declare_field(std::optional<bool>,can_change_info); /// Optional. True, if the user is allowed to change the chat title, photo and other settings. Ignored in public supergroups
declare_field(std::optional<bool>,can_invite_users); /// Optional. True, if the user is allowed to invite new users to the chat
declare_field(std::optional<bool>,can_pin_messages); /// Optional. True, if the user is allowed to pin messages. Ignored in public supergroups
};
/// This object represents a bot command.
struct BotCommand {
declare_struct
declare_field(std::string,command); /// Text of the command, 1-32 characters. Can contain only lowercase English letters, digits and underscores.
declare_field(std::string,description); /// Description of the command, 3-256 characters.
};
/// Contains information about why a request was unsuccessful.
struct ResponseParameters {
declare_struct
declare_field(std::optional<int64_t>,migrate_to_chat_id); /// Optional. The group has been migrated to a supergroup with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it is smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
declare_field(std::optional<int64_t>,retry_after); /// Optional. In case of exceeding flood control, the number of seconds left to wait before the request can be repeated
};
/// This object represents the content of a media message to be sent. It should be one of
struct InputMedia {
declare_struct
declare_field(std::string,type); /// Type of the result, must be photo
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the photo to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the photo caption. See formatting options for more details.
};
/// Represents a photo to be sent.
struct InputMediaPhoto {
declare_struct
declare_field(std::string,type); /// Type of the result, must be photo
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the photo to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the photo caption. See formatting options for more details.
};
/// Represents a video to be sent.
struct InputMediaVideo {
declare_struct
declare_field(std::string,type); /// Type of the result, must be video
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<InputFile>,thumb); /// Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the video to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the video caption. See formatting options for more details.
declare_field(std::optional<int64_t>,width); /// Optional. Video width
declare_field(std::optional<int64_t>,height); /// Optional. Video height
declare_field(std::optional<int64_t>,duration); /// Optional. Video duration
declare_field(std::optional<bool>,supports_streaming); /// Optional. Pass True, if the uploaded video is suitable for streaming
};
/// Represents an animation file (GIF or H.264/MPEG-4 AVC video without sound) to be sent.
struct InputMediaAnimation {
declare_struct
declare_field(std::string,type); /// Type of the result, must be animation
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<InputFile>,thumb); /// Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the animation to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the animation caption. See formatting options for more details.
declare_field(std::optional<int64_t>,width); /// Optional. Animation width
declare_field(std::optional<int64_t>,height); /// Optional. Animation height
declare_field(std::optional<int64_t>,duration); /// Optional. Animation duration
};
/// Represents an audio file to be treated as music to be sent.
struct InputMediaAudio {
declare_struct
declare_field(std::string,type); /// Type of the result, must be audio
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<InputFile>,thumb); /// Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the audio to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the audio caption. See formatting options for more details.
declare_field(std::optional<int64_t>,duration); /// Optional. Duration of the audio in seconds
declare_field(std::optional<std::string>,performer); /// Optional. Performer of the audio
declare_field(std::optional<std::string>,title); /// Optional. Title of the audio
};
/// Represents a general file to be sent.
struct InputMediaDocument {
declare_struct
declare_field(std::string,type); /// Type of the result, must be document
declare_field(std::string,media); /// File to send. Pass a file_id to send a file that exists on the Telegram servers (recommended), pass an HTTP URL for Telegram to get a file from the Internet, or pass "attach://<file_attach_name>" to upload a new one using multipart/form-data under <file_attach_name> name. 
declare_field(std::optional<InputFile>,thumb); /// Optional. Thumbnail of the file sent; can be ignored if thumbnail generation for the file is supported server-side. The thumbnail should be in JPEG format and less than 200 kB in size. A thumbnail's width and height should not exceed 320. Ignored if the file is not uploaded using multipart/form-data. Thumbnails can't be reused and can be only uploaded as a new file, so you can pass "attach://<file_attach_name>" if the thumbnail was uploaded using multipart/form-data under <file_attach_name>. 
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the document to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the document caption. See formatting options for more details.
};
/// This object represents a sticker set.
struct StickerSet {
declare_struct
declare_field(std::string,name); /// Sticker set name
declare_field(std::string,title); /// Sticker set title
declare_field(bool,is_animated); /// True, if the sticker set contains animated stickers
declare_field(bool,contains_masks); /// True, if the sticker set contains masks
declare_field(std::vector<Sticker>,stickers); /// List of all set stickers
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Sticker set thumbnail in the .WEBP or .TGS format
};
/// This object describes the position on faces where a mask should be placed by default.
struct MaskPosition {
declare_struct
declare_field(std::string,point); /// The part of the face relative to which the mask should be placed. One of "forehead", "eyes", "mouth", or "chin".
declare_field(float,x_shift); /// Shift by X-axis measured in widths of the mask scaled to the face size, from left to right. For example, choosing -1.0 will place mask just to the left of the default mask position.
declare_field(float,y_shift); /// Shift by Y-axis measured in heights of the mask scaled to the face size, from top to bottom. For example, 1.0 will place the mask just below the default mask position.
declare_field(float,scale); /// Mask scaling coefficient. For example, 2.0 means double size.
};
/// This object represents an incoming inline query. When the user sends an empty query, your bot could return some default or trending results.
struct InlineQuery {
declare_struct
declare_field(std::string,id); /// Unique identifier for this query
declare_field(User,from); /// Sender
declare_field(std::optional<Location>,location); /// Optional. Sender location, only for bots that request user location
declare_field(std::string,query); /// Text of the query (up to 256 characters)
declare_field(std::string,offset); /// Offset of the results to be returned, can be controlled by the bot
};
/// Represents a Game.
struct InlineQueryResultGame {
declare_struct
declare_field(std::string,type); /// Type of the result, must be game
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,game_short_name); /// Short name of the game
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
};
/// Represents the content of a text message to be sent as the result of an inline query.
struct InputTextMessageContent {
declare_struct
declare_field(std::string,message_text); /// Text of the message to be sent, 1-4096 characters
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the message text. See formatting options for more details.
declare_field(std::optional<bool>,disable_web_page_preview); /// Optional. Disables link previews for links in the sent message
};
/// Represents the content of a location message to be sent as the result of an inline query.
struct InputLocationMessageContent {
declare_struct
declare_field(float,latitude); /// Latitude of the location in degrees
declare_field(float,longitude); /// Longitude of the location in degrees
declare_field(std::optional<int64_t>,live_period); /// Optional. Period in seconds for which the location can be updated, should be between 60 and 86400.
};
/// Represents the content of a venue message to be sent as the result of an inline query.
struct InputVenueMessageContent {
declare_struct
declare_field(float,latitude); /// Latitude of the venue in degrees
declare_field(float,longitude); /// Longitude of the venue in degrees
declare_field(std::string,title); /// Name of the venue
declare_field(std::string,address); /// Address of the venue
declare_field(std::optional<std::string>,foursquare_id); /// Optional. Foursquare identifier of the venue, if known
declare_field(std::optional<std::string>,foursquare_type); /// Optional. Foursquare type of the venue, if known. (For example, "arts_entertainment/default", "arts_entertainment/aquarium" or "food/icecream".)
};
/// Represents the content of a contact message to be sent as the result of an inline query.
struct InputContactMessageContent {
declare_struct
declare_field(std::string,phone_number); /// Contact's phone number
declare_field(std::string,first_name); /// Contact's first name
declare_field(std::optional<std::string>,last_name); /// Optional. Contact's last name
declare_field(std::optional<std::string>,vcard); /// Optional. Additional data about the contact in the form of a vCard, 0-2048 bytes
};
using InputMessageContent = std::variant<InputTextMessageContent,InputLocationMessageContent,InputVenueMessageContent,InputContactMessageContent>;
/// Represents a result of an inline query that was chosen by the user and sent to their chat partner.
struct ChosenInlineResult {
declare_struct
declare_field(std::string,result_id); /// The unique identifier for the result that was chosen
declare_field(User,from); /// The user that chose the result
declare_field(std::optional<Location>,location); /// Optional. Sender location, only for bots that require user location
declare_field(std::optional<std::string>,inline_message_id); /// Optional. Identifier of the sent inline message. Available only if there is an inline keyboard attached to the message. Will be also received in callback queries and can be used to edit the message.
declare_field(std::string,query); /// The query that was used to obtain the result
};
/// Your bot can accept payments from Telegram users. Please see the introduction to payments for more details on the process and how to set up payments for your bot. Please note that users will need Telegram v.4.0 or higher to use payments (released on May 18, 2017).
struct Payments {
declare_struct
declare_field(int64_t,chat_id); /// Unique identifier for the target private chat
declare_field(std::string,title); /// Product name, 1-32 characters
declare_field(std::string,description); /// Product description, 1-255 characters
declare_field(std::string,payload); /// Bot-defined invoice payload, 1-128 bytes. This will not be displayed to the user, use for your internal processes.
declare_field(std::string,provider_token); /// Payments provider token, obtained via Botfather
declare_field(std::string,start_parameter); /// Unique deep-linking parameter that can be used to generate this invoice when used as a start parameter
declare_field(std::string,currency); /// Three-letter ISO 4217 currency code, see more on currencies
declare_field(std::vector<LabeledPrice>,prices); /// Price breakdown, a JSON-serialized list of components (e.g. product price, tax, discount, delivery cost, delivery tax, bonus, etc.)
declare_field(std::optional<std::string>,provider_data); /// A JSON-serialized data about the invoice, which will be shared with the payment provider. A detailed description of required fields should be provided by the payment provider.
declare_field(std::optional<std::string>,photo_url); /// URL of the product photo for the invoice. Can be a photo of the goods or a marketing image for a service. People like it better when they see what they are paying for.
declare_field(std::optional<int64_t>,photo_size); /// Photo size
declare_field(std::optional<int64_t>,photo_width); /// Photo width
declare_field(std::optional<int64_t>,photo_height); /// Photo height
declare_field(std::optional<bool>,need_name); /// Pass True, if you require the user's full name to complete the order
declare_field(std::optional<bool>,need_phone_number); /// Pass True, if you require the user's phone number to complete the order
declare_field(std::optional<bool>,need_email); /// Pass True, if you require the user's email address to complete the order
declare_field(std::optional<bool>,need_shipping_address); /// Pass True, if you require the user's shipping address to complete the order
declare_field(std::optional<bool>,send_phone_number_to_provider); /// Pass True, if user's phone number should be sent to provider
declare_field(std::optional<bool>,send_email_to_provider); /// Pass True, if user's email address should be sent to provider
declare_field(std::optional<bool>,is_flexible); /// Pass True, if the final price depends on the shipping method
declare_field(std::optional<bool>,disable_notification); /// Sends the message silently. Users will receive a notification with no sound.
declare_field(std::optional<int64_t>,reply_to_message_id); /// If the message is a reply, ID of the original message
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// A JSON-serialized object for an inline keyboard. If empty, one 'Pay total price' button will be shown. If not empty, the first button must be a Pay button.
};
/// This object represents a portion of the price for goods or services.
struct LabeledPrice {
declare_struct
declare_field(std::string,label); /// Portion label
declare_field(int64_t,amount); /// Price of the product in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).
};
/// This object contains basic information about an invoice.
struct Invoice {
declare_struct
declare_field(std::string,title); /// Product name
declare_field(std::string,description); /// Product description
declare_field(std::string,start_parameter); /// Unique bot deep-linking parameter that can be used to generate this invoice
declare_field(std::string,currency); /// Three-letter ISO 4217 currency code
declare_field(int64_t,total_amount); /// Total price in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).
};
/// This object represents a shipping address.
struct ShippingAddress {
declare_struct
declare_field(std::string,country_code); /// ISO 3166-1 alpha-2 country code
declare_field(std::string,state); /// State, if applicable
declare_field(std::string,city); /// City
declare_field(std::string,street_line1); /// First line for the address
declare_field(std::string,street_line2); /// Second line for the address
declare_field(std::string,post_code); /// Address post code
};
/// This object represents information about an order.
struct OrderInfo {
declare_struct
declare_field(std::optional<std::string>,name); /// Optional. User name
declare_field(std::optional<std::string>,phone_number); /// Optional. User's phone number
declare_field(std::optional<std::string>,email); /// Optional. User email
declare_field(std::optional<ShippingAddress>,shipping_address); /// Optional. User shipping address
};
/// This object represents one shipping option.
struct ShippingOption {
declare_struct
declare_field(std::string,id); /// Shipping option identifier
declare_field(std::string,title); /// Option title
declare_field(std::vector<LabeledPrice>,prices); /// List of price portions
};
/// This object contains basic information about a successful payment.
struct SuccessfulPayment {
declare_struct
declare_field(std::string,currency); /// Three-letter ISO 4217 currency code
declare_field(int64_t,total_amount); /// Total price in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).
declare_field(std::string,invoice_payload); /// Bot specified invoice payload
declare_field(std::optional<std::string>,shipping_option_id); /// Optional. Identifier of the shipping option chosen by the user
declare_field(std::optional<OrderInfo>,order_info); /// Optional. Order info provided by the user
declare_field(std::string,telegram_payment_charge_id); /// Telegram payment identifier
declare_field(std::string,provider_payment_charge_id); /// Provider payment identifier
};
/// This object contains information about an incoming shipping query.
struct ShippingQuery {
declare_struct
declare_field(std::string,id); /// Unique query identifier
declare_field(User,from); /// User who sent the query
declare_field(std::string,invoice_payload); /// Bot specified invoice payload
declare_field(ShippingAddress,shipping_address); /// User specified shipping address
};
/// This object contains information about an incoming pre-checkout query.
struct PreCheckoutQuery {
declare_struct
declare_field(std::string,id); /// Unique query identifier
declare_field(User,from); /// User who sent the query
declare_field(std::string,currency); /// Three-letter ISO 4217 currency code
declare_field(int64_t,total_amount); /// Total price in the smallest units of the currency (integer, not float/double). For example, for a price of US$ 1.45 pass amount = 145. See the exp parameter in currencies.json, it shows the number of digits past the decimal point for each currency (2 for the majority of currencies).
declare_field(std::string,invoice_payload); /// Bot specified invoice payload
declare_field(std::optional<std::string>,shipping_option_id); /// Optional. Identifier of the shipping option chosen by the user
declare_field(std::optional<OrderInfo>,order_info); /// Optional. Order info provided by the user
};
/// This object represents a file uploaded to Telegram Passport. Currently all Telegram Passport files are in JPEG format when decrypted and don't exceed 10MB.
struct PassportFile {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,file_size); /// File size
declare_field(int64_t,file_date); /// Unix time when the file was uploaded
};
/// Contains information about documents or other Telegram Passport elements shared with the bot by the user.
struct EncryptedPassportElement {
declare_struct
declare_field(std::string,type); /// Element type. One of "personal_details", "passport", "driver_license", "identity_card", "internal_passport", "address", "utility_bill", "bank_statement", "rental_agreement", "passport_registration", "temporary_registration", "phone_number", "email".
declare_field(std::optional<std::string>,data); /// Optional. Base64-encoded encrypted Telegram Passport element data provided by the user, available for "personal_details", "passport", "driver_license", "identity_card", "internal_passport" and "address" types. Can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::optional<std::string>,phone_number); /// Optional. User's verified phone number, available only for "phone_number" type
declare_field(std::optional<std::string>,email); /// Optional. User's verified email address, available only for "email" type
declare_field(std::optional<std::vector<PassportFile>>,files); /// Optional. Array of encrypted files with documents provided by the user, available for "utility_bill", "bank_statement", "rental_agreement", "passport_registration" and "temporary_registration" types. Files can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::optional<PassportFile>,front_side); /// Optional. Encrypted file with the front side of the document, provided by the user. Available for "passport", "driver_license", "identity_card" and "internal_passport". The file can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::optional<PassportFile>,reverse_side); /// Optional. Encrypted file with the reverse side of the document, provided by the user. Available for "driver_license" and "identity_card". The file can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::optional<PassportFile>,selfie); /// Optional. Encrypted file with the selfie of the user holding a document, provided by the user; available for "passport", "driver_license", "identity_card" and "internal_passport". The file can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::optional<std::vector<PassportFile>>,translation); /// Optional. Array of encrypted files with translated versions of documents provided by the user. Available if requested for "passport", "driver_license", "identity_card", "internal_passport", "utility_bill", "bank_statement", "rental_agreement", "passport_registration" and "temporary_registration" types. Files can be decrypted and verified using the accompanying EncryptedCredentials.
declare_field(std::string,hash); /// Base64-encoded element hash for using in PassportElementErrorUnspecified
};
/// Contains data required for decrypting and authenticating EncryptedPassportElement. See the Telegram Passport Documentation for a complete description of the data decryption and authentication processes.
struct EncryptedCredentials {
declare_struct
declare_field(std::string,data); /// Base64-encoded encrypted JSON-serialized data with unique user's payload, data hashes and secrets required for EncryptedPassportElement decryption and authentication
declare_field(std::string,hash); /// Base64-encoded data hash for data authentication
declare_field(std::string,secret); /// Base64-encoded secret, encrypted with the bot's public RSA key, required for data decryption
};
/// This object represents an error in the Telegram Passport element which was submitted that should be resolved by the user. It should be one of:
struct PassportElementError {
declare_struct
declare_field(std::string,source); /// Error source, must be data
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the error, one of "personal_details", "passport", "driver_license", "identity_card", "internal_passport", "address"
declare_field(std::string,field_name); /// Name of the data field which has the error
declare_field(std::string,data_hash); /// Base64-encoded data hash
declare_field(std::string,message); /// Error message
};
/// Represents an issue in one of the data fields that was provided by the user. The error is considered resolved when the field's value changes.
struct PassportElementErrorDataField {
declare_struct
declare_field(std::string,source); /// Error source, must be data
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the error, one of "personal_details", "passport", "driver_license", "identity_card", "internal_passport", "address"
declare_field(std::string,field_name); /// Name of the data field which has the error
declare_field(std::string,data_hash); /// Base64-encoded data hash
declare_field(std::string,message); /// Error message
};
/// Represents an issue with the front side of a document. The error is considered resolved when the file with the front side of the document changes.
struct PassportElementErrorFrontSide {
declare_struct
declare_field(std::string,source); /// Error source, must be front_side
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the issue, one of "passport", "driver_license", "identity_card", "internal_passport"
declare_field(std::string,file_hash); /// Base64-encoded hash of the file with the front side of the document
declare_field(std::string,message); /// Error message
};
/// Represents an issue with the reverse side of a document. The error is considered resolved when the file with reverse side of the document changes.
struct PassportElementErrorReverseSide {
declare_struct
declare_field(std::string,source); /// Error source, must be reverse_side
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the issue, one of "driver_license", "identity_card"
declare_field(std::string,file_hash); /// Base64-encoded hash of the file with the reverse side of the document
declare_field(std::string,message); /// Error message
};
/// Represents an issue with the selfie with a document. The error is considered resolved when the file with the selfie changes.
struct PassportElementErrorSelfie {
declare_struct
declare_field(std::string,source); /// Error source, must be selfie
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the issue, one of "passport", "driver_license", "identity_card", "internal_passport"
declare_field(std::string,file_hash); /// Base64-encoded hash of the file with the selfie
declare_field(std::string,message); /// Error message
};
/// Represents an issue with a document scan. The error is considered resolved when the file with the document scan changes.
struct PassportElementErrorFile {
declare_struct
declare_field(std::string,source); /// Error source, must be file
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the issue, one of "utility_bill", "bank_statement", "rental_agreement", "passport_registration", "temporary_registration"
declare_field(std::string,file_hash); /// Base64-encoded file hash
declare_field(std::string,message); /// Error message
};
/// Represents an issue with a list of scans. The error is considered resolved when the list of files containing the scans changes.
struct PassportElementErrorFiles {
declare_struct
declare_field(std::string,source); /// Error source, must be files
declare_field(std::string,type); /// The section of the user's Telegram Passport which has the issue, one of "utility_bill", "bank_statement", "rental_agreement", "passport_registration", "temporary_registration"
declare_field(std::vector<std::string>,file_hashes); /// List of base64-encoded file hashes
declare_field(std::string,message); /// Error message
};
/// Represents an issue with one of the files that constitute the translation of a document. The error is considered resolved when the file changes.
struct PassportElementErrorTranslationFile {
declare_struct
declare_field(std::string,source); /// Error source, must be translation_file
declare_field(std::string,type); /// Type of element of the user's Telegram Passport which has the issue, one of "passport", "driver_license", "identity_card", "internal_passport", "utility_bill", "bank_statement", "rental_agreement", "passport_registration", "temporary_registration"
declare_field(std::string,file_hash); /// Base64-encoded file hash
declare_field(std::string,message); /// Error message
};
/// Represents an issue with the translated version of a document. The error is considered resolved when a file with the document translation change.
struct PassportElementErrorTranslationFiles {
declare_struct
declare_field(std::string,source); /// Error source, must be translation_files
declare_field(std::string,type); /// Type of element of the user's Telegram Passport which has the issue, one of "passport", "driver_license", "identity_card", "internal_passport", "utility_bill", "bank_statement", "rental_agreement", "passport_registration", "temporary_registration"
declare_field(std::vector<std::string>,file_hashes); /// List of base64-encoded file hashes
declare_field(std::string,message); /// Error message
};
/// Represents an issue in an unspecified place. The error is considered resolved when new data is added.
struct PassportElementErrorUnspecified {
declare_struct
declare_field(std::string,source); /// Error source, must be unspecified
declare_field(std::string,type); /// Type of element of the user's Telegram Passport which has the issue
declare_field(std::string,element_hash); /// Base64-encoded element hash
declare_field(std::string,message); /// Error message
};
/// Your bot can offer users HTML5 games to play solo or to compete against each other in groups and one-on-one chats. Create games via @BotFather using the /newgame command. Please note that this kind of power requires responsibility: you will need to accept the terms for each game that your bots will be offering.
struct Games {
declare_struct
declare_field(int64_t,chat_id); /// Unique identifier for the target chat
declare_field(std::string,game_short_name); /// Short name of the game, serves as the unique identifier for the game. Set up your games via Botfather.
declare_field(std::optional<bool>,disable_notification); /// Sends the message silently. Users will receive a notification with no sound.
declare_field(std::optional<int64_t>,reply_to_message_id); /// If the message is a reply, ID of the original message
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// A JSON-serialized object for an inline keyboard. If empty, one 'Play game_title' button will be shown. If not empty, the first button must launch the game.
};
/// This object represents a game. Use BotFather to create and edit games, their short names will act as unique identifiers.
struct Game {
declare_struct
declare_field(std::string,title); /// Title of the game
declare_field(std::string,description); /// Description of the game
declare_field(std::vector<PhotoSize>,photo); /// Photo that will be displayed in the game message in chats.
declare_field(std::optional<std::string>,text); /// Optional. Brief description of the game or high scores included in the game message. Can be automatically edited to include current high scores for the game when the bot calls setGameScore, or manually edited using editMessageText. 0-4096 characters.
declare_field(std::optional<std::vector<MessageEntity>>,text_entities); /// Optional. Special entities that appear in text, such as usernames, URLs, bot commands, etc.
declare_field(std::optional<Animation>,animation); /// Optional. Animation that will be displayed in the game message in chats. Upload via BotFather
};
/// A placeholder, currently holds no information. Use BotFather to set up your game.
struct CallbackGame {
declare_struct
declare_field(int64_t,user_id); /// User identifier
declare_field(int64_t,score); /// New score, must be non-negative
declare_field(std::optional<bool>,force); /// Pass True, if the high score is allowed to decrease. This can be useful when fixing mistakes or banning cheaters
declare_field(std::optional<bool>,disable_edit_message); /// Pass True, if the game message should not be automatically edited to include the current scoreboard
declare_field(std::optional<int64_t>,chat_id); /// Required if inline_message_id is not specified. Unique identifier for the target chat
declare_field(std::optional<int64_t>,message_id); /// Required if inline_message_id is not specified. Identifier of the sent message
declare_field(std::optional<std::string>,inline_message_id); /// Required if chat_id and message_id are not specified. Identifier of the inline message
};
/// This object represents one row of the high scores table for a game.
struct GameHighScore {
declare_struct
declare_field(int64_t,position); /// Position in high score table for the game
declare_field(User,user); /// User
declare_field(int64_t,score); /// Score
};
/// This object represents a chat.
struct Chat {
declare_struct
declare_field(int64_t,id); /// Unique identifier for this chat. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it is smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
declare_field(std::string,type); /// Type of chat, can be either "private", "group", "supergroup" or "channel"
declare_field(std::optional<std::string>,title); /// Optional. Title, for supergroups, channels and group chats
declare_field(std::optional<std::string>,username); /// Optional. Username, for private chats, supergroups and channels if available
declare_field(std::optional<std::string>,first_name); /// Optional. First name of the other party in a private chat
declare_field(std::optional<std::string>,last_name); /// Optional. Last name of the other party in a private chat
declare_field(std::optional<ChatPhoto>,photo); /// Optional. Chat photo. Returned only in getChat.
declare_field(std::optional<std::string>,description); /// Optional. Description, for groups, supergroups and channel chats. Returned only in getChat.
declare_field(std::optional<std::string>,invite_link); /// Optional. Chat invite link, for groups, supergroups and channel chats. Each administrator in a chat generates their own invite links, so the bot must first generate the link using exportChatInviteLink. Returned only in getChat.
declare_field(std::optional<std::unique_ptr<Message>>,pinned_message); /// Optional. Pinned message, for groups, supergroups and channels. Returned only in getChat.
declare_field(std::optional<ChatPermissions>,permissions); /// Optional. Default chat member permissions, for groups and supergroups. Returned only in getChat.
declare_field(std::optional<int64_t>,slow_mode_delay); /// Optional. For supergroups, the minimum allowed delay between consecutive messages sent by each unpriviledged user. Returned only in getChat.
declare_field(std::optional<std::string>,sticker_set_name); /// Optional. For supergroups, name of group sticker set. Returned only in getChat.
declare_field(std::optional<bool>,can_set_sticker_set); /// Optional. True, if the bot can change the group sticker set. Returned only in getChat.
};
/// This object represents one button of the reply keyboard. For simple text buttons String can be used instead of this object to specify text of the button. Optional fields request_contact, request_location, and request_poll are mutually exclusive.
struct KeyboardButton {
declare_struct
declare_field(std::string,text); /// Text of the button. If none of the optional fields are used, it will be sent as a message when the button is pressed
declare_field(std::optional<bool>,request_contact); /// Optional. If True, the user's phone number will be sent as a contact when the button is pressed. Available in private chats only
declare_field(std::optional<bool>,request_location); /// Optional. If True, the user's current location will be sent when the button is pressed. Available in private chats only
declare_field(std::optional<KeyboardButtonPollType>,request_poll); /// Optional. If specified, the user will be asked to create a poll and send it to the bot when the button is pressed. Available in private chats only
};
/// This object represents one button of an inline keyboard. You must use exactly one of the optional fields.
struct InlineKeyboardButton {
declare_struct
declare_field(std::string,text); /// Label text on the button
declare_field(std::optional<std::string>,url); /// Optional. HTTP or tg:// url to be opened when button is pressed
declare_field(std::optional<LoginUrl>,login_url); /// Optional. An HTTP URL used to automatically authorize the user. Can be used as a replacement for the Telegram Login Widget.
declare_field(std::optional<std::string>,callback_data); /// Optional. Data to be sent in a callback query to the bot when button is pressed, 1-64 bytes
declare_field(std::optional<std::string>,switch_inline_query); /// Optional. If set, pressing the button will prompt the user to select one of their chats, open that chat and insert the bot's username and the specified inline query in the input field. Can be empty, in which case just the bot's username will be inserted.Note: This offers an easy way for users to start using your bot in inline mode when they are currently in a private chat with it. Especially useful when combined with switch_pm… actions - in this case the user will be automatically returned to the chat they switched from, skipping the chat selection screen.
declare_field(std::optional<std::string>,switch_inline_query_current_chat); /// Optional. If set, pressing the button will insert the bot's username and the specified inline query in the current chat's input field. Can be empty, in which case only the bot's username will be inserted.This offers a quick way for the user to open your bot in inline mode in the same chat - good for selecting something from multiple options.
declare_field(std::optional<CallbackGame>,callback_game); /// Optional. Description of the game that will be launched when the user presses the button.NOTE: This type of button must always be the first button in the first row.
declare_field(std::optional<bool>,pay); /// Optional. Specify True, to send a Pay button.NOTE: This type of button must always be the first button in the first row.
};
/// This object represents an incoming callback query from a callback button in an inline keyboard. If the button that originated the query was attached to a message sent by the bot, the field message will be present. If the button was attached to a message sent via the bot (in inline mode), the field inline_message_id will be present. Exactly one of the fields data or game_short_name will be present.
struct CallbackQuery {
declare_struct
declare_field(std::string,id); /// Unique identifier for this query
declare_field(User,from); /// Sender
declare_field(std::optional<std::unique_ptr<Message>>,message); /// Optional. Message with the callback button that originated the query. Note that message content and message date will not be available if the message is too old
declare_field(std::optional<std::string>,inline_message_id); /// Optional. Identifier of the message sent via the bot in inline mode, that originated the query.
declare_field(std::string,chat_instance); /// Global identifier, uniquely corresponding to the chat to which the message with the callback button was sent. Useful for high scores in games.
declare_field(std::optional<std::string>,data); /// Optional. Data associated with the callback button. Be aware that a bad client can send arbitrary data in this field.
declare_field(std::optional<std::string>,game_short_name); /// Optional. Short name of a Game to be returned, serves as the unique identifier for the game
};
/// The following methods and objects allow your bot to handle stickers and sticker sets.
struct Stickers {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,width); /// Sticker width
declare_field(int64_t,height); /// Sticker height
declare_field(bool,is_animated); /// True, if the sticker is animated
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Sticker thumbnail in the .WEBP or .JPG format
declare_field(std::optional<std::string>,emoji); /// Optional. Emoji associated with the sticker
declare_field(std::optional<std::string>,set_name); /// Optional. Name of the sticker set to which the sticker belongs
declare_field(std::optional<MaskPosition>,mask_position); /// Optional. For mask stickers, the position where the mask should be placed
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents a sticker.
struct Sticker {
declare_struct
declare_field(std::string,file_id); /// Identifier for this file, which can be used to download or reuse the file
declare_field(std::string,file_unique_id); /// Unique identifier for this file, which is supposed to be the same over time and for different bots. Can't be used to download or reuse the file.
declare_field(int64_t,width); /// Sticker width
declare_field(int64_t,height); /// Sticker height
declare_field(bool,is_animated); /// True, if the sticker is animated
declare_field(std::optional<PhotoSize>,thumb); /// Optional. Sticker thumbnail in the .WEBP or .JPG format
declare_field(std::optional<std::string>,emoji); /// Optional. Emoji associated with the sticker
declare_field(std::optional<std::string>,set_name); /// Optional. Name of the sticker set to which the sticker belongs
declare_field(std::optional<MaskPosition>,mask_position); /// Optional. For mask stickers, the position where the mask should be placed
declare_field(std::optional<int64_t>,file_size); /// Optional. File size
};
/// This object represents one result of an inline query. Telegram clients currently support results of the following 20 types:
struct InlineQueryResult {
declare_struct
declare_field(std::string,type); /// Type of the result, must be article
declare_field(std::string,id); /// Unique identifier for this result, 1-64 Bytes
declare_field(std::string,title); /// Title of the result
declare_field(InputMessageContent,input_message_content); /// Content of the message to be sent
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<std::string>,url); /// Optional. URL of the result
declare_field(std::optional<bool>,hide_url); /// Optional. Pass True, if you don't want the URL to be shown in the message
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,thumb_url); /// Optional. Url of the thumbnail for the result
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a link to an article or web page.
struct InlineQueryResultArticle {
declare_struct
declare_field(std::string,type); /// Type of the result, must be article
declare_field(std::string,id); /// Unique identifier for this result, 1-64 Bytes
declare_field(std::string,title); /// Title of the result
declare_field(InputMessageContent,input_message_content); /// Content of the message to be sent
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<std::string>,url); /// Optional. URL of the result
declare_field(std::optional<bool>,hide_url); /// Optional. Pass True, if you don't want the URL to be shown in the message
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,thumb_url); /// Optional. Url of the thumbnail for the result
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a link to a photo. By default, this photo will be sent by the user with optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the photo.
struct InlineQueryResultPhoto {
declare_struct
declare_field(std::string,type); /// Type of the result, must be photo
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,photo_url); /// A valid URL of the photo. Photo must be in jpeg format. Photo size must not exceed 5MB
declare_field(std::string,thumb_url); /// URL of the thumbnail for the photo
declare_field(std::optional<int64_t>,photo_width); /// Optional. Width of the photo
declare_field(std::optional<int64_t>,photo_height); /// Optional. Height of the photo
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the photo to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the photo caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the photo
};
/// Represents a link to an animated GIF file. By default, this animated GIF file will be sent by the user with optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the animation.
struct InlineQueryResultGif {
declare_struct
declare_field(std::string,type); /// Type of the result, must be gif
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,gif_url); /// A valid URL for the GIF file. File size must not exceed 1MB
declare_field(std::optional<int64_t>,gif_width); /// Optional. Width of the GIF
declare_field(std::optional<int64_t>,gif_height); /// Optional. Height of the GIF
declare_field(std::optional<int64_t>,gif_duration); /// Optional. Duration of the GIF
declare_field(std::string,thumb_url); /// URL of the static (JPEG or GIF) or animated (MPEG4) thumbnail for the result
declare_field(std::optional<std::string>,thumb_mime_type); /// Optional. MIME type of the thumbnail, must be one of "image/jpeg", "image/gif", or "video/mp4". Defaults to "image/jpeg"
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the GIF file to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the GIF animation
};
/// Represents a link to a video animation (H.264/MPEG-4 AVC video without sound). By default, this animated MPEG-4 file will be sent by the user with optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the animation.
struct InlineQueryResultMpeg4Gif {
declare_struct
declare_field(std::string,type); /// Type of the result, must be mpeg4_gif
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,mpeg4_url); /// A valid URL for the MP4 file. File size must not exceed 1MB
declare_field(std::optional<int64_t>,mpeg4_width); /// Optional. Video width
declare_field(std::optional<int64_t>,mpeg4_height); /// Optional. Video height
declare_field(std::optional<int64_t>,mpeg4_duration); /// Optional. Video duration
declare_field(std::string,thumb_url); /// URL of the static (JPEG or GIF) or animated (MPEG4) thumbnail for the result
declare_field(std::optional<std::string>,thumb_mime_type); /// Optional. MIME type of the thumbnail, must be one of "image/jpeg", "image/gif", or "video/mp4". Defaults to "image/jpeg"
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the MPEG-4 file to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the video animation
};
/// Represents a link to a page containing an embedded video player or a video file. By default, this video file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the video.
struct InlineQueryResultVideo {
declare_struct
declare_field(std::string,type); /// Type of the result, must be video
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,video_url); /// A valid URL for the embedded video player or video file
declare_field(std::string,mime_type); /// Mime type of the content of video url, "text/html" or "video/mp4"
declare_field(std::string,thumb_url); /// URL of the thumbnail (jpeg only) for the video
declare_field(std::string,title); /// Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the video to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the video caption. See formatting options for more details.
declare_field(std::optional<int64_t>,video_width); /// Optional. Video width
declare_field(std::optional<int64_t>,video_height); /// Optional. Video height
declare_field(std::optional<int64_t>,video_duration); /// Optional. Video duration in seconds
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the video. This field is required if InlineQueryResultVideo is used to send an HTML-page as a result (e.g., a YouTube video).
};
/// Represents a link to an MP3 audio file. By default, this audio file will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the audio.
struct InlineQueryResultAudio {
declare_struct
declare_field(std::string,type); /// Type of the result, must be audio
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,audio_url); /// A valid URL for the audio file
declare_field(std::string,title); /// Title
declare_field(std::optional<std::string>,caption); /// Optional. Caption, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the audio caption. See formatting options for more details.
declare_field(std::optional<std::string>,performer); /// Optional. Performer
declare_field(std::optional<int64_t>,audio_duration); /// Optional. Audio duration in seconds
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the audio
};
/// Represents a link to a voice recording in an .OGG container encoded with OPUS. By default, this voice recording will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the the voice message.
struct InlineQueryResultVoice {
declare_struct
declare_field(std::string,type); /// Type of the result, must be voice
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,voice_url); /// A valid URL for the voice recording
declare_field(std::string,title); /// Recording title
declare_field(std::optional<std::string>,caption); /// Optional. Caption, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the voice message caption. See formatting options for more details.
declare_field(std::optional<int64_t>,voice_duration); /// Optional. Recording duration in seconds
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the voice recording
};
/// Represents a link to a file. By default, this file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the file. Currently, only .PDF and .ZIP files can be sent using this method.
struct InlineQueryResultDocument {
declare_struct
declare_field(std::string,type); /// Type of the result, must be document
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,title); /// Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the document to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the document caption. See formatting options for more details.
declare_field(std::string,document_url); /// A valid URL for the file
declare_field(std::string,mime_type); /// Mime type of the content of the file, either "application/pdf" or "application/zip"
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the file
declare_field(std::optional<std::string>,thumb_url); /// Optional. URL of the thumbnail (jpeg only) for the file
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a location on a map. By default, the location will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the location.
struct InlineQueryResultLocation {
declare_struct
declare_field(std::string,type); /// Type of the result, must be location
declare_field(std::string,id); /// Unique identifier for this result, 1-64 Bytes
declare_field(float,latitude); /// Location latitude in degrees
declare_field(float,longitude); /// Location longitude in degrees
declare_field(std::string,title); /// Location title
declare_field(std::optional<int64_t>,live_period); /// Optional. Period in seconds for which the location can be updated, should be between 60 and 86400.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the location
declare_field(std::optional<std::string>,thumb_url); /// Optional. Url of the thumbnail for the result
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a venue. By default, the venue will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the venue.
struct InlineQueryResultVenue {
declare_struct
declare_field(std::string,type); /// Type of the result, must be venue
declare_field(std::string,id); /// Unique identifier for this result, 1-64 Bytes
declare_field(float,latitude); /// Latitude of the venue location in degrees
declare_field(float,longitude); /// Longitude of the venue location in degrees
declare_field(std::string,title); /// Title of the venue
declare_field(std::string,address); /// Address of the venue
declare_field(std::optional<std::string>,foursquare_id); /// Optional. Foursquare identifier of the venue if known
declare_field(std::optional<std::string>,foursquare_type); /// Optional. Foursquare type of the venue, if known. (For example, "arts_entertainment/default", "arts_entertainment/aquarium" or "food/icecream".)
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the venue
declare_field(std::optional<std::string>,thumb_url); /// Optional. Url of the thumbnail for the result
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a contact with a phone number. By default, this contact will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the contact.
struct InlineQueryResultContact {
declare_struct
declare_field(std::string,type); /// Type of the result, must be contact
declare_field(std::string,id); /// Unique identifier for this result, 1-64 Bytes
declare_field(std::string,phone_number); /// Contact's phone number
declare_field(std::string,first_name); /// Contact's first name
declare_field(std::optional<std::string>,last_name); /// Optional. Contact's last name
declare_field(std::optional<std::string>,vcard); /// Optional. Additional data about the contact in the form of a vCard, 0-2048 bytes
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the contact
declare_field(std::optional<std::string>,thumb_url); /// Optional. Url of the thumbnail for the result
declare_field(std::optional<int64_t>,thumb_width); /// Optional. Thumbnail width
declare_field(std::optional<int64_t>,thumb_height); /// Optional. Thumbnail height
};
/// Represents a link to a photo stored on the Telegram servers. By default, this photo will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the photo.
struct InlineQueryResultCachedPhoto {
declare_struct
declare_field(std::string,type); /// Type of the result, must be photo
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,photo_file_id); /// A valid file identifier of the photo
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the photo to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the photo caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the photo
};
/// Represents a link to an animated GIF file stored on the Telegram servers. By default, this animated GIF file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with specified content instead of the animation.
struct InlineQueryResultCachedGif {
declare_struct
declare_field(std::string,type); /// Type of the result, must be gif
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,gif_file_id); /// A valid file identifier for the GIF file
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the GIF file to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the GIF animation
};
/// Represents a link to a video animation (H.264/MPEG-4 AVC video without sound) stored on the Telegram servers. By default, this animated MPEG-4 file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the animation.
struct InlineQueryResultCachedMpeg4Gif {
declare_struct
declare_field(std::string,type); /// Type of the result, must be mpeg4_gif
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,mpeg4_file_id); /// A valid file identifier for the MP4 file
declare_field(std::optional<std::string>,title); /// Optional. Title for the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the MPEG-4 file to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the video animation
};
/// Represents a link to a sticker stored on the Telegram servers. By default, this sticker will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the sticker.
struct InlineQueryResultCachedSticker {
declare_struct
declare_field(std::string,type); /// Type of the result, must be sticker
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,sticker_file_id); /// A valid file identifier of the sticker
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the sticker
};
/// Represents a link to a file stored on the Telegram servers. By default, this file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the file.
struct InlineQueryResultCachedDocument {
declare_struct
declare_field(std::string,type); /// Type of the result, must be document
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,title); /// Title for the result
declare_field(std::string,document_file_id); /// A valid file identifier for the file
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the document to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the document caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the file
};
/// Represents a link to a video file stored on the Telegram servers. By default, this video file will be sent by the user with an optional caption. Alternatively, you can use input_message_content to send a message with the specified content instead of the video.
struct InlineQueryResultCachedVideo {
declare_struct
declare_field(std::string,type); /// Type of the result, must be video
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,video_file_id); /// A valid file identifier for the video file
declare_field(std::string,title); /// Title for the result
declare_field(std::optional<std::string>,description); /// Optional. Short description of the result
declare_field(std::optional<std::string>,caption); /// Optional. Caption of the video to be sent, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the video caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the video
};
/// Represents a link to a voice message stored on the Telegram servers. By default, this voice message will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the voice message.
struct InlineQueryResultCachedVoice {
declare_struct
declare_field(std::string,type); /// Type of the result, must be voice
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,voice_file_id); /// A valid file identifier for the voice message
declare_field(std::string,title); /// Voice message title
declare_field(std::optional<std::string>,caption); /// Optional. Caption, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the voice message caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the voice message
};
/// Represents a link to an MP3 audio file stored on the Telegram servers. By default, this audio file will be sent by the user. Alternatively, you can use input_message_content to send a message with the specified content instead of the audio.
struct InlineQueryResultCachedAudio {
declare_struct
declare_field(std::string,type); /// Type of the result, must be audio
declare_field(std::string,id); /// Unique identifier for this result, 1-64 bytes
declare_field(std::string,audio_file_id); /// A valid file identifier for the audio file
declare_field(std::optional<std::string>,caption); /// Optional. Caption, 0-1024 characters after entities parsing
declare_field(std::optional<std::string>,parse_mode); /// Optional. Mode for parsing entities in the audio caption. See formatting options for more details.
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message
declare_field(std::optional<InputMessageContent>,input_message_content); /// Optional. Content of the message to be sent instead of the audio
};
/// Contains information about Telegram Passport data shared with the bot by the user.
struct PassportData {
declare_struct
declare_field(std::vector<EncryptedPassportElement>,data); /// Array with information about documents and other Telegram Passport elements that was shared with the bot
declare_field(EncryptedCredentials,credentials); /// Encrypted credentials required to decrypt the data
};
/// This object represents a message.
struct Message {
declare_struct
declare_field(int64_t,message_id); /// Unique message identifier inside this chat
declare_field(std::optional<User>,from); /// Optional. Sender, empty for messages sent to channels
declare_field(int64_t,date); /// Date the message was sent in Unix time
declare_field(Chat,chat); /// Conversation the message belongs to
declare_field(std::optional<User>,forward_from); /// Optional. For forwarded messages, sender of the original message
declare_field(std::optional<Chat>,forward_from_chat); /// Optional. For messages forwarded from channels, information about the original channel
declare_field(std::optional<int64_t>,forward_from_message_id); /// Optional. For messages forwarded from channels, identifier of the original message in the channel
declare_field(std::optional<std::string>,forward_signature); /// Optional. For messages forwarded from channels, signature of the post author if present
declare_field(std::optional<std::string>,forward_sender_name); /// Optional. Sender's name for messages forwarded from users who disallow adding a link to their account in forwarded messages
declare_field(std::optional<int64_t>,forward_date); /// Optional. For forwarded messages, date the original message was sent in Unix time
declare_field(std::optional<std::unique_ptr<Message>>,reply_to_message); /// Optional. For replies, the original message. Note that the Message object in this field will not contain further reply_to_message fields even if it itself is a reply.
declare_field(std::optional<User>,via_bot); /// Optional. Bot through which the message was sent
declare_field(std::optional<int64_t>,edit_date); /// Optional. Date the message was last edited in Unix time
declare_field(std::optional<std::string>,media_group_id); /// Optional. The unique identifier of a media message group this message belongs to
declare_field(std::optional<std::string>,author_signature); /// Optional. Signature of the post author for messages in channels
declare_field(std::optional<std::string>,text); /// Optional. For text messages, the actual UTF-8 text of the message, 0-4096 characters
declare_field(std::optional<std::vector<MessageEntity>>,entities); /// Optional. For text messages, special entities like usernames, URLs, bot commands, etc. that appear in the text
declare_field(std::optional<Animation>,animation); /// Optional. Message is an animation, information about the animation. For backward compatibility, when this field is set, the document field will also be set
declare_field(std::optional<Audio>,audio); /// Optional. Message is an audio file, information about the file
declare_field(std::optional<Document>,document); /// Optional. Message is a general file, information about the file
declare_field(std::optional<std::vector<PhotoSize>>,photo); /// Optional. Message is a photo, available sizes of the photo
declare_field(std::optional<Sticker>,sticker); /// Optional. Message is a sticker, information about the sticker
declare_field(std::optional<Video>,video); /// Optional. Message is a video, information about the video
declare_field(std::optional<VideoNote>,video_note); /// Optional. Message is a video note, information about the video message
declare_field(std::optional<Voice>,voice); /// Optional. Message is a voice message, information about the file
declare_field(std::optional<std::string>,caption); /// Optional. Caption for the animation, audio, document, photo, video or voice, 0-1024 characters
declare_field(std::optional<std::vector<MessageEntity>>,caption_entities); /// Optional. For messages with a caption, special entities like usernames, URLs, bot commands, etc. that appear in the caption
declare_field(std::optional<Contact>,contact); /// Optional. Message is a shared contact, information about the contact
declare_field(std::optional<Dice>,dice); /// Optional. Message is a dice with random value from 1 to 6
declare_field(std::optional<Game>,game); /// Optional. Message is a game, information about the game. More about games »
declare_field(std::optional<Poll>,poll); /// Optional. Message is a native poll, information about the poll
declare_field(std::optional<Venue>,venue); /// Optional. Message is a venue, information about the venue. For backward compatibility, when this field is set, the location field will also be set
declare_field(std::optional<Location>,location); /// Optional. Message is a shared location, information about the location
declare_field(std::optional<std::vector<User>>,new_chat_members); /// Optional. New members that were added to the group or supergroup and information about them (the bot itself may be one of these members)
declare_field(std::optional<User>,left_chat_member); /// Optional. A member was removed from the group, information about them (this member may be the bot itself)
declare_field(std::optional<std::string>,new_chat_title); /// Optional. A chat title was changed to this value
declare_field(std::optional<std::vector<PhotoSize>>,new_chat_photo); /// Optional. A chat photo was change to this value
declare_field(std::optional<bool>,delete_chat_photo); /// Optional. Service message: the chat photo was deleted
declare_field(std::optional<bool>,group_chat_created); /// Optional. Service message: the group has been created
declare_field(std::optional<bool>,supergroup_chat_created); /// Optional. Service message: the supergroup has been created. This field can't be received in a message coming through updates, because bot can't be a member of a supergroup when it is created. It can only be found in reply_to_message if someone replies to a very first message in a directly created supergroup.
declare_field(std::optional<bool>,channel_chat_created); /// Optional. Service message: the channel has been created. This field can't be received in a message coming through updates, because bot can't be a member of a channel when it is created. It can only be found in reply_to_message if someone replies to a very first message in a channel.
declare_field(std::optional<int64_t>,migrate_to_chat_id); /// Optional. The group has been migrated to a supergroup with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it is smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
declare_field(std::optional<int64_t>,migrate_from_chat_id); /// Optional. The supergroup has been migrated from a group with the specified identifier. This number may be greater than 32 bits and some programming languages may have difficulty/silent defects in interpreting it. But it is smaller than 52 bits, so a signed 64 bit integer or double-precision float type are safe for storing this identifier.
declare_field(std::optional<std::unique_ptr<Message>>,pinned_message); /// Optional. Specified message was pinned. Note that the Message object in this field will not contain further reply_to_message fields even if it is itself a reply.
declare_field(std::optional<Invoice>,invoice); /// Optional. Message is an invoice for a payment, information about the invoice. More about payments »
declare_field(std::optional<SuccessfulPayment>,successful_payment); /// Optional. Message is a service message about a successful payment, information about the payment. More about payments »
declare_field(std::optional<std::string>,connected_website); /// Optional. The domain name of the website on which the user has logged in. More about Telegram Login »
declare_field(std::optional<PassportData>,passport_data); /// Optional. Telegram Passport data
declare_field(std::optional<InlineKeyboardMarkup>,reply_markup); /// Optional. Inline keyboard attached to the message. login_url buttons are represented as ordinary url buttons.
};
/// This object represents an incoming update.At most one of the optional parameters can be present in any given update.
struct Update {
declare_struct
declare_field(int64_t,update_id); /// The update's unique identifier. Update identifiers start from a certain positive number and increase sequentially. This ID becomes especially handy if you're using Webhooks, since it allows you to ignore repeated updates or to restore the correct update sequence, should they get out of order. If there are no new updates for at least a week, then identifier of the next update will be chosen randomly instead of sequentially.
declare_field(std::optional<Message>,message); /// Optional. New incoming message of any kind - text, photo, sticker, etc.
declare_field(std::optional<Message>,edited_message); /// Optional. New version of a message that is known to the bot and was edited
declare_field(std::optional<Message>,channel_post); /// Optional. New incoming channel post of any kind - text, photo, sticker, etc.
declare_field(std::optional<Message>,edited_channel_post); /// Optional. New version of a channel post that is known to the bot and was edited
declare_field(std::optional<InlineQuery>,inline_query); /// Optional. New incoming inline query
declare_field(std::optional<ChosenInlineResult>,chosen_inline_result); /// Optional. The result of an inline query that was chosen by a user and sent to their chat partner. Please see our documentation on the feedback collecting for details on how to enable these updates for your bot.
declare_field(std::optional<CallbackQuery>,callback_query); /// Optional. New incoming callback query
declare_field(std::optional<ShippingQuery>,shipping_query); /// Optional. New incoming shipping query. Only for invoices with flexible price
declare_field(std::optional<PreCheckoutQuery>,pre_checkout_query); /// Optional. New incoming pre-checkout query. Contains full information about checkout
declare_field(std::optional<Poll>,poll); /// Optional. New poll state. Bots receive only updates about stopped polls and polls, which are sent by the bot
declare_field(std::optional<PollAnswer>,poll_answer); /// Optional. A user changed their answer in a non-anonymous poll. Bots receive new votes only in polls that were sent by the bot itself.
};
}
