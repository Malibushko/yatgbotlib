#pragma once
#include <type_traits>
#include <functional>
#include <optional>
#include <memory>
#include "telegram_structs.h"

namespace telegram {

namespace traits {
template<class T>
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};
template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

static_assert (std::is_same_v<remove_cvref_t<const int&>,int>);

template <class T,typename = std::void_t<>>
struct is_unique_ptr : std::false_type {};

template <class T>
struct is_unique_ptr<T,std::void_t<decltype (*std::declval<T>()),typename T::element_type,
                                                                    typename T::deleter_type>> : std::true_type {};

template <class T>
constexpr static bool is_unique_ptr_v = is_unique_ptr<T>::value;
static_assert (!is_unique_ptr_v<int*>);
static_assert (is_unique_ptr_v<decltype(std::unique_ptr<int>{})>);



template <class T,typename = std::void_t<>>
struct is_logger : std::false_type {};

template <class T>
struct is_logger<T,std::void_t<decltype (T::error),decltype (T::info),
                                decltype(T::warning),decltype(T::critical)>> : std::true_type {};
template <class T>
constexpr static bool is_logger_v = is_logger<T>::value;

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

static_assert (!is_parsable_v<decltype(int{})>);


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

} // helpers

} // telegram
