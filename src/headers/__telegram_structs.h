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

