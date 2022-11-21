/**
 * @file         liblava/core/types.hpp
 * @brief        Basic types
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <cstdint>
#include <functional>
#include <liblava/core/def.hpp>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace lava {

/// 8 bit integer
using int8 = std::int8_t;

/// @see int8
using i8 = int8;

/// 8 bit unsigned integer
using uint8 = std::uint8_t;

/// @see uint8
using ui8 = uint8;

/// 16 bit integer
using int16 = std::int16_t;

/// @see int16
using i16 = int16;

/// 16 bit unsigned integer
using uint16 = std::uint16_t;

/// @see uint16
using ui16 = uint16;

/// 32 bit integer
using int32 = std::int32_t;

/// @see int32
using i32 = int32;

/// 32 bit unsigned integer
using uint32 = std::uint32_t;

/// @see uint32
using ui32 = uint32;

/// 64 bit integer
using int64 = std::int64_t;

/// @see int64
using i64 = int64;

/// 64 bit unsigned integer
using uint64 = std::uint64_t;

/// @see uint64
using ui64 = uint64;

/// 8 bit char
using char8 = std::int_fast8_t;

/// @see char8
using c8 = char8;

/// 8 bit unsigened char
using uchar8 = std::uint_fast8_t;

/// @see uchar8
using uc8 = uchar8;

/// 16 bit char
using char16 = int16;

/// @see char16
using c16 = char16;

/// 16 bit unsigned char
using uchar16 = uint16;

/// @see uchar16
using uc16 = uchar16;

/// 32 bit char
using char32 = int32;

/// @see char32
using c32 = char32;

/// 32 bit unsigned char
using uchar32 = uint32;

/// @see uchar32
using uc32 = uchar32;

/// Size
using size_t = std::size_t;

/// Unsigned char
using uchar = unsigned char;

/// Single-precision floating-point
using r32 = float;

/// Double-precision floating-point
using r64 = double;

/// Real number
using real = r64;

/// Delta
using delta = r32;

/// Void pointer
using void_p = void*;

/// Const void pointer
using void_cp = void const*;

/// Flag
using flag = ui32;

/// Index
using index = ui32;

/// Index type
using index_t = index;

/// No index
constexpr index const no_index = 0xffffffff;

/// List of indices
using index_list = std::vector<index>;

/// Map of indices
using index_map = std::map<index, index>;

/// String
using string = std::string;

/// Reference to string
using string_ref = string const&;

/// List of strings
using string_list = std::vector<string>;

/// Reference to list of strings
using string_list_ref = string_list const&;

/// String view
using string_view = std::string_view;

/// Map of strings
using string_map = std::map<string, string>;

/// Reference to map of strings
using string_map_ref = string_map const&;

/// Name
using name = char const*;

/// Reference to name
using name_ref = char const&;

/// List of names
using names = std::vector<name>;

/// Reference to list of names
using names_ref = names const&;

/// lava
constexpr name _lava_ = "lava";

/// liblava
constexpr name _liblava_ = "liblava";

/// default
constexpr name _default_ = "default";

/**
 * @brief Get c-string representation of string
 * @param value    Source string
 * @return name    C-string representation
 */
inline name str(string_ref value) {
    return value.c_str();
}

/**
 * @brief Convert to r32
 * @param value    Source value
 * @return r32     Converted value
 */
inline r32 to_r32(auto value) {
    return static_cast<r32>(value);
}

/**
 * @brief Convert to r64
 * @param value    Source value
 * @return r64     Converted value
 */
inline r64 to_r64(auto value) {
    return static_cast<r64>(value);
}

/**
 * @brief Convert to i32
 * @param value    Source value
 * @return i32     Converted value
 */
inline i32 to_i32(auto value) {
    return static_cast<i32>(value);
}

/**
 * @brief Convert to i64
 * @param value    Source value
 * @return i64     Converted value
 */
inline i64 to_i64(auto value) {
    return static_cast<i64>(value);
}

/**
 * @brief Convert to ui32
 * @param value    Source value
 * @return ui32    Converted value
 */
inline ui32 to_ui32(auto value) {
    return static_cast<ui32>(value);
}

/**
 * @brief Convert to ui64
 * @param value    Source value
 * @return ui64    Converted value
 */
inline ui64 to_ui64(auto value) {
    return static_cast<ui64>(value);
}

/**
 * @brief Convert to size_t
 * @param value      Source value
 * @return size_t    Converted value
 */
inline size_t to_size_t(auto value) {
    return static_cast<size_t>(value);
}

/**
 * @brief Convert to index
 * @param value     Source value
 * @return index    Converted value
 */
inline index to_index(auto value) {
    return static_cast<index>(value);
}

/**
 * @brief Convert to pointer const char
 * @param value           Source value
 * @return char const*    Converted value
 */
inline char const* to_char(auto value) {
    return (char const*) value;
}

/**
 * @brief No copy and no move object
 */
struct no_copy_no_move {
    /**
     * @brief Construct a new object
     */
    no_copy_no_move() = default;

    /**
     * @brief No copy
     */
    no_copy_no_move(no_copy_no_move const&) = delete;

    /**
     * @brief No move
     */
    void operator=(no_copy_no_move const&) = delete;
};

/**
 * @brief Interface
 */
struct interface {
    /**
     * @brief Destroy the interface
     */
    virtual ~interface() = default;
};

/**
 * @brief Combine hash seed with value - from boost (functional/hash)
 * @see http://www.boost.org/doc/libs/1_77_0/doc/html/hash/combine.html
 * @tparam T      Type of value
 * @param seed    Seed to combine
 * @param val     Value to combine
 */
template<typename T>
inline void hash_combine(size_t& seed,
                         T const& val) {
    seed ^= std::hash<T>()(val) + 0x9e3779b9
            + (seed << 6) + (seed >> 2);
}

/**
 * @brief Auxiliary generic functions to create a hash value using a seed
 * @tparam T      Type of value
 * @param seed    Seed for hash
 * @param val     Hash value
 */
template<typename T>
inline void hash_val(size_t& seed,
                     T const& val) {
    hash_combine(seed, val);
}

/**
 * @see hash_val<T>()
 */
template<typename T, typename... Types>
inline void hash_val(size_t& seed,
                     T const& val,
                     Types const&... args) {
    hash_combine(seed, val);
    hash_val(seed, args...);
}

/**
 * @see hash_val<T>()
 */
template<typename... Types>
inline size_t hash_val(Types const&... args) {
    size_t seed = 0;
    hash_val(seed, args...);
    return seed;
}

/**
 * @brief Pair hash
 */
struct pair_hash {
    /**
     * @brief Hash operator
     * @tparam T1       Type of first
     * @tparam T2       Type of second
     * @param p          Hash pair
     * @return size_t    Hash value
     */
    template<class T1, class T2>
    size_t operator()(std::pair<T1, T2> const& p) const {
        return hash_val(p.first, p.second);
    }
};

/// Enum flag operators
#define ENUM_FLAG_OPERATORS(T) \
    inline T operator~(T a) { \
        return static_cast<T>(~static_cast<std::underlying_type<T>::type>(a)); \
    } \
    inline T operator|(T a, T b) { \
        return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) | static_cast<std::underlying_type<T>::type>(b)); \
    } \
    inline T operator&(T a, T b) { \
        return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) & static_cast<std::underlying_type<T>::type>(b)); \
    } \
    inline T operator^(T a, T b) { \
        return static_cast<T>(static_cast<std::underlying_type<T>::type>(a) ^ static_cast<std::underlying_type<T>::type>(b)); \
    } \
    inline T& operator|=(T& a, T b) { \
        return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) |= static_cast<std::underlying_type<T>::type>(b)); \
    } \
    inline T& operator&=(T& a, T b) { \
        return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) &= static_cast<std::underlying_type<T>::type>(b)); \
    } \
    inline T& operator^=(T& a, T b) { \
        return reinterpret_cast<T&>(reinterpret_cast<std::underlying_type<T>::type&>(a) ^= static_cast<std::underlying_type<T>::type>(b)); \
    }

} // namespace lava
