#pragma once
#include "networkmanager.h"
#include "querybuilder.h"
#include "utility/utility.h"

namespace telegram {
using namespace traits;
using name_value_pair = std::pair<std::string_view, std::string>;

namespace utility {
    /**
     * Compare two string_views case insensitive
     * @param a first string to be compared
     * @param b second string to be compared
     * \return true if strings are equal, false otherwise
     */
    bool lowercase_compare(std::string_view a, std::string_view b)
    {
        return std::equal(a.begin(), a.end(),
                          b.begin(), b.end(),
                          [](char a, char b) {
                              return tolower(a) == tolower(b);
                          });
    }
    /**
     * Read file in binary format and returns std::string object
     * which represent content of the file
     * @param path to file
     * \warning empty string is returned if path is not valid
     * \return byte representation of file
     */
    std::string fileBytes(const std::string &filePath) {
        std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) {
            utility::Logger::info("File ",filePath, "is empty");
            return {};
        }
        size_t fileSize = fs::file_size(filePath);
        std::vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }
    /**
     * @brief Struct represents error
     * Struct that contain error code to indicate the problem
     * and description
     * Has overloaded operator<< for writing objects to istreams
     */
    struct Error {
        /// code that represents the problem, may include HTTP codes and TGLib error codes
        int32_t error_code;
        /// description of error if available
        std::string description;
        template<typename IStream>
        friend std::ostream& operator<<(IStream& os,const Error & e) {
            os << e.toString();
            return  os;
        }
        /// returns string representation of error
        std::string toString() const {
            return {"[Error]: " + std::to_string(error_code) + ' '
                        + (description.size() ? description : " no description")};
        }
    };

}
using utility::Error;
/**
 * @brief Class that implements request to telegram api
 */
class ApiManager {
  /// url that will be prepended to each request
  std::string base_url;
  NetworkManager m_manager{"api.telegram.org"};

private:
  /**
   * Function assigns value from string (not only JSON) based on value type
   * @param value - Value to be assigned
   * @param data - JSON string containing data
   */
  template <class T> void assignValue(T &value, const std::string &data) const {
    if constexpr (traits::is_string_type<T>)
      value = data;
    else if constexpr (traits::is_parsable_v<T> || traits::is_container_v<T>)
      value = std::move(JsonParser::i().fromJson<T>(data));
    else if constexpr (std::is_same_v<bool, std::decay_t<T>>)
      value = (utility::lowercase_compare(data, "true") ? true : false);
    else if constexpr (std::is_integral_v<T>)
      value = std::stoi(data);
    else if constexpr (std::is_floating_point_v<T>)
      value = std::stof(data);
  }
  /**
   * Function assigns value of type AssignType
   * but accepts any type
   * It is when value in reply is not equal to object value (like assinging to std::variant)
   */
  template <class T, class AssignType>
  void assignValue(T &value, const std::string &data) const {
    AssignType temp_val{};
    assignValue<AssignType>(temp_val, data);
    value = std::move(temp_val);
  }
  /**
    This function parses telegram reply (see telegram documentation)
    end returns pair of error flag and json string that contains data (e.g 'result' part)
    @param view Teleram Bot Api reply
    @return pair of boolean indicating error and JSON value from 'description' response part
   */
  std::pair<bool,std::string> parseWithError(std::string_view view) {
      rapidjson::Document doc;
      if (!view.size()) {
          return {false,{"Empty or not valid json"}};
      }
      doc.Parse(view.data(),view.length());
      if (doc["ok"].GetBool()) {
          if (auto& val = doc["result"];val.IsArray())
              return {true,JsonParser::i().rapidArrayToJson(val.GetArray())};
          else if (val.IsObject())
              return {true,JsonParser::i().rapidObjectToJson(val.GetObject())};
          else if (val.IsNumber()) {
              return {true,std::to_string(val.GetInt())};
          } else if (val.IsString()) {
              return {true,val.GetString()};
          } else if (val.IsFloat()) {
              return {true,std::to_string(val.GetFloat())};
          } else if (val.IsTrue()) {
              return {true,"true"};
          } else if (val.IsFalse()) {
              return {true,"false"};
          } else {
              assert("Undefined value");
          }
      } else {
          return {false,doc["description"].GetString()};
      }
  }

public:
  ApiManager() {}
  ApiManager(std::string &&url) noexcept : base_url{std::move(url)} {}

  /**
   * Overloaded function that accepts name of API method and QueryBuilder
   * that contains arguments neccessary for the call
   * @param api - Telegram Bot Api method name
   * @param builder - QueryBuilder that contains data
   * @return Pair of Error (if available) and value
   */
  template <class T>
  std::pair<T, std::optional<Error>> ApiCall(const char *api,
                                             const QueryBuilder &builder) {
    std::shared_ptr<httplib::Response> reply =
        m_manager.post(base_url + api, {}, builder.getQuery());

    if (!reply) {
      return {T{}, Error{static_cast<uint32_t>(ErrorCodes::UnableToMakeRequest),
                      "Unable to make a request"}};
    }
    std::pair<T, std::optional<Error>> result;

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      assignValue<T>(result.first, value);
    } else {
      result.second = Error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }
  /**
   * Overloaded function that accepts name of API method and QueryBuilder
   * that contains arguments neccessary for the call. This overload has
   * two template parameters and is used then function can have two different
   * returning types (everywhere is True or Value).
   * If value is not True than a Value is returned, otherwise Error is returned;
   * @param api - Telegram Bot Api method name
   * @param builder - QueryBuilder that contains data
   * @return Pair of Error (if available) and value
   */
  template <class T, class TrueOrType>
  std::pair<T, std::optional<Error>> ApiCall(const char *api,
                                             const QueryBuilder &builder) {
    auto reply = m_manager.post(base_url + api, {}, builder.getQuery());
    if (!reply) {
      return {T{}, Error{static_cast<uint32_t>(ErrorCodes::UnableToMakeRequest),
                      "Unable to make a request"}};
    }

    std::pair<T, std::optional<Error>> result;

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      if (utility::lowercase_compare(value, utility::true_literal.data())) {
        result.first = true;
      } else if (utility::lowercase_compare(value, utility::false_literal.data())) {
        result.first = false;
      } else {
        assignValue<T, TrueOrType>(result.first, value);
      }
    } else {
      result.second = Error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }
  /**
   * @brief Call to Telegram bot API without arguments
   * This overload just call API method and returns result
   * @param api - Telegram Bot Api method name
   * @return Pair of Error (if available) and value
   */
  template <class T>
  std::pair<T, std::optional<Error>> ApiCall(const char *api) {
    auto reply = m_manager.post(base_url + api);

    std::pair<T, std::optional<Error>> result;
    if (!reply) {
      return {T{}, Error{static_cast<uint32_t>(ErrorCodes::UnableToMakeRequest),
                      "Unable to make a request"}};
    }

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      assignValue<T>(result.first, value);
    } else {
      result.second = Error{static_cast<uint32_t>(ErrorCodes::InvalidReply),
                        value};
    }
    return result;
  }
  /**
   * This overload is used to send multipart requests (e.g when it is neccessary to send some files)
   * @param api - Telegram Bot API method name
   * @param builder - QueryBuilder that contains data 
   * @param params - Container of paths to files (either absolute or relative)
   * @return Pair of Error (if available) and value
   */
  template <class T>
  std::pair<T, std::optional<Error>>
  ApiCall(const char *api, QueryBuilder &builder,
          const std::vector<name_value_pair> &params) {
    std::pair<T, std::optional<Error>> result;
    std::string reply;
    int status_code = 0;

    /// check if there is a valid path to local file
    if (std::any_of(params.begin(), params.end(),
                    [](const name_value_pair &value) {
                      return fs::exists(fs::path(value.second));
                    })) {
      /// rewrite the doc to httplib multipart object type
      std::vector<httplib::MultipartFormData> items;
      const auto &build_doc = builder.getDocument().GetObject();
      items.reserve(static_cast<size_t>(
          std::distance(build_doc.begin(), build_doc.end())));

      for (auto it = build_doc.begin(); it != build_doc.end(); ++it) {
        if (it->value.IsInt() || it->value.IsInt64())
          items.push_back(
              {it->name.GetString(), std::to_string(it->value.GetInt64()), {}});
        else if (it->value.IsBool())
          items.push_back({it->name.GetString(),
                           it->value.GetBool() ? utility::true_literal.data() :
                                                 utility::false_literal.data(),
                           {}});
        else if (it->value.IsString())
          items.push_back({it->name.GetString(), it->value.GetString()});
      }

      for (auto &&[name, path_or_id] : params) {
        if (auto path = fs::path(path_or_id); fs::exists(path)) {
            /// check if value is certificate or key file
            /// telegram has additional requirements in that case (prepend '@' to filename)
          if (path.filename().string().find(".pem") != std::string::npos)
            items.push_back({"certificate",
                             utility::fileBytes(fs::absolute(path)),
                             '@' + path.string()});
          else
            items.push_back({name.data(),
                             utility::fileBytes(fs::absolute(path)),
                             path.filename().string()});
        } else {
          items.push_back({{name.data(), name.size()}, path_or_id});
        }
      }
      auto response = m_manager.post(base_url + api, items);
      reply = response->body;
      status_code = response->status;
    } else if (params.size()) {
      for (auto &&it : params) {
        builder << it;
      }
      auto response = m_manager.post(base_url + api, {}, builder.getQuery());
      reply = response->body;
      status_code = response->status;
    } else {
      result.second = Error{
              static_cast<int32_t>(ErrorCodes::NotValid),
          std::string("Id or filepath is not valid for at least one file at ") +
              __func__};
      return result;
    }

    auto &&[is_valid, value] = parseWithError(reply);
    if (is_valid) {
      assignValue<T>(result.first, value);
    } else {
      result.second = Error{static_cast<int32_t>(status_code), value};
    }
    return result;
  }
  /**
   * This function calls Telegram Bot Api and returns result with no processing made
   * @param api - Telegram Bot API method name
   * @param builder - QueryBuilder that contains data
   * @return std::string containing HTTP response body
   */
  std::string ApiCallRaw(const char *api, const QueryBuilder &builder) {
    return m_manager.post(base_url + api, {}, builder.getQuery())->body;
  }
};
} // namespace telegram
