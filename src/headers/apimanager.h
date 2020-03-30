#pragma once
#include "networkmanager.h"
#include "querybuilder.h"
#include "utility/traits.h"

namespace telegram {
using namespace traits;
using name_value_pair = std::pair<std::string_view, std::string>;
using utility::error;

class api_manager {
  std::string base_url;

private:
  template <class T> void assign_impl(T &value, const std::string &data) const {
    if constexpr (traits::is_string_type<T>)
      value = data;
    else if constexpr (traits::is_parsable_v<T> || traits::is_container_v<T>)
      value = std::move(from_json<T>(data));
    else if constexpr (std::is_same_v<bool, std::decay_t<T>>)
      value = (utility::lowercase_compare(data, "true") ? true : false);
    else if constexpr (std::is_integral_v<T>)
      value = std::stoi(data);
    else if constexpr (std::is_floating_point_v<T>)
      value = std::stof(data);
  }
  template <class T, class AssignType>
  void assign_impl(T &value, const std::string &data) const {
    AssignType temp_val{};
    assign_impl<AssignType>(temp_val, data);
    value = std::move(temp_val);
  }

public:
  api_manager() noexcept {}
  api_manager(std::string &&url) noexcept : base_url{std::move(url)} {}

  template <class T>
  std::pair<T, std::optional<error>> call_api(const char *api,
                                              const query_builder &builder) {
    std::shared_ptr<httplib::Response> reply =
        network_manager::i().post(base_url + api, {}, builder.get_query());

    if (!reply) {
      return {T{}, error{600, "Unable to make a request"}};
    }
    std::pair<T, std::optional<error>> result;

    auto &&[is_valid, value] = utility::parse_value(reply->body);
    if (is_valid) {
      assign_impl<T>(result.first, value);
    } else {
      result.second = error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }
  template <class T, class TrueOrType>
  std::pair<T, std::optional<error>>
  call_api(const char *api, const query_builder &builder) const {
    auto reply =
        network_manager::i().post(base_url + api, {}, builder.get_query());

    std::pair<T, std::optional<error>> result;

    auto &&[is_valid, value] = utility::parse_value(reply->body);
    if (is_valid) {
      if (utility::lowercase_compare(value, utility::true_literal)) {
        result.first = true;
      } else if (utility::lowercase_compare(value, utility::false_literal)) {
        result.first = false;
      } else {
        assign_impl<T, TrueOrType>(result.first, value);
      }
    } else {
      result.second = error{static_cast<int32_t>(reply->status), value};
    }
    return result;
  }

  template <class T>
  std::pair<T, std::optional<error>> call_api(const char *api) const {
    auto reply = network_manager::i().post(base_url + api);

    std::pair<T, std::optional<error>> result;

    auto &&[is_valid, value] = utility::parse_value(reply->body);
    if (is_valid) {
      assign_impl<T>(result.first, value);
    } else {
      result.second = error{0, value};
    }
    return result;
  }

  template <class T>
  std::pair<T, std::optional<error>>
  call_api(const char *api, query_builder &builder,
           const std::vector<name_value_pair> &params) {
    std::pair<T, std::optional<error>> result;
    std::string reply;
    int status_code = 0;
    // check if there is a valid path to local file
    if (std::any_of(params.begin(), params.end(),
                    [](const name_value_pair &value) {
                      return fs::exists(fs::path(value.second));
                    })) {
      // rewrite the doc
      std::vector<httplib::MultipartFormData> items;
      const auto &build_doc = builder.get_document().GetObject();
      items.reserve(static_cast<size_t>(
          std::distance(build_doc.begin(), build_doc.end())));

      for (auto it = build_doc.begin(); it != build_doc.end(); ++it) {
        if (it->value.IsInt() || it->value.IsInt64())
          items.push_back({it->name.GetString(),
                           std::to_string(it->value.GetInt64()),
                           {}});
        else if (it->value.IsBool())
          items.push_back({it->name.GetString(),
                           it->value.GetBool() ? "true" : "false",
                           {}});
        else if (it->value.IsString())
          items.push_back(
              {it->name.GetString(), it->value.GetString()});
      }

      for (auto &&[name, path_or_id] : params) {
        if (auto path = fs::path(path_or_id); fs::exists(path)) {
          if (path.filename().string().find(".pem") != std::string::npos)
            items.push_back({"certificate",
                             utility::get_file_bytes(fs::absolute(path)),
                             '@' + path.string()});
          else
            items.push_back({name.data(),
                             utility::get_file_bytes(fs::absolute(path)),
                             path.filename().string()});
        } else {
          items.push_back({{name.data(), name.size()}, path_or_id});
        }
      }
      auto response = network_manager::i().post(base_url + api, items);
      reply = response->body;
      status_code = response->status;
    } else if (params.size()) {
      for (auto &&it : params) {
        builder << it;
      }
      auto response =
          network_manager::i().post(base_url + api, {}, builder.get_query());
      reply = response->body;
      status_code = response->status;
    } else {
      result.second = error{
          0,
          std::string("Id or filepath is not valid for at least one file at ") +
              __func__};
      return result;
    }

    auto &&[is_valid, value] = utility::parse_value(reply);
    if (is_valid) {
      assign_impl<T>(result.first, value);
    } else {
      result.second = error{static_cast<int32_t>(status_code), value};
    }
    return result;
  }
  std::string call_api_raw_json(const char *api, const query_builder &builder) {
    return network_manager::i()
        .post(base_url + api, {}, builder.get_query())
        ->body;
  }
};
} // namespace telegram
