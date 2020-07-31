// file      : liblava/base/debug_utils.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/base/debug_utils.hpp>

#if LIBLAVA_DEBUG
#    include <liblava/base/instance.hpp>

void lava::begin_label(VkCommandBuffer cmd_buf, name label, v4 color) {
    if (!instance::singleton().get_debug_config().utils)
        return;

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

    vkCmdBeginDebugUtilsLabelEXT(cmd_buf, &info);
}

void lava::end_label(VkCommandBuffer cmd_buf) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    vkCmdEndDebugUtilsLabelEXT(cmd_buf);
}

void lava::insert_label(VkCommandBuffer cmd_buf, name label, v4 color) {
    if (!instance::singleton().get_debug_config().utils)
        return;

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

    vkCmdInsertDebugUtilsLabelEXT(cmd_buf, &info);
}

void lava::begin_label(VkQueue queue, name label, v4 color) {
    if (!instance::singleton().get_debug_config().utils)
        return;

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

    vkQueueBeginDebugUtilsLabelEXT(queue, &info);
}

void lava::end_label(VkQueue queue) {
    if (!instance::singleton().get_debug_config().utils)
        return;

    vkQueueEndDebugUtilsLabelEXT(queue);
}

void lava::insert_label(VkQueue queue, name label, v4 color) {
    if (!instance::singleton().get_debug_config().utils)
        return;

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

    vkQueueInsertDebugUtilsLabelEXT(queue, &info);
}

void lava::set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object) {
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

void lava::set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size) {
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

#endif // LIBLAVA_DEBUG
