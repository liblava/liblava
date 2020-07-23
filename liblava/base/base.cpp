// file      : liblava/base/base.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/base.hpp>

bool lava::check(VkResult result) {
    if (result == VK_SUCCESS)
        return true;

    if (result > 0) {
        log()->critical("VkResult {}", to_string(result));
        return false;
    }

    log()->error("VkResult {}", to_string(result));
    return false;
}

lava::string lava::to_string(VkResult result) {
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
        RETURN_STR(VK_ERROR_OUT_OF_POOL_MEMORY)
        RETURN_STR(VK_ERROR_INVALID_EXTERNAL_HANDLE)
        RETURN_STR(VK_ERROR_SURFACE_LOST_KHR)
        RETURN_STR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
        RETURN_STR(VK_SUBOPTIMAL_KHR)
        RETURN_STR(VK_ERROR_OUT_OF_DATE_KHR)
        RETURN_STR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
        RETURN_STR(VK_ERROR_VALIDATION_FAILED_EXT)
        RETURN_STR(VK_ERROR_INVALID_SHADER_NV)
        RETURN_STR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
        RETURN_STR(VK_ERROR_FRAGMENTATION_EXT)
        RETURN_STR(VK_ERROR_NOT_PERMITTED_EXT)
        RETURN_STR(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT)
        RETURN_STR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)

    default:
        return fmt::format("[invalid VkResult {}]", result);
    }

#undef RETURN_STR
}

lava::string lava::version_to_string(ui32 version) {
    return fmt::format("{}.{}.{}", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));
}
