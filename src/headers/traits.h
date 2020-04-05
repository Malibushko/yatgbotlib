#pragma once
#include <type_traits>
#include <optional>
#include <memory>
#include <functional>
#include <fstream>
#include "../include/telegram_structs.h"

namespace telegram {
#define NAME_VALUE_PAIR(value) std::pair<std::string_view,std::decay_t<decltype(value)>>{#value,value}
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

#define DECLARE_FIELD(type, field_name) \
    type field_name; \
    template<typename Dummy__ >       \
    struct field_info<__COUNTER__-current_counter-1,Dummy__> \
    { constexpr static std::string_view name = #field_name;}

#define DECLARE_STRUCT template<size_t N,class Dummy = void> struct field_info; \
    static constexpr bool is_parsable = true;\
    static constexpr int current_counter = __COUNTER__;

#endif

namespace helpers {
// from cppref
template<class T>
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};



template <class T,typename = std::void_t<>>
struct is_unique_ptr : std::false_type {};

template <class T>
struct is_unique_ptr<T,std::void_t<decltype (*std::declval<T>()),typename T::element_type,
                                                                    typename T::deleter_type>> : std::true_type {};

template <class T>
constexpr static bool is_unique_ptr_v = is_unique_ptr<T>::value;
static_assert (!is_unique_ptr_v<int*>);
static_assert (is_unique_ptr_v<decltype(std::unique_ptr<int>{})>);



// проверяет, является ли тип std::optional
template <class T,typename = std::void_t<>>
struct is_optional : std::false_type {};

template <class T>
struct is_optional<T,std::void_t<decltype(std::declval<T>().has_value()),typename T::value_type>> : std::true_type {};

template <class T>
inline constexpr bool is_optional_v = is_optional<T>::value;

static_assert (is_optional_v<decltype (std::optional<int>{})>);
static_assert (!is_optional_v<int>);


// lazy-evaluation std::conditional
template<class T>
struct identity { using type = T; };

template<class C>
struct size_type_of : identity<typename C::value_type> { };

template<class C>
using optional_or_value = typename std::conditional<!is_optional_v<C>,
identity<C>,
size_type_of<C>>::type::type;

static_assert (std::is_same_v<optional_or_value<decltype (std::optional<int>{})>,int>);
static_assert (std::is_same_v<optional_or_value<int>, int>);


// проверяет является ли тип T одним из типов Ts
template <class T,class ...Ts>
struct is_any_of {
    static constexpr bool value = std::disjunction_v<std::is_same<T, Ts>...>;
};
template <class T>
constexpr static bool is_string_type = is_any_of<T,const char *,char *,std::string,std::string_view>::value;

template <class T,class ... Ts>
inline constexpr bool is_any_of_v = is_any_of<T,Ts...>::value;

static_assert (is_any_of_v<int,float,float,int>);
static_assert (!is_any_of_v<float,int,int,int>);


// трейт, который проверяет есть ли в нашей структуре макрос 'DECLARE_STRUCT'
template <class T,typename = std::void_t<>>
struct is_parsable : std::false_type {};

template <class T>
struct is_parsable<T,std::void_t<decltype (T::is_parsable)>> : std::true_type {};

template <class T>
constexpr inline bool is_parsable_v = is_parsable<T>::value;

struct Test {
    DECLARE_STRUCT
    DECLARE_FIELD(std::string,test);
};

static_assert (!is_parsable_v<decltype(int{})>);
static_assert (is_parsable_v<Test>);


template <class T,typename = std::void_t<>>
struct is_variant : std::false_type {};

template <class T>
struct is_variant<T,std::void_t<decltype (std::declval<T>().valueless_by_exception())>> : std::true_type {};

template <class T>
constexpr static bool is_variant_v = is_variant<T>::value;
static_assert (!is_variant_v<int>);
static_assert (is_variant_v<std::variant<int,double,std::string>>);


/// проверяет, является ли тип STL-like контейнером
template<typename T, typename _ = void>
struct is_container : std::false_type {};

template<typename... Ts>
struct is_container_helper {};

template<typename T>
struct is_container<
        T,
        std::conditional_t<
        false,
        is_container_helper<
        typename T::value_type,
        typename T::size_type,
        typename T::allocator_type,
        typename T::iterator,
        typename T::const_iterator,
        decltype(std::declval<T>().size()),
decltype(std::declval<T>().begin()),
decltype(std::declval<T>().end()),
decltype(std::declval<T>().cbegin()),
decltype(std::declval<T>().cend()) >,void>> : public std::true_type {};

template <class T>
constexpr inline bool is_container_v = is_container<T>::value;

static_assert (!is_container_v<int>);
static_assert (is_container_v<std::vector<int>>);


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


template <typename F>
struct checked_callback;

template <typename R, typename ...T>
struct checked_callback<R(T...)> {
   using checker = std::function<bool(T...)>;
   using callback = std::function<R(T...)>;
};

template <typename R, typename T>
struct checked_callback<std::function<R(T)>> {
   using checker = std::function<bool(T)>;
   using callback = std::function<R(T)>;
};

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
} // helpers

} // telegram
