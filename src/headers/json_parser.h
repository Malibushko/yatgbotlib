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

namespace telegram {

using jsonAllocator = rapidjson::Document::AllocatorType;
using jsonArray = decltype(std::declval<rapidjson::Document>().GetArray());

// forward declarations
template <class T,typename = std::enable_if_t<traits::is_container_v<T> || traits::is_parsable_v<T>>>
T fromJson(const std::string &);

template <size_t N,class MetaStruct,typename = std::enable_if_t<traits::is_parsable_v<MetaStruct>>>
void appendToJson(rapidjson::Document & doc,jsonAllocator & allocator
                  ,const MetaStruct & str);

template<size_t N,class T>
void assignField(T& field,const rapidjson::Document& doc);



// -------------------- SERIALIZE -----------------------------------
template <class T,typename = std::enable_if_t<traits::is_parsable_v<T>>>
std::string toJson(const T& item) {
    rapidjson::Value object(rapidjson::kObjectType);
    rapidjson::Document document;
    document.SetObject();
    if (!boost::pfr::tuple_size_v<std::decay_t<T>>)
        return {"{}"};

    appendToJson<boost::pfr::tuple_size_v<T>-1>(document,document.GetAllocator(),item);
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    document.Accept(writer);
    return buff.GetString();
}

// вспомогательная функция для распарсивания отдельных полей структуры (если они массивы)
template <class T>
void arrayToJson(const T & arr,rapidjson::Value & val,jsonAllocator & allocator) {
    using type = traits::optional_or_value<std::decay_t<T>>;
    if (val.IsNull())
        val = rapidjson::Value(rapidjson::kArrayType);

    if constexpr (traits::is_string_type<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value strVal(rapidjson::kStringType);
            strVal.SetString(it.data(),it.size(),allocator);
            val.PushBack(strVal,allocator);
        }
    }
    else if constexpr (std::is_floating_point_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(rapidjson::kNumberType);
            numVal.SetFloat(it);
            val.PushBack(numVal,allocator);
        }
    }
    else if constexpr (std::is_same_v<typename type::value_type,bool>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(it ? rapidjson::kTrueType : rapidjson::kFalseType);
            numVal.SetBool(it);
            val.PushBack(numVal,allocator);
        }
    }
    else if constexpr (std::is_arithmetic_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value numVal(rapidjson::kNumberType);
            numVal.SetInt64(it);
            val.PushBack(numVal,allocator);
        }
    }
    else if constexpr (traits::is_parsable_v<typename type::value_type>) {
        for (auto && it : arr) {
            const auto & json = toJson(it);
            rapidjson::Document sub_doc(&allocator);
            sub_doc.Parse(json.data());
            val.PushBack(sub_doc,allocator);
        }
    }
    else if constexpr (traits::is_container_v<typename type::value_type>) {
        for (auto && it : arr) {
            rapidjson::Value temp_arr(rapidjson::kArrayType);
            arrayToJson(it,temp_arr,allocator);
            val.PushBack(temp_arr.Move(),allocator);
        }
    }
}

template <class T>
void valueToJson(const T & value,rapidjson::Value & val,jsonAllocator & allocator) {
    using field_type = typename std::decay_t<decltype(value)>;
    using type = traits::optional_or_value<field_type>;

    if constexpr (traits::is_optional_v<field_type>) {
        if (value) {
            valueToJson(value.value(),val,allocator);
            return;
        }
        else {
            val = rapidjson::Value(rapidjson::kNullType);
            val.SetNull();
            return;
        }
    }
    else
    if constexpr (traits::is_variant_v<type>) {
        std::visit([&](auto && inner_val){
            valueToJson(inner_val,val,allocator);
        },value);
    } else if constexpr (traits::is_string_type<type>) {
        val = rapidjson::Value(rapidjson::kStringType);
        val.SetString(value.data(),value.size(),allocator);
    } else if constexpr (std::is_same_v<type, bool>) {
        std::optional<type> inner_val;
        if constexpr (traits::is_optional_v<field_type>) {
            inner_val = value.value_or(type{});
        } else {
            inner_val = value;
        }
        if (inner_val.value())
            val = rapidjson::Value(rapidjson::kTrueType);
        else
            val = rapidjson::Value(rapidjson::kFalseType);

        val.SetBool(inner_val.value());
    }
    else if constexpr(std::is_floating_point_v<type>) {
        val = rapidjson::Value(rapidjson::kNumberType);
        val.SetFloat(value);
    }
    else if constexpr(std::is_integral_v<type>) {
       val = rapidjson::Value(rapidjson::kNumberType);
        val.SetInt64(value);
    }
    else if constexpr (traits::is_parsable_v<type>) {
        val = rapidjson::Value(rapidjson::kObjectType);
        val.SetObject();
        std::string parsed_field = toJson(value);
        rapidjson::Document subdoc(&allocator);
        subdoc.Parse(parsed_field.data());

        auto doc_obj = subdoc.GetObject();
        for (auto it = doc_obj.begin();it != doc_obj.end();++it) {
            val.AddMember(it->name,it->value,subdoc.GetAllocator());
        }
    }
    else if constexpr (traits::is_container_v<type>) {
        arrayToJson(value,val,allocator);
    }
}

template <size_t N,class MetaStruct,typename>
void appendToJson(rapidjson::Document & doc, jsonAllocator & allocator
                  ,const MetaStruct & str) {
    rapidjson::Value val(rapidjson::kNullType);

    if constexpr (traits::is_unique_ptr_v<decltype (boost::pfr::get<N>(str))>) {
        if (boost::pfr::get<N>(str))
            valueToJson(*boost::pfr::get<N>(str),val,allocator);
        else
            if constexpr (N > 0)
                appendToJson<N-1,MetaStruct>(doc,allocator,str);
            else
                return;
    } else
        valueToJson(boost::pfr::get<N>(str),val,allocator);

    if (val.GetType() != rapidjson::kNullType) {
        rapidjson::Value name(rapidjson::kStringType);

        name.SetString(std::decay_t<MetaStruct>::template field_info<N>::name.data(),
                       std::decay_t<MetaStruct>::template field_info<N>::name.length(),allocator);
        doc.AddMember(name.Move(),val,allocator);
    }
    if constexpr (N > 0)
            appendToJson<N-1,MetaStruct>(doc,allocator,str);
}


// ---------------------- DESERIALIZE ----------------------------

template <class T,typename>
T fromJson(const std::string& data) {
    rapidjson::Document doc;
    rapidjson::Value val;
    rapidjson::ParseResult ok = doc.Parse(data.data());
    if (!ok) {
        std::cerr << "Invalid json;";
        return {};
    }
    T item{};
    if constexpr (traits::is_container_v<T> && !traits::is_string_type<T>) {
        auto temp_arr = doc.GetArray();
        assignArrayFromJson(item,temp_arr,doc.GetAllocator());
    }
    else if constexpr (traits::is_parsable_v<T>) {
        if constexpr(boost::pfr::tuple_size_v<T> != 0)
                assignField<boost::pfr::tuple_size_v<T>-1>(item,doc);
    }
    return item;
}

template <class T>
void assignArrayFromJson(T& value,const jsonArray & arr,jsonAllocator & allocator) {
    using type = traits::optional_or_value<std::remove_reference_t<std::remove_cv_t<T>>>;
    if constexpr(traits::is_optional_v<T>)
            value = type{};

    if constexpr (std::is_same_v<bool, typename type::value_type>) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                value.value().emplace_back(it.GetBool());
            else
                value.emplace_back(it.GetBool());
        }
    }
    else if constexpr (std::is_floating_point<typename type::value_type>::value) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                value.value().emplace_back(it.GetFloat());
            else
                value.emplace_back(it.GetFloat());
        }
    }
    else if constexpr(std::is_arithmetic<typename type::value_type>::value) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
                value.value().emplace_back(it.GetInt64());
            else
                value.emplace_back(it.GetInt64());
        }
    }
    else if constexpr (traits::is_string_type<typename type::value_type>) {
        for (auto && it : arr) {
            if constexpr (traits::is_optional_v<T>)
               value.value().emplace_back(it.GetString());
            else
               value.emplace_back(it.GetString());
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

            rapidjson::StringBuffer buff;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
            doc.Accept(writer);
            if constexpr (traits::is_optional_v<T>) {
                value.value().emplace_back(fromJson<typename type::value_type>(buff.GetString()));

            } else {
                value.emplace_back(fromJson<typename type::value_type>(buff.GetString()));
                value.back();
            }
        }
    }
    else if constexpr (traits::is_container_v<typename type::value_type>) {
        using val_type = typename type::value_type;
        val_type inner_arr;
        for (auto it = arr.begin();it != arr.end();++it) {
            if (it->IsArray()) {
                auto val = it->GetArray();
                assignArrayFromJson(inner_arr,val,allocator);
            }
        }
        if constexpr (traits::is_optional_v<T>)
            value.value().emplace_back(std::move(inner_arr));
        else
            value.emplace_back(std::move(inner_arr));
    }
}
template <class T>
void assignFieldImpl(const char * field_name,T & field,rapidjson::Document & doc) {
    auto & val = doc[field_name];
    using field_type = traits::optional_or_value<std::decay_t<decltype (field)>>;

    // алгоритм аналогично функциям выше
    if constexpr (traits::is_string_type<field_type>)
            field = val.GetString();
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
            subdoc.AddMember(it->name.Move(),
                             it->value.Move(),
                             subdoc.GetAllocator());
        }
        rapidjson::StringBuffer buff;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
        subdoc.Accept(writer);
        field = fromJson<field_type>(buff.GetString());
    }
    else if constexpr(traits::is_container_v<field_type>) {
        auto arr = val.GetArray();
        assignArrayFromJson(field,arr,doc.GetAllocator());
    }
}

template <size_t N,class T>
void assignField(T & s,rapidjson::Document& doc) {
    if (doc.HasMember(T::template field_info<N>::name.data())) {
        using type = traits::optional_or_value<std::decay_t<decltype (boost::pfr::get<N>(s))>>;

        if constexpr (traits::is_unique_ptr_v<type>) {
            boost::pfr::get<N>(s) = std::make_unique<typename type::element_type>();
            assignFieldImpl(T::template field_info<N>::name.data(),*boost::pfr::get<N>(s),doc);
        }
        else
            assignFieldImpl(T::template field_info<N>::name.data(),boost::pfr::get<N>(s),doc);
    }
    if constexpr (N > 0)
            assignField<N-1>(s,doc);
}

}
