/**
 * @file         liblava/base/base.cpp
 * @brief        Vulkan base types
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/base/base.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool check(VkResult result) {
    if (result == VK_SUCCESS)
        return true;

    if (result > 0) {
        if (result == VK_SUBOPTIMAL_KHR)
            log()->warn("VkResult: {}", to_string(result));
        else
            log()->critical("VkResult: {}", to_string(result));

        return false;
    }

    log()->error("VkResult: {}", to_string(result));
    return false;
}

//-----------------------------------------------------------------------------
string to_string(VkResult result) {
#define RETURN_STR(result_code) \
    case result_code: \
        return string(#result_code);

    switch (result) {
        RETURN_STR(VK_SUCCESS)
        RETURN_STR(VK_NOT_READY)
        RETURN_STR(VK_TIMEOUT)
        RETURN_STR(VK_EVENT_SET)
        RETURN_STR(VK_EVENT_RESET)
        RETURN_STR(VK_INCOMPLETE)
        RETURN_STR(VK_ERROR_OUT_OF_HOST_MEMORY)
        RETURN_STR(VK_ERROR_OUT_OF_DEVICE_MEMORY)
        RETURN_STR(VK_ERROR_INITIALIZATION_FAILED)
        RETURN_STR(VK_ERROR_DEVICE_LOST)
        RETURN_STR(VK_ERROR_MEMORY_MAP_FAILED)
        RETURN_STR(VK_ERROR_LAYER_NOT_PRESENT)
        RETURN_STR(VK_ERROR_EXTENSION_NOT_PRESENT)
        RETURN_STR(VK_ERROR_FEATURE_NOT_PRESENT)
        RETURN_STR(VK_ERROR_INCOMPATIBLE_DRIVER)
        RETURN_STR(VK_ERROR_TOO_MANY_OBJECTS)
        RETURN_STR(VK_ERROR_FORMAT_NOT_SUPPORTED)
        RETURN_STR(VK_ERROR_FRAGMENTED_POOL)
        RETURN_STR(VK_ERROR_UNKNOWN)
        RETURN_STR(VK_ERROR_OUT_OF_POOL_MEMORY)
        RETURN_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE)
        RETURN_STR(VK_ERROR_FRAGMENTATION)
        RETURN_STR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
        RETURN_STR(VK_PIPELINE_COMPILE_REQUIRED)
        RETURN_STR(VK_ERROR_SURFACE_LOST_KHR)
        RETURN_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
        RETURN_STR(VK_SUBOPTIMAL_KHR)
        RETURN_STR(VK_ERROR_OUT_OF_DATE_KHR)
        RETURN_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
        RETURN_STR(VK_ERROR_VALIDATION_FAILED_EXT)
        RETURN_STR(VK_ERROR_INVALID_SHADER_NV)
        RETURN_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
        RETURN_STR(VK_ERROR_NOT_PERMITTED_KHR)
        RETURN_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
        RETURN_STR(VK_THREAD_IDLE_KHR)
        RETURN_STR(VK_THREAD_DONE_KHR)
        RETURN_STR(VK_OPERATION_DEFERRED_KHR)
        RETURN_STR(VK_OPERATION_NOT_DEFERRED_KHR)
        RETURN_STR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT)

    default:
        return fmt::format("[invalid VkResult {}]", to_i32(result));
    }

#undef RETURN_STR
}

//-----------------------------------------------------------------------------
string version_to_string(ui32 version) {
    return fmt::format("{}.{}.{}",
                       VK_API_VERSION_MAJOR(version),
                       VK_API_VERSION_MINOR(version),
                       VK_API_VERSION_PATCH(version));
}

//-----------------------------------------------------------------------------
int_version to_version(ui32 version) {
    return { (i32) VK_API_VERSION_MAJOR(version),
             (i32) VK_API_VERSION_MINOR(version),
             (i32) VK_API_VERSION_PATCH(version) };
}

//-----------------------------------------------------------------------------
ui32 to_version(int_version version) {
    return VK_MAKE_API_VERSION(0,
                               version.major,
                               version.minor,
                               version.patch);
}

//-----------------------------------------------------------------------------
api_version to_api_version(ui32 version) {
    switch (to_version(version).minor) {
    case 1:
        return api_version::v1_1;
    case 2:
        return api_version::v1_2;
    case 3:
        return api_version::v1_3;
    default:
        return api_version::v1_0;
    }
}

} // namespace lava
