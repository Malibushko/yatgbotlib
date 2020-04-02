#pragma once
#include "networkmanager.h"
#include "querybuilder.h"
#include "utility/utility.h"

namespace telegram {
using namespace traits;
using name_value_pair = std::pair<std::string_view, std::string>;

namespace utility {
    bool lowercase_compare(std::string_view a, std::string_view b)
    {
        return std::equal(a.begin(), a.end(),
                          b.begin(), b.end(),
                          [](char a, char b) {
                              return tolower(a) == tolower(b);
                          });
    }

    std::string fileBytes(const std::string &filePath) {
        std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) {
            utility::logger::info("File ",filePath, "is empty");
            return {};
        }
        size_t fileSize = fs::file_size(filePath);
        std::vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }
    struct Error {
        int32_t error_code;
        std::string description;
        template<typename IStream>
        friend std::ostream& operator<<(IStream& os,const Error & e) {
            os << e.toString();
            return  os;
        }
        std::string toString() const {
            return {"[Error]: " + std::to_string(error_code) + ' '
                        + (description.size() ? description : " no description")};
        }
    };

}
using utility::Error;

class ApiManager {
  std::string base_url;
  NetworkManager m_manager{"api.telegram.org"};

private:
  template <class T> void assignFromJson(T &value, const std::string &data) const {
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
  template <class T, class AssignType>
  void assignFromJson(T &value, const std::string &data) const {
    AssignType temp_val{};
    assignFromJson<AssignType>(temp_val, data);
    value = std::move(temp_val);
  }
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

  template <class T>
  std::pair<T, std::optional<Error>> ApiCall(const char *api,
                                             const QueryBuilder &builder) {
    std::shared_ptr<httplib::Response> reply =
        m_manager.post(base_url + api, {}, builder.getQuery());

    if (!reply) {
      return {T{}, Error{600, "Unable to make a request"}};
    }
    std::pair<T, std::optional<Error>> result;

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      assignFromJson<T>(result.first, value);
    } else {
      result.second = Error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }

  template <class T, class TrueOrType>
  std::pair<T, std::optional<Error>> ApiCall(const char *api,
                                             const QueryBuilder &builder) {
    auto reply = m_manager.post(base_url + api, {}, builder.getQuery());

    std::pair<T, std::optional<Error>> result;

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      if (utility::lowercase_compare(value, utility::true_literal.data())) {
        result.first = true;
      } else if (utility::lowercase_compare(value, utility::false_literal.data())) {
        result.first = false;
      } else {
        assignFromJson<T, TrueOrType>(result.first, value);
      }
    } else {
      result.second = Error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }

  template <class T>
  std::pair<T, std::optional<Error>> ApiCall(const char *api) {
    auto reply = m_manager.post(base_url + api);

    std::pair<T, std::optional<Error>> result;

    auto &&[is_valid, value] = parseWithError(reply->body);
    if (is_valid) {
      assignFromJson<T>(result.first, value);
    } else {
      result.second = Error{0, value};
    }
    return result;
  }

  template <class T>
  std::pair<T, std::optional<Error>>
  ApiCall(const char *api, QueryBuilder &builder,
          const std::vector<name_value_pair> &params) {
    std::pair<T, std::optional<Error>> result;
    std::string reply;
    int status_code = 0;
    // check if there is a valid path to local file
    if (std::any_of(params.begin(), params.end(),
                    [](const name_value_pair &value) {
                      return fs::exists(fs::path(value.second));
                    })) {
      // rewrite the doc
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
                           it->value.GetBool() ? "true" : "false",
                           {}});
        else if (it->value.IsString())
          items.push_back({it->name.GetString(), it->value.GetString()});
      }

      for (auto &&[name, path_or_id] : params) {
        if (auto path = fs::path(path_or_id); fs::exists(path)) {
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
          0,
          std::string("Id or filepath is not valid for at least one file at ") +
              __func__};
      return result;
    }

    auto &&[is_valid, value] = parseWithError(reply);
    if (is_valid) {
      assignFromJson<T>(result.first, value);
    } else {
      result.second = Error{static_cast<int32_t>(status_code), value};
    }
    return result;
  }
  std::string ApiCallRaw(const char *api, const QueryBuilder &builder) {
    return m_manager.post(base_url + api, {}, builder.getQuery())->body;
  }
};
} // namespace telegram
