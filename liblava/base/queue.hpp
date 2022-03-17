/**
 * @file         liblava/base/queue.hpp
 * @brief        Device queue
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/base.hpp>

namespace lava {

/**
 * @brief Device queue
 */
struct queue {
    /// List of queues
    using list = std::deque<queue>;

    /// Reference to queue
    using ref = queue const&;

    /// Vulkan queue
    VkQueue vk_queue = nullptr;

    /// Queue flags
    VkQueueFlags flags = 0;

    /// Queue family index
    index family = 0;

    /// Queue priority
    r32 priority = 1.f;

    /**
     * @brief Check if queue is valid
     *
     * @return true     Queue is valid
     * @return false    Queue is invalid
     */
    bool valid() const {
        return vk_queue != nullptr;
    }

    /**
     * @brief Queue priority compare operator
     *
     * @param other     Queue to compare
     *
     * @return true     Priority of queue is higher
     * @return false    Priority of queue is lower or equal
     */
    bool operator<(queue const& other) const {
        return priority < other.priority;
    }
};

/// Default queue flags
constexpr VkQueueFlags const default_queue_flags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

/**
 * @brief Queue information
 */
struct queue_info {
    /// List of queue informations
    using list = std::deque<queue_info>;

    /// Queue flags
    VkQueueFlags flags = default_queue_flags;

    /// Queue priority
    r32 priority = 1.f;
};

/**
 * @brief Queue family information
 */
struct queue_family_info {
    /// List of queue family informations
    using list = std::deque<queue_family_info>;

    /// Queue family index
    index family_index = 0;

    /// List of queue informations
    queue_info::list queues;

    /**
     * @brief Add a queue family information
     *
     * @param flags       Queue flags
     * @param count       Number of queues
     * @param priority    Queue priority
     */
    void add(VkQueueFlags flags, ui32 count = 1, r32 priority = 1.f) {
        for (auto i = 0u; i < count; ++i) {
            queue_info info{ flags, priority };
            queues.push_back(info);
        }
    }

    /**
     * @brief Get the count of queues
     *
     * @return ui32    Count of queues
     */
    ui32 count() const {
        return to_ui32(queues.size());
    }

    /**
     * @brief Clear the queue information
     */
    void clear() {
        queues.clear();
    }
};

/**
 * @brief Set the default queues
 *
 * @param list    List of queue family informations
 */
void set_default_queues(queue_family_info::list& list);

/**
 * @brief Set all queues
 *
 * @param list          List of queue family informations
 * @param properties    List of queue family properties
 */
void set_all_queues(queue_family_info::list& list, VkQueueFamilyPropertiesList const& properties);

/**
 * @brief Add queues
 *
 * @param list          List of queue family informations
 * @param properties    List of queue family properties
 * @param flags         Queue flags
 * @param count         Number of queues
 * @param priority      Queue priority
 *
 * @return true         Add was successful
 * @return false        Add failed
 */
bool add_queues(queue_family_info::list& list,
                VkQueueFamilyPropertiesList const& properties,
                VkQueueFlags flags, ui32 count, r32 priority = 1.f);

/**
 * @brief Add dedicated queues
 *
 * @param list          List of queue family informations
 * @param properties    List of queue family properties
 * @param priority      Queue priority
 *
 * @return true         Add was successful
 * @return false        Add failed
 */
bool add_dedicated_queues(queue_family_info::list& list,
                          VkQueueFamilyPropertiesList const& properties,
                          r32 priority = 1.f);

/**
 * @brief Result of queue verifications
 */
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

/**
 * @brief Verify queues
 *
 * @param list                     List of queue family informations
 * @param properties               List of queue family properties
 *
 * @return verify_queues_result    Verification result
 */
verify_queues_result verify_queues(queue_family_info::list const& list, VkQueueFamilyPropertiesList const& properties);

} // namespace lava
