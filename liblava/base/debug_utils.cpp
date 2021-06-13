/**
 * @file         liblava/base/debug_utils.cpp
 * @brief        Debug utilities
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/debug_utils.hpp>

#if LIBLAVA_DEBUG_UTILS
    #include <liblava/base/instance.hpp>
    #include <optional>

namespace lava {

/// Optional debug utils label
using optional_label = std::optional<VkDebugUtilsLabelEXT>;

/**
 * @brief Create a label information
 * 
 * @param label              Name of label
 * @param color              Color of label
 * @return optional_label    Optional label
 */
optional_label create_label_info(name label, v4 color) {
    if (!instance::singleton().get_debug_config().utils)
        return {};

    VkDebugUtilsLabelEXT info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pLabelName = label,
        .color = {
            color.r,
            color.g,
            color.b,
            color.a,
        },
    };

    return info;
}

//-----------------------------------------------------------------------------
void begin_label(VkCommandBuffer cmd_buf, name label, v4 color) {
    auto info = create_label_info(label, color);
    if (!info)
        return;

    vkCmdBeginDebugUtilsLabelEXT(cmd_buf, &*info);
}

//-----------------------------------------------------------------------------
void end_label(VkCommandBuffer cmd_buf) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    vkCmdEndDebugUtilsLabelEXT(cmd_buf);
}

//-----------------------------------------------------------------------------
void insert_label(VkCommandBuffer cmd_buf, name label, v4 color) {
    auto info = create_label_info(label, color);
    if (!info)
        return;

    vkCmdInsertDebugUtilsLabelEXT(cmd_buf, &*info);
}

//-----------------------------------------------------------------------------
void begin_label(VkQueue queue, name label, v4 color) {
    auto info = create_label_info(label, color);
    if (!info)
        return;

    vkQueueBeginDebugUtilsLabelEXT(queue, &*info);
}

//-----------------------------------------------------------------------------
void end_label(VkQueue queue) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    vkQueueEndDebugUtilsLabelEXT(queue);
}

//-----------------------------------------------------------------------------
void insert_label(VkQueue queue, name label, v4 color) {
    auto info = create_label_info(label, color);
    if (!info)
        return;

    vkQueueInsertDebugUtilsLabelEXT(queue, &*info);
}

//-----------------------------------------------------------------------------
void set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    VkDebugUtilsObjectNameInfoEXT info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
        .objectType = type,
        .objectHandle = handle,
        .pObjectName = object,
    };

    vkSetDebugUtilsObjectNameEXT(device, &info);
}

//-----------------------------------------------------------------------------
void set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    VkDebugUtilsObjectTagInfoEXT info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_TAG_INFO_EXT,
        .objectType = type,
        .objectHandle = handle,
        .tagName = name,
        .tagSize = size,
        .pTag = tag,
    };

    vkSetDebugUtilsObjectTagEXT(device, &info);
}

} // namespace lava

#endif
