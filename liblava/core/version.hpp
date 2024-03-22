/**
 * @file         liblava/core/version.hpp
 * @brief        Version information
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/types.hpp"

namespace lava {

/**
 * @brief Semantic version
 */
struct semantic_version {
    /// Major version
    ui32 major = LAVA_VERSION_MAJOR;

    /// Minor version
    ui32 minor = LAVA_VERSION_MINOR;

    /// Patch version
    ui32 patch = LAVA_VERSION_PATCH;

    /**
     * @brief Default compare operators
     */
    auto operator<=>(semantic_version const&) const = default;
};

/// Semantic version
using sem_version = semantic_version;

/**
 * @brief Convert string to semantic version
 * @param str             String to convert
 * @return sem_version    Semantic version
 */
inline sem_version to_version(string_ref str) {
    sem_version result{ 0, 0, 0 };
    sscanf(str.c_str(), "%d.%d.%d",
           &result.major, &result.minor, &result.patch);
    return result;
}

/**
 * @brief Version stages
 */
enum class version_stage : index {
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
    ui32 year = 2024;

    /// Version release
    ui32 release = 0;

    /// Version stage
    version_stage stage = version_stage::preview;

    /// Version revision
    ui32 rev = 0;
};

/// Build date
constexpr name _build_date = LAVA_BUILD_DATE;

/// Build time
constexpr name _build_time = LAVA_BUILD_TIME;

} // namespace lava
