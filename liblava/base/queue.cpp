/**
 * @file         liblava/base/queue.cpp
 * @brief        Device queue
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/queue.hpp>
#include <liblava/util/misc.hpp>

namespace lava {

//-----------------------------------------------------------------------------
void set_default_queues(queue_family_info::list& list) {
    list.clear();

    list.resize(1);
    list.at(0).queues.resize(1);
}

//-----------------------------------------------------------------------------
void set_all_queues(queue_family_info::list& list,
                    VkQueueFamilyPropertiesList const& properties) {
    list.clear();

    for (auto& queue_family : properties) {
        queue_family_info family_info;
        family_info.family_index = list.size();
        for (auto queue_count = 0;
             queue_count < queue_family.queueCount;
             ++queue_count) {
            queue_info info{ queue_family.queueFlags, 1.f };
            family_info.queues.push_back(info);
        }

        list.push_back(family_info);
    }
}

//-----------------------------------------------------------------------------
void add_queues(queue_family_info::list& list,
                index family_index,
                VkQueueFlags flags,
                ui32 count,
                r32 priority) {
    for (auto& family_info : list) {
        if (family_info.family_index == family_index) {
            family_info.add(flags, count, priority);
            return;
        }
    }

    queue_family_info family_info;
    family_info.family_index = family_index;
    family_info.add(flags, count, priority);
    list.push_back(family_info);
}

//-----------------------------------------------------------------------------
bool add_queues(queue_family_info::list& list,
                VkQueueFamilyPropertiesList const& properties,
                VkQueueFlags flags,
                ui32 count,
                r32 priority) {
    VkQueueFamilyPropertiesList free_properties;
    for (auto family_index = 0u; family_index < properties.size(); ++family_index) {
        auto family_properties = properties.at(family_index);
        for (auto& family_info : list) {
            if (family_info.family_index == family_index) {
                family_properties.queueCount =
                    properties.at(family_index).queueCount - family_info.count();
                break;
            }
        }

        free_properties.push_back(family_properties);
    }

    // first look for a free family with the same flags (in reverse order)
    i32 family_index_count = free_properties.size();
    for (auto family_index = family_index_count - 1;
         family_index >= 0;
         --family_index) {
        auto const& queue_family = free_properties.at(family_index);

        auto queue_family_flags = queue_family.queueFlags & ~VK_QUEUE_SPARSE_BINDING_BIT; // ignore
        if ((queue_family_flags == flags) && (queue_family.queueCount >= count)) {
            add_queues(list, family_index, flags, count, priority);
            return true;
        }
    }

    // fallback: search again if the flags exists within a free family
    for (auto family_index = family_index_count - 1; family_index >= 0; --family_index) {
        auto const& queue_family = free_properties.at(family_index);

        auto queue_family_flags = queue_family.queueFlags & ~VK_QUEUE_SPARSE_BINDING_BIT; // ignore
        if (((queue_family_flags & flags) == flags)
            && (queue_family.queueCount >= count)) {
            add_queues(list, family_index, flags, count, priority);
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
bool add_dedicated_queues(queue_family_info::list& list,
                          VkQueueFamilyPropertiesList const& properties,
                          r32 priority) {
    if (properties.size() < 2)
        return false;

    for (auto family_index = 1u; family_index < properties.size(); ++family_index) {
        queue_family_info family_info;
        family_info.family_index = family_index;
        for (auto queue_count = 0; queue_count < properties.at(family_index).queueCount; ++queue_count) {
            queue_info info{ properties.at(family_index).queueFlags, priority };
            family_info.queues.push_back(info);
        }

        list.push_back(family_info);
    }

    return true;
}

//-----------------------------------------------------------------------------
verify_queues_result verify_queues(queue_family_info::list const& list,
                                   VkQueueFamilyPropertiesList const& properties) {
    if (list.empty())
        return verify_queues_result::empty_list;

    if (properties.empty())
        return verify_queues_result::no_properties;

    index_list families;
    for (auto& info : list) {
        if (contains(families, info.family_index))
            return verify_queues_result::duplicate_family_index;

        families.push_back(info.family_index);

        if (info.family_index > properties.size() - 1)
            return verify_queues_result::no_family_index;

        if (info.queues.empty())
            return verify_queues_result::no_queues;

        if (info.count() > properties[info.family_index].queueCount)
            return verify_queues_result::too_many_queues;

        for (auto& queue : info.queues) {
            if ((properties[info.family_index].queueFlags & queue.flags) != queue.flags)
                return verify_queues_result::no_compatible_flags;
        }
    }

    return verify_queues_result::ok;
}

} // namespace lava
