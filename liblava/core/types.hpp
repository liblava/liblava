// file      : liblava/core/types.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/def.hpp>

#include <cassert>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <map>

#include <functional>

#include <enum.h>

namespace lava {

using int8 = std::int8_t;
using i8 = int8;

using uint8 = std::uint8_t;
using ui8 = uint8;

using int16 = std::int16_t;
using i16 = int16;

using uint16 = std::uint16_t;
using ui16 = uint16;

using int32 = std::int32_t;
using i32 = int32;

using uint32 = std::uint32_t;
using ui32 = uint32;

using int64 = std::int64_t;
using i64 = int64;

using uint64 = std::uint64_t;
using ui64 = uint64;

using char8 = std::int_fast8_t;
using c8 = char8;

using uchar8 = std::uint_fast8_t;
using uc8 = uchar8;

using char16 = int16;
using c16 = char16;

using uchar16 = uint16;
using uc16 = uchar16;

using char32 = int32;
using c32 = char32;

using uchar32 = uint32;
using uc32 = uchar32;

using size_t = std::size_t;
using uchar = unsigned char;
using r32 = float;
using r64 = double;
using real = r64;

using type = ui32;
constexpr type const no_type = 0xffffffff;
constexpr type const undef = 0;

using index = type;
constexpr index const no_index = no_type;
using index_list = std::vector<index>;
using index_map = std::map<index, index>;

using string = std::string;
using string_ref = string const&;
using string_list = std::vector<string>;
using string_view = std::string_view;

using name = char const*;
using names = std::vector<name>;
using names_ref = names const&;

constexpr name _lava_ = "lava";
constexpr name _liblava_ = "liblava";

template <typename T>
inline r32 to_r32(T value) { return static_cast<r32>(value); }

template <typename T>
inline r64 to_r64(T value) { return static_cast<r64>(value); }

template <typename T>
inline i32 to_i32(T value) { return static_cast<i32>(value); }

template <typename T>
inline i64 to_i64(T value) { return static_cast<i64>(value); }

template <typename T>
inline ui32 to_ui32(T value) { return static_cast<ui32>(value); }

template <typename T>
inline ui64 to_ui64(T value) { return static_cast<ui64>(value); }

template <typename T>
inline size_t to_size_t(T value) { return static_cast<size_t>(value); }

template <typename T>
inline index to_index(T value) { return static_cast<index>(value); }

struct no_copy_no_move {

    no_copy_no_move() = default;
    no_copy_no_move(no_copy_no_move const&) = delete;

    void operator=(no_copy_no_move const&) = delete;
};

struct interface {

    virtual ~interface() = default;
};

} // lava
