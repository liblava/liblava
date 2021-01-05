// file      : liblava/base/queue.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/base.hpp>

namespace lava {

    struct queue {
        using list = std::deque<queue>;
        using ref = queue const&;

        VkQueue vk_queue = nullptr;
        VkQueueFlags flags = 0;

        index family = 0;
        r32 priority = 1.f;

        bool operator<(queue const& other) const {
            return priority < other.priority;
        }
    };

    constexpr VkQueueFlags const default_queue_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

    struct queue_info {
        using list = std::deque<queue_info>;

        VkQueueFlags flags = default_queue_flags;
        r32 priority = 1.f;
    };

    struct queue_family_info {
        using list = std::deque<queue_family_info>;

        index family_index = 0;
        queue_info::list queues;

        void add(VkQueueFlags flags, ui32 count = 1, r32 priority = 1.f) {
            for (auto i = 0u; i < count; ++i) {
                queue_info info{ flags, priority };
                queues.push_back(info);
            }
        }

        ui32 count() const {
            return to_ui32(queues.size());
        }

        void clear() {
            queues.clear();
        }
    };

    void set_default_queues(queue_family_info::list& list);
    void set_all_queues(queue_family_info::list& list, VkQueueFamilyPropertiesList const& properties);

    bool add_queues(queue_family_info::list& list,
                    VkQueueFamilyPropertiesList const& properties,
                    VkQueueFlags flags, ui32 count, r32 priority = 1.f);

    bool add_dedicated_queues(queue_family_info::list& list,
                              VkQueueFamilyPropertiesList const& properties,
                              r32 priority = 1.f);

    enum class verify_queues_result : type {
        ok = 0,
        empty_list,
        no_properties,
        duplicate_family_index,
        no_family_index,
        no_queues,
        too_many_queues,
        no_compatible_flags
    };
    verify_queues_result verify_queues(queue_family_info::list const& list, VkQueueFamilyPropertiesList const& properties);

} // namespace lava
