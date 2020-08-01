// file      : liblava/base/debug_utils.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>
#include <liblava/core/def.hpp>

#ifndef LIBLAVA_DEBUG_UTILS
#    define LIBLAVA_DEBUG_UTILS LIBLAVA_DEBUG
#endif

namespace lava {

#if LIBLAVA_DEBUG_UTILS
    void begin_label(VkCommandBuffer cmd_buf, name label, v4 color);
    void end_label(VkCommandBuffer cmd_buf);
    void insert_label(VkCommandBuffer cmd_buf, name label, v4 color);

    void begin_label(VkQueue queue, name label, v4 color);
    void end_label(VkQueue queue);
    void insert_label(VkQueue queue, name label, v4 color);

    void set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object);
    void set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size);
#else
    inline void begin_label(VkCommandBuffer cmd_buf, name label, v4 color) {}
    inline void end_label(VkCommandBuffer cmd_buf) {}
    inline void insert_label(VkCommandBuffer cmd_buf, name label, v4 color) {}

    inline void begin_label(VkQueue queue, name label, v4 color) {}
    inline void end_label(VkQueue queue) {}
    inline void insert_label(VkQueue queue, name label, v4 color) {}

    inline void set_object_name(VkDevice device, VkObjectType type, VkObjectHandle handle, name object) {}
    inline void set_object_tag(VkDevice device, VkObjectType type, VkObjectHandle handle, ui64 name, void_cptr tag, size_t size) {}
#endif

    // T can be VkCommandBuffer or VkQueue
    template<typename T>
    struct scoped_label {
        scoped_label(T scope, name label, v4 color = v4(0.f))
        : scope(scope) {
            begin_label(scope, label, color);
        }

        ~scoped_label() {
            end_label(scope);
        }

    private:
        T scope;
    };

} // namespace lava
