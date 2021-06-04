/**
 * @file liblava/base/debug_utils.hpp
 * @brief Debug utilities
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/base.hpp>
#include <liblava/base/object_type.hpp>
#include <liblava/core/def.hpp>

/// Only active in debug - enable for release profiling
#ifndef LIBLAVA_DEBUG_UTILS
    #define LIBLAVA_DEBUG_UTILS LIBLAVA_DEBUG
#endif

namespace lava {

#if LIBLAVA_DEBUG_UTILS

/**
 * @brief Begin a label for a command buffer
 * 
 * @param cmd_buf Vulkan command buffer
 * @param label Name of label
 * @param color Color of label
 */
void begin_label(VkCommandBuffer cmd_buf, name label, v4 color);

/**
 * @brief End the command buffer label
 * 
 * @param cmd_buf Vulkan command buffer 
 */
void end_label(VkCommandBuffer cmd_buf);

/**
 * @brief Insert a label for a command buffer
 * 
 * @param cmd_buf Vulkan command buffer
 * @param label Name of label
 * @param color Color of label
 */
void insert_label(VkCommandBuffer cmd_buf, name label, v4 color);

/**
 * @brief Begin a label for a queue
 * 
 * @param queue Vulkan queue
 * @param label Name of label
 * @param color Color of label
 */
void begin_label(VkQueue queue, name label, v4 color);

/**
 * @brief End the queue label
 * 
 * @param queue Vulkan queue
 */
void end_label(VkQueue queue);

/**
 * @brief Insert a label for a queue
 * 
 * @param queue Vulkan queue
 * @param label Name of label
 * @param color Color of label
 */
void insert_label(VkQueue queue, name label, v4 color);

/**
 * @brief Set the object name
 * 
 * @param device Vulkan device
 * @param type Object type
 * @param handle Object handle
 * @param object Name of object
 */
void set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object);

/**
 * @brief Set the object tag
 * 
 * @param device Vulkan device
 * @param type Object type
 * @param handle Object handle
 * @param name Tag name
 * @param tag Tag data
 * @param size Size of tag data
 */
void set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size);

#else

/// @see begin_label
inline void begin_label(VkCommandBuffer cmd_buf, name label, v4 color) {}

/// @see end_label
inline void end_label(VkCommandBuffer cmd_buf) {}

/// @see insert_label
inline void insert_label(VkCommandBuffer cmd_buf, name label, v4 color) {}

/// @see begin_label
inline void begin_label(VkQueue queue, name label, v4 color) {}

/// @see end_label
inline void end_label(VkQueue queue) {}

/// @see insert_label
inline void insert_label(VkQueue queue, name label, v4 color) {}

/// @see set_object_name
inline void set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object) {}

/// @see set_object_tag
inline void set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size) {}

#endif

/**
 * @brief Set the object name
 * 
 * @tparam T Type of object
 * @param device Vulkan device
 * @param handle Object handle
 * @param object Name of object
 */
template<typename T>
inline void set_object_name(VkDevice device, T handle, name object) {
    set_object_name(device, object_type<T>::value, VkObjectHandle(handle), object);
}

/**
 * @brief Set the object tag
 * 
 * @tparam T Type of object
 * @param device Vulkan device
 * @param handle Object handle
 * @param name Tag name
 * @param tag Tag data
 * @param size Size of tag data
 */
template<typename T>
inline void set_object_tag(VkDevice device, T handle, ui64 name, void_cptr tag, size_t size) {
    set_object_tag(device, object_type<T>::value, VkObjectHandle(handle), name, tag, size);
}

/**
 * @brief Scoped debug util label
 * 
 * @tparam T VkCommandBuffer or VkQueue
 */
template<typename T>
struct scoped_label {
    /**
     * @brief Construct a new scoped label
     * 
     * @param scope Scoped label
     * @param label Name of label
     * @param color Color of label
     */
    scoped_label(T scope, name label, v4 color = v4(0.f))
    : scope(scope) {
        begin_label(scope, label, color);
    }

    /**
     * @brief Destroy the scoped label
     */
    ~scoped_label() {
        end_label(scope);
    }

private:
    /// Scoped label
    T scope;
};

} // namespace lava
