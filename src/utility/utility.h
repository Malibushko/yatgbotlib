#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <map>
#    if __has_include(<filesystem>)
#        include <filesystem>
namespace fs = std::filesystem;
#else
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif
#ifdef WIN32
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#endif

#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include "headers/json_parser.h"
#include "logger.h"

namespace telegram {
/// HTTP request content types
enum class ContentTypes {
    text_plain,
    application_json,
    multipart_form_data,
    application_octet_stream
};
/// parse_mode for Telegram Bot Api
enum class ParseMode {
    Markdown,
    HTML
};
/// chat_action for Telegram Bot Api
enum class ChatAction {
    Typing,
    UploadPhoto,
    UploadVideo,
    RecordVideo,
    RecordAudio,
    UploadAudio,
    UploadDocument,
    FindLocation,
    RecordVideoNote,
    UploadVideoNote
};
/// Library error codes
enum class ErrorCodes {
    UnableToMakeRequest = 1000, /// httplib was not able to make a request
    NotValid, /// not valid parameters
    InvalidReply /// reply did not contain any value
};
}

namespace telegram::utility {
#ifndef make_named_pair
#define make_named_pair(value) std::pair<std::string_view,std::decay_t<decltype(value)>>{#value,value}
#endif

inline constexpr std::string_view toString(ContentTypes value) {
    switch (value) {
    case ContentTypes::text_plain:
        return "text/plain";
    case ContentTypes::application_json:
        return "application/json";
    case ContentTypes::multipart_form_data:
        return "multipart/form-data";
    case ContentTypes::application_octet_stream:
        return "application/octet-stream";
    default:
        return "";
    }
}
inline constexpr std::string_view toString(ParseMode value) {
    switch (value) {
    case ParseMode::Markdown:
        return "Markdown";
    case ParseMode::HTML:
        return "HTML";
    default:
        return "";
    }
}
inline constexpr std::string_view toString(ChatAction value) {
    switch (value) {
    case ChatAction::UploadAudio:
        return "upload_audio";
    case ChatAction::UploadPhoto:
        return "upload_photo";
    case ChatAction::UploadDocument:
        return "upload_document";
    case ChatAction::RecordVideo:
        return "record_video";
    case ChatAction::Typing:
        return "typing";
    case ChatAction::RecordAudio:
        return "record_audio";
    case ChatAction::FindLocation:
        return "find_location";
    case ChatAction::RecordVideoNote:
        return "record_video_note";
    case ChatAction::UploadVideoNote:
        return "upload_video_note";
    case ChatAction::UploadVideo:
        return "upload_video";
    default:
        return "";
    }
}

constexpr uint32_t telegram_first_subnet_range_begin = 2509938689;
constexpr uint32_t telegram_first_subnet_range_end = 2509942782;
constexpr uint32_t teleram_second_subnet_range_begin = 1533805569;
constexpr uint32_t telegram_second_subned_range_end = 1533806590;

constexpr std::string_view true_literal = "true";
constexpr std::string_view false_literal = "false";
}
