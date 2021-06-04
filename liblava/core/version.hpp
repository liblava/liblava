/**
 * @file liblava/core/version.hpp
 * @brief Version information
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>

namespace lava {

/**
 * @brief Internal version
 */
struct internal_version {
    /// Major version
    i32 major = LIBLAVA_VERSION_MAJOR;

    /// Minor version
    i32 minor = LIBLAVA_VERSION_MINOR;

    /// Patch version
    i32 patch = LIBLAVA_VERSION_PATCH;

    /**
     * @brief Default compare operators
     */
    auto operator<=>(internal_version const&) const = default;
};

/**
 * @brief Version stage
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
    i32 year = 2021;

    /// Version release
    i32 release = 0;

    /// Version stage
    version_stage stage = version_stage::preview;

    /// Version revision
    i32 rev = 0;
};

/// Build date
constexpr name _build_date = LIBLAVA_BUILD_DATE;

/// Build time
constexpr name _build_time = LIBLAVA_BUILD_TIME;

} // namespace lava
