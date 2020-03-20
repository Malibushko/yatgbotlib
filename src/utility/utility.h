#pragma once
#include <cstdint>
#include <string>
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
namespace telegram::utility {
#ifndef NAME_VALUE_PAIR
#define NAME_VALUE_PAIR(value) std::pair<std::string_view,std::decay_t<decltype(value)>>{#value,value}
#endif

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


#define DECLARE_FIELD(type, field_name) \
    type field_name; \
    template<typename Dummy__ >       \
    struct field_info<counter_id()-current_counter-1,Dummy__> \
    { constexpr static std::string_view name = #field_name;}

#define DECLARE_STRUCT template<size_t N,class Dummy = void> struct field_info; \
    static constexpr bool is_parsable = true;\
    static constexpr int current_counter = counter_id();

#else
#ifndef DECLARE_FIELD
#define DECLARE_FIELD(type, field_name) \
    type field_name; \
    template<typename Dummy__ >       \
    struct field_info<__COUNTER__-current_counter-1,Dummy__> \
    { constexpr static std::string_view name = #field_name;}
#endif
#ifndef DECLARE_STRUCT
#define DECLARE_STRUCT template<size_t N,class Dummy = void> struct field_info; \
    static constexpr bool is_parsable = true;\
    static constexpr int current_counter = __COUNTER__;
#endif
#endif

static std::string objectToJson(rapidjson::Value val) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    val.Accept(writer);
    return buff.GetString();
}
static std::string arrayToJson(rapidjson::Value val) {
    rapidjson::StringBuffer buff;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buff);
    rapidjson::Document doc;
    doc.SetArray();

    for (auto it = val.Begin();it != val.End();++it) {
        doc.PushBack(*it,doc.GetAllocator());
    }
    doc.Accept(writer);
    return buff.GetString();
}

inline constexpr uint32_t to_ipv4(uint8_t oct1, uint8_t oct2, uint8_t oct3, uint8_t oct4) noexcept {
        uint32_t result{0};
        result |= oct1;
        result <<= 8;
        result |= oct2;
        result <<= 8;
        result |= oct3;
        result <<= 8;
        result |= oct4;
        return result;
}
static inline uint32_t ipv4(const std::string& s) {
        uint8_t last_pos = 0;
        uint8_t buf = 0;
        uint8_t oct1 = std::stoi(s.substr(0,buf = last_pos = s.find_first_of('.')));
        uint8_t oct2 = std::stoi(s.substr(buf+1,last_pos = s.find_first_of('.',buf+1)));
        buf = last_pos;
        uint8_t oct3 = std::stoi(s.substr(buf+1,last_pos = s.find_first_of('.',buf+1)));
        buf = last_pos;
        uint8_t oct4 = std::stoi(s.substr(buf+1));

        return to_ipv4(oct1,
                       oct2,
                       oct3,
                       oct4);
}

constexpr uint32_t telegram_first_subnet_range_begin = 2509938689;
constexpr uint32_t telegram_first_subnet_range_end = 2509942782;
constexpr uint32_t teleram_second_subnet_range_begin = 1533805569;
constexpr uint32_t telegram_second_subned_range_end = 1533806590;

struct error {
    int32_t error_code;
    std::string description;
    template<typename IStream>
    friend std::ostream& operator<<(IStream& os,const error & e) {
        os <<"[Error]: " << e.error_code << " : " << (e.description.size() ? e.description : " no description")
          << std::endl;
        return  os;
    }
};
}
