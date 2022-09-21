/**
 * @file         liblava/core/version.hpp
 * @brief        Version information
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>

namespace lava {

/**
 * @brief Internal version
 */
struct internal_version {
    /// Major version
    i32 major = LAVA_VERSION_MAJOR;

    /// Minor version
    i32 minor = LAVA_VERSION_MINOR;

    /// Patch version
    i32 patch = LAVA_VERSION_PATCH;

    /**
     * @brief Default compare operators
     */
    auto operator<=>(internal_version const&) const = default;
};

/// Internal version
using int_version = internal_version;

/**
 * @brief Convert string to internal version
 * @param str             String to convert
 * @return int_version    Internal version
 */
inline int_version to_version(string_ref str) {
    int_version result{ 0, 0, 0 };
    sscanf(str.c_str(), "%d.%d.%d",
           &result.major, &result.minor, &result.patch);
    return result;
}

/**
 * @brief Version stages
 */
enum class version_stage {
    preview,
    alpha,
    beta,
    rc,
    release
};

/**
 * @brief Version
 */
struct version {
    /// Version year
    i32 year = 2022;

    /// Version release
    i32 release = 0;

    /// Version stage
    version_stage stage = version_stage::alpha;

    /// Version revision
    i32 rev = 0;
};

/// Build date
constexpr name _build_date = LAVA_BUILD_DATE;

/// Build time
constexpr name _build_time = LAVA_BUILD_TIME;

} // namespace lava
