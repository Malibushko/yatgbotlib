#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <type_traits>
#include "boost/pfr.hpp"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "utility/traits.h"
#include "utility/utility.h"

namespace telegram {

using jsonAllocator = rapidjson::Document::AllocatorType;
using jsonArray = decltype(std::declval<rapidjson::Document>().GetArray());

// forward declarations
template <class T,typename = std::enable_if_t<traits::is_container_v<T> || traits::is_parsable_v<T>>>
T from_json(const std::string &);

template <size_t N,class MetaStruct,typename = std::enable_if_t<traits::is_parsable_v<MetaStruct>>>
void append_to_document(rapidjson::Document & doc,jsonAllocator & allocator
                  ,const MetaStruct & str);

template<size_t N,class T>
void parse_field(T& field,const rapidjson::Document& doc);



// -------------------- SERIALIZE -----------------------------------
template <class T,typename = std::enable_if_t<traits::is_parsable_v<T>>>
std::string to_json(const T& item) {
    static_assert(boost::pfr::tuple_size_v<std::decay_t<T>> > 0,"The structure has no fields.");

    rapidjson::Value object(rapidjson::kObjectType);
    rapidjson::Document document;
    document.SetObject();

    append_to_document<boost::pfr::tuple_size_v<T>-1>(document,document.GetAllocator(),item);
    return utility::to_string(document);
}

// вспомогательная функция для распарсивания отдельных полей структуры (если они массивы)
template <class T>
void array_to_json(const T & arr,rapidjson::Value & val,jsonAllocator & allocator) {
    using type = traits::optional_or_value<std::decay_t<T>>;
    if (val.IsNull()) // check if value has not been set before
        val = rapidjson::Value(rapidjson::kArrayType);
    // string case
    if constexpr (traits::is_string_type<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value strVal(rapidjson::kStringType);
            strVal.SetString(it.data(),it.size(),allocator);
            val.PushBack(strVal,allocator);
        }
    }
    // float case
    else if constexpr (std::is_floating_point_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(rapidjson::kNumberType);
            numVal.SetFloat(it);
            val.PushBack(numVal,allocator);
        }
    }
    // boolean case
    else if constexpr (std::is_same_v<typename type::value_type,bool>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(it ? rapidjson::kTrueType : rapidjson::kFalseType);
            numVal.SetBool(it);
            val.PushBack(numVal,allocator);
        }
    }
    // integer case
    else if constexpr (std::is_arithmetic_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(rapidjson::kNumberType);
            numVal.SetInt64(it);
            val.PushBack(numVal,allocator);
        }
    }
    // struct contain some other struct
    else if constexpr (traits::is_parsable_v<typename type::value_type>) {
        for (auto && it : arr) {
            const auto & json = to_json(it);
            rapidjson::Document sub_doc(&allocator);
            sub_doc.Parse(json.data());
            val.PushBack(sub_doc,allocator);
        }
    }
    // struct contains array of values
    else if constexpr (traits::is_container_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value temp_arr(rapidjson::kArrayType);
            // recursive call
            array_to_json(it,temp_arr,allocator);
            val.PushBack(temp_arr.Move(),allocator);
        }
    }
}

template <class T>
void value_to_json(const T & value,rapidjson::Value & val,jsonAllocator & allocator) {
    using field_type = typename std::decay_t<decltype(value)>;
    using type = traits::optional_or_value<field_type>; // get real value type

    // optional case
    if constexpr (traits::is_optional_v<field_type>) {
        if (value) {
            // call recursive with value
            value_to_json(value.value(),val,allocator);
            return;
        }
        else {
            // return empty value
            val = rapidjson::Value(rapidjson::kNullType);
            val.SetNull();
            return;
        }
    }
    else
        if constexpr (traits::is_variant_v<type>) {
            std::visit([&](auto && inner_val){
                // get real value from std::variant
                value_to_json(inner_val,val,allocator);
            },value);
        }
        // string case
        else if constexpr (traits::is_string_type<type>) {
            val = rapidjson::Value(rapidjson::kStringType);
            val.SetString(value.data(),value.size(),allocator);
        }
        // bool case
        else if constexpr (std::is_same_v<type, bool>) {
            std::optional<type> inner_val;
            if constexpr (traits::is_optional_v<field_type>) {
                inner_val = value.value_or(type{});
            }
            else {
                inner_val = value;
            }
            if (inner_val.value())
                val = rapidjson::Value(rapidjson::kTrueType);
            else
                val = rapidjson::Value(rapidjson::kFalseType);

            val.SetBool(inner_val.value());
        }
        // float case
        else if constexpr(std::is_floating_point_v<type>) {
            val = rapidjson::Value(rapidjson::kNumberType);
            val.SetFloat(value);
        }
        // integer case
        else if constexpr(std::is_arithmetic_v<type>) {
            val = rapidjson::Value(rapidjson::kNumberType);
            val.SetInt64(value);
        }
        // value is a struct
        else if constexpr (traits::is_parsable_v<type>) {
            val = rapidjson::Value(rapidjson::kObjectType);
            val.SetObject();
            std::string parsed_field = to_json(value);
            // parse all fields using rapidjson and write them to our struct
            rapidjson::Document subdoc(&allocator);
            subdoc.Parse(parsed_field.data());

            auto doc_obj = subdoc.GetObject();
            for (auto it = doc_obj.begin();it != doc_obj.end();++it) {
                val.AddMember(it->name,it->value,subdoc.GetAllocator());
            }
        }
        else if constexpr (traits::is_container_v<type>) {
            array_to_json(value,val,allocator);
        }
}

template <size_t N,class MetaStruct,typename>
void append_to_document(rapidjson::Document & doc, jsonAllocator & allocator
                  ,const MetaStruct & str) {
    // set init value to Null
    rapidjson::Value val(rapidjson::kNullType);
    // check if value is unique_ptr (support for other smart pointer will be added later)
    if constexpr (traits::is_unique_ptr_v<decltype (boost::pfr::get<N>(str))>) {
        if (boost::pfr::get<N>(str))
            value_to_json(*boost::pfr::get<N>(str),val,allocator);
        else
            if constexpr (N > 0)
                append_to_document<N-1,MetaStruct>(doc,allocator,str);
        else
            return;
    }
    else {
        value_to_json(boost::pfr::get<N>(str),val,allocator);
    }
    // null values are not being written to doc
    if (val.GetType() != rapidjson::kNullType) {
        rapidjson::Value name(rapidjson::kStringType);
        name.SetString(std::decay_t<MetaStruct>::template field_info<N>::name.data(),
                       std::decay_t<MetaStruct>::template field_info<N>::name.length(),
                       allocator);

        doc.AddMember(name.Move(),val,allocator);
    }
    // recursive step
    if constexpr (N > 0)
            append_to_document<N-1,MetaStruct>(doc,allocator,str);
}


// ---------------------- DESERIALIZE ----------------------------

template <class T,typename>
T from_json(const std::string& data) {
    rapidjson::Document doc;
    rapidjson::Value val;
    rapidjson::ParseResult ok = doc.Parse(data.data());
    if (!ok) { // telegram API returns some error so there is no struct in JSON
        std::cerr << "Invalid json;";
        return T{};
    }
    T item{};
    // array case
    if constexpr (traits::is_container_v<T> && !traits::is_string_type<T>) {
        auto temp_arr = doc.GetArray();
        array_from_json(item,temp_arr,doc.GetAllocator());
    }
    else if constexpr (traits::is_parsable_v<T>) {
        static_assert (boost::pfr::tuple_size_v<T> > 0,"The struct has no fields");
        parse_field<boost::pfr::tuple_size_v<T>-1>(item,doc);
    }
    return item;
}

template <class T>
void array_from_json(T& array,const jsonArray & arr,jsonAllocator & allocator) {
    using type = traits::optional_or_value<std::remove_reference_t<std::remove_cv_t<T>>>;
    if constexpr(traits::is_optional_v<T>)
       array = type{}; // if value is optional set it to default

    // the same as 'to_json'
    if constexpr (std::is_same_v<bool, typename type::value_type>) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                array.value().emplace_back(it.GetBool());
            else
                array.emplace_back(it.GetBool());
        }
    }
    else if constexpr (std::is_floating_point<typename type::value_type>::value) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                array.value().emplace_back(it.GetFloat());
            else
                array.emplace_back(it.GetFloat());
        }
    }
    else if constexpr(std::is_arithmetic<typename type::value_type>::value) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                array.value().emplace_back(it.GetInt64());
            else
                array.emplace_back(it.GetInt64());
        }
    }
    else if constexpr (traits::is_string_type<typename type::value_type>) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                array.value().emplace_back(it.GetString());
            else
                array.emplace_back(it.GetString());
        }
    }
    else if constexpr (traits::is_parsable_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Document doc(&allocator);
            doc.SetObject();
            const auto & obj = it.GetObject();

            for (auto&& field_iter : obj) {
                doc.AddMember(field_iter.name.Move(),field_iter.value.Move(),allocator);
            }
            if constexpr (traits::is_optional_v<T>) {
                array.value().emplace_back(from_json<typename type::value_type>(utility::to_string(doc)));
            } else {
                array.emplace_back(from_json<typename type::value_type>(utility::to_string(doc)));
            }
        }
    }
    else if constexpr (traits::is_container_v<typename type::value_type>) {
        using val_type = typename type::value_type;
        val_type inner_arr;
        for (auto it = arr.begin();it != arr.end();++it) {
            if (it->IsArray()) {
                auto val = it->GetArray();
                array_from_json(inner_arr,val,allocator);
            }
        }
        if constexpr (traits::is_optional_v<T>)
            array.value().emplace_back(std::move(inner_arr));
        else
            array.emplace_back(std::move(inner_arr));
    }
}
template <class T>
void parse_field_impl(const char * field_name,T & field,rapidjson::Document & doc) {
    if (!doc.HasMember(field_name)) {
        // log
        field = {};
        return;
    }
    auto & val = doc[field_name];
    using field_type = traits::optional_or_value<std::decay_t<decltype (field)>>;

    // the same as 'to_json' and 'from_json'
    if constexpr (traits::is_string_type<field_type>)
            field = val.Move().GetString();
    else if constexpr(std::is_same_v<bool,field_type>)
            field = val.GetBool();
    else if constexpr (std::is_floating_point_v<field_type>)
            field = val.GetFloat();
    else if constexpr (std::is_integral_v<field_type>)
            field = val.GetInt64();
    else if constexpr (traits::is_parsable_v<field_type>) {
        rapidjson::Document subdoc(&doc.GetAllocator());
        subdoc.SetObject();
        for (auto it = val.GetObject().begin();it != val.GetObject().end();++it) {
            subdoc.AddMember(it->name.Move(),it->value.Move(),subdoc.GetAllocator());
        }
        field = from_json<field_type>(utility::to_string(subdoc));
    }
    else if constexpr(traits::is_container_v<field_type>) {
        auto arr = val.GetArray();
        array_from_json(field,arr,doc.GetAllocator());
    }
}

template <size_t N,class T>
void parse_field(T & s,rapidjson::Document& doc) {
    if (doc.HasMember(T::template field_info<N>::name.data())) {
        using type = traits::optional_or_value<std::decay_t<decltype (boost::pfr::get<N>(s))>>;

        if constexpr (traits::is_unique_ptr_v<type>) {
            boost::pfr::get<N>(s) = std::make_unique<typename type::element_type>();
            parse_field_impl(T::template field_info<N>::name.data(),*boost::pfr::get<N>(s),doc);
        }
        else
            parse_field_impl(T::template field_info<N>::name.data(),boost::pfr::get<N>(s),doc);
    }
    if constexpr (N > 0)
            parse_field<N-1>(s,doc);
}

} // namespace end
