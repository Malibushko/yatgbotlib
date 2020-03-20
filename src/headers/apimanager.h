#pragma once
#include "querybuilder.h"
#include "networkmanager.h"
#include "traits.h"
#    if __has_include(<filesystem>)
#        include <filesystem>
namespace fs = namespace std::filesystem;
#else
#    include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif


namespace telegram {
using namespace helpers;
using name_value_pair = std::pair<std::string_view,std::string>;

class ApiManager {
    std::string getFileBytes(const std::string &filePath) {
        std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) {
            return {};
        }
        size_t fileSize = fs::file_size(filePath);
        std::vector<char> bytes(fileSize);
        ifs.read(bytes.data(), fileSize);

        return std::string(bytes.data(), fileSize);
    }


    std::pair<bool,std::string> getValueOrError(std::string_view view) const {
        rapidjson::Document doc;
        if (!view.size()) {
            return {false,{"Empty or not valid json"}};
        }
        doc.Parse(view.data(),view.length());
        if (doc["ok"].GetBool()) {
            if (auto& val = doc["result"];val.IsArray())
                return {true,helpers::arrayToJson(val.GetArray())};
            else if (val.IsObject())
                return {true,helpers::objectToJson(val.GetObject())};
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

    template<class T>
    void assign_impl(T& value,const std::string& data) const {
        if constexpr (helpers::is_string_type<T>)
            value = data;
        else if constexpr (helpers::is_parsable_v<T> || helpers::is_container_v<T>)
            value = std::move(fromJson<T>(data));
        else if constexpr(std::is_same_v<bool,std::decay_t<T>>)
            value = (data == "true" ? 1 : 0);
        else if constexpr (std::is_integral_v<T>)
            value = std::stoi(data);
        else if constexpr (std::is_floating_point_v<T>)
            value = std::stof(data);
    }
    template<class T,class AssignType>
    void assign_impl(T& value,const std::string& data) const {
        AssignType temp_val{};
        assign_impl<AssignType>(temp_val,data);
        value = std::move(temp_val);
    }

    std::string base_url;
public:
    ApiManager() noexcept {}
    ApiManager(std::string&& url) noexcept : base_url{std::move(url)} {}

    template <class T>
    std::pair<T,std::optional<error>> call_api(const char* api,const QueryBuilder & builder) {
        std::cout << builder.getQuery() << std::endl;
        std::shared_ptr<httplib::Response> reply = NetworkManager::i().post(
                                               base_url + api,
                                               {},
                                               builder.getQuery());
        if (!reply) {
            return {T{},error{600,"Unable to make a request"}};
        }
        std::pair<T,std::optional<error>> result;

        auto&& [is_valid,value] = getValueOrError(reply->body);
         if (is_valid) {
            assign_impl<T>(result.first,value);
        } else {
            result.second = error{static_cast<int32_t>(reply->status),value};
        }
        return result;
    }
    std::string call_api_raw_json(const char * api,const QueryBuilder& builder) {
        return NetworkManager::i().post(base_url + api,
                                               {},
                                               builder.getQuery())->body;
    }

    template <class T,class TrueOrType>
    std::pair<T,std::optional<error>> call_api(const char* api,const QueryBuilder & builder) const {
        auto reply = NetworkManager::i().post(base_url + api,
                                               {},
                                               builder.getQuery());

        std::pair<T,std::optional<error>> result;

        auto&& [is_valid,value] = getValueOrError(reply->body);
         if (is_valid) {
            if (value == "true")
                result.first = true;
            else if (value == "false")
                result.first = false;
            else
                assign_impl<T,TrueOrType>(result.first,value);
        } else {
            result.second = error{static_cast<int32_t>(reply->status),value};
        }
        return result;
    }

    template <class T>
    std::pair<T,std::optional<error>> call_api(const char* api) const {
        auto reply = NetworkManager::i().post(base_url + api);

        std::pair<T,std::optional<error>> result;

        auto&& [is_valid,value] = getValueOrError(reply->body);
         if (is_valid) {
            assign_impl<T>(result.first,value);
        } else {
            result.second = error{0,value};
        }
        return result;
    }

    template<class T>
    std::pair<T,std::optional<error>> call_api(const char * api,QueryBuilder& builder,
                                               const std::vector<name_value_pair>& params) {
        std::pair<T,std::optional<error>> result;
        std::string reply;
        int status_code = 0;
        if (std::any_of(params.begin(),params.end(),[](const name_value_pair & value)
                                                {return fs::exists(fs::path(value.second));})
            ) {
            std::vector<httplib::MultipartFormData> items;
            const auto& build_doc = builder.getDoc().GetObject();
            items.reserve(static_cast<size_t>(
                              std::distance(build_doc.begin(),build_doc.end()))
                          );

            for (auto it = build_doc.begin();it != build_doc.end();++it) {
                if (it->value.IsInt() || it->value.IsInt64())
                    items.push_back({it->name.GetString(),std::to_string(it->value.GetInt64())});
                else if (it->value.IsBool())
                    items.push_back({it->name.GetString(),it->value.GetBool() ? "true" : "false"});
                else if (it->value.IsString())
                    items.push_back({it->name.GetString(),it->value.GetString()});
            }
            for (auto&& [name,path_or_id] : params) {
                if (auto path = fs::path(path_or_id);fs::exists(path)) {
                    if (path.filename().string().find(".pem") != std::string::npos)
                     items.push_back({"certificate",getFileBytes(fs::absolute(path)),'@'+path.string()});
                    else
                        items.push_back({path.filename(),getFileBytes(fs::absolute(path)),path.filename().string()});
                } else {
                    items.push_back({{name.data(),name.size()},path_or_id});
                }
            }
            auto response = NetworkManager::i().post(base_url + api,items);
            reply = response->body;
            status_code = response->status;
        } else if (params.size()) {
            for (auto && it : params)
                builder << it;
           auto response = NetworkManager::i().post(base_url + api,
                                                   {},
                                                   builder.getQuery());
           reply = response->body;
           status_code = response->status;

        } else {
            result.second = error{0,std::string("Id or filepath is not valid for at least one file at ") + __func__};
            return result;
        }
        auto&& [is_valid,value] = getValueOrError(reply);
        if (is_valid) {
            assign_impl<T>(result.first,value);
        } else {
            result.second = error{static_cast<int32_t>(status_code),value};
        }
        return result;
    }

};
}