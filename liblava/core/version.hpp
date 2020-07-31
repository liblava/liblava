// file      : liblava/core/version.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/core/types.hpp>

namespace lava {

    struct internal_version {
        i32 major = LIBLAVA_VERSION_MAJOR;
        i32 minor = LIBLAVA_VERSION_MINOR;
        i32 patch = LIBLAVA_VERSION_PATCH;
    };

    constexpr internal_version const _internal_version = {};

    enum class version_stage {
        preview,
        alpha,
        beta,
        rc,
        release
    };

    struct version {
        i32 year = 2020;
        i32 release = 0;
        version_stage stage = version_stage::preview;
        i32 rev = 2;
    };

    constexpr version const _version = {};

    constexpr name _build_date = LIBLAVA_BUILD_DATE;
    constexpr name _build_time = LIBLAVA_BUILD_TIME;

} // namespace lava
