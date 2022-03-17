/**
 * @file         liblava/base/profile.hpp
 * @brief        Vulkan profile
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/base.hpp>
#include <vulkan/vulkan_profiles.hpp>

namespace lava {

/**
 * @brief Vulkan profile information
 */
struct profile_info {
    /**
     * @brief Construct a new profile information
     */
    profile_info() = default;

    /**
     * @brief Construct a new profile information
     *
     * @param profile_name       Name of profile
     * @param spec_version       Specification Version
     * @param min_api_version    Minimal API version
     */
    profile_info(string profile_name,
                 i32 spec_version,
                 ui32 min_api_version)
    : name(profile_name), spec_version(spec_version),
      min_api_version(to_version(min_api_version)) {
    }

    /**
     * @brief Check if profile information is empty
     *
     * @return true     Profile information is set
     * @return false    No profile information set
     */
    bool empty() const {
        return name.empty();
    }

    /**
     * @brief Get Vulkan profile properties
     *
     * @return VpProfileProperties    Profile properties
     */
    VpProfileProperties get() const {
        VpProfileProperties result;
        strcpy(result.profileName, str(name));
        result.specVersion = spec_version;
        return result;
    }

    /**
     * @brief Get the minimal API version
     *
     * @return ui32    Minimal API version
     */
    ui32 get_min_api_version() const {
        return to_version(min_api_version);
    }

private:
    /// Name of profile
    string name;

    /// Specification version
    i32 spec_version = 0;

    /// Minimal API version
    internal_version min_api_version{ 0, 0, 0 };
};

/**
 * @brief Get VP_KHR_ROADMAP_2022 profile information
 *
 * @return profile_info    Profile information
 */
inline profile_info profile_roadmap_2022() {
    return {
        VP_KHR_ROADMAP_2022_NAME,
        VP_KHR_ROADMAP_2022_SPEC_VERSION,
        VP_KHR_ROADMAP_2022_MIN_API_VERSION
    };
}

/**
 * @brief Get VP_LUNARG_DESKTOP_PORTABILITY_2021 profile information
 *
 * @return profile_info    Profile information
 */
inline profile_info profile_desktop_portability_2021() {
    return {
        VP_LUNARG_DESKTOP_PORTABILITY_2021_NAME,
        VP_LUNARG_DESKTOP_PORTABILITY_2021_SPEC_VERSION,
        VP_LUNARG_DESKTOP_PORTABILITY_2021_MIN_API_VERSION
    };
}

/**
 * @brief Check if Vulkan Profile is supported
 *
 * @param profile    Profile to check
 *
 * @return true      Profile is supported
 * @return false     Profile is not supported
 */
inline bool profile_supported(VpProfileProperties profile) {
    auto res = VK_FALSE;
    if (failed(vpGetInstanceProfileSupport(nullptr, &profile, &res))) {
        log()->error("profile support");
        return false;
    }

    return res == VK_TRUE;
}

} // namespace lava