/**
 * @file         liblava/base/test/queue.cpp
 * @brief        Queue unit tests
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/test.hpp"

// https://vulkan.gpuinfo.org/listreports.php?devicename=GeForce+GTX+1060
//-----------------------------------------------------------------------------
TEST_CASE("queue setup - GeForce GTX 1060", "[queue]") {
    VkQueueFamilyPropertiesList properties{
        { VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 16 },
        { VK_QUEUE_TRANSFER_BIT, 2 },
        { VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 8 }
    };

    queue_family_info::list list;

    SECTION("set default queues") {
        set_default_queues(list);

        REQUIRE(list.at(0).count() == 1);
        REQUIRE(list.at(0).queues.at(0).flags
                == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT));
    }

    SECTION("set all queues") {
        set_all_queues(list, properties);

        REQUIRE(list.at(0).count() == 16);
        for (auto& queue : list.at(0).queues)
            REQUIRE(queue.flags
                    == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT));

        REQUIRE(list.at(1).count() == 2);
        for (auto& queue : list.at(1).queues)
            REQUIRE(queue.flags == (VK_QUEUE_TRANSFER_BIT));

        REQUIRE(list.at(2).count() == 8);
        for (auto& queue : list.at(2).queues)
            REQUIRE(queue.flags == (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT));
    }

    SECTION("add default + each dedicated queues") {
        set_default_queues(list);

        REQUIRE(add_queues(list, properties, VK_QUEUE_COMPUTE_BIT, 1));
        REQUIRE(add_queues(list, properties, VK_QUEUE_TRANSFER_BIT, 1));
    }

    REQUIRE(verify_queues(list, properties) == verify_queues_result::ok);
}

// https://vulkan.gpuinfo.org/listreports.php?devicename=Radeon+RX+580+Series
//-----------------------------------------------------------------------------
TEST_CASE("queue setup - Radeon RX 580 Series", "[queue]") {
    VkQueueFamilyPropertiesList properties{
        { VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1 },
        { VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 2 },
        { VK_QUEUE_TRANSFER_BIT, 2 }
    };

    queue_family_info::list list;

    SECTION("add all dedicated queues") {
        SECTION("with default queues") {
            set_default_queues(list);
            REQUIRE(list.size() == 1);
            REQUIRE(add_dedicated_queues(list, properties));
            REQUIRE(list.size() == 3);
        }

        SECTION("without default queues") {
            REQUIRE(list.empty());
            REQUIRE(add_dedicated_queues(list, properties));
            REQUIRE(list.size() == 2);
        }
    }

    SECTION("add more queues + fallback") {
        REQUIRE(add_queues(list, properties, VK_QUEUE_TRANSFER_BIT, 2));
        REQUIRE(add_queues(list, properties, VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT, 1));
        REQUIRE(add_queues(list, properties, VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT, 1));

        // fallback
        REQUIRE(add_queues(list, properties, VK_QUEUE_TRANSFER_BIT, 1));
        REQUIRE_FALSE(add_queues(list, properties, VK_QUEUE_COMPUTE_BIT, 1));
    }

    REQUIRE(verify_queues(list, properties) == verify_queues_result::ok);
}

// https://vulkan.gpuinfo.org/listreports.php?devicename=Intel%28R%29+HD+Graphics+620
//-----------------------------------------------------------------------------
TEST_CASE("queue setup - Intel(R) HD Graphics 620", "[queue]") {
    VkQueueFamilyPropertiesList properties{
        { VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1 }
    };

    queue_family_info::list list;

    SECTION("no dedicated queues available") {
        REQUIRE_FALSE(add_dedicated_queues(list, properties));

        REQUIRE(verify_queues(list, properties) == verify_queues_result::empty_list);
    }

    SECTION("no more queues available") {
        REQUIRE(add_queues(list, properties, VK_QUEUE_GRAPHICS_BIT, 1));
        REQUIRE_FALSE(add_queues(list, properties, VK_QUEUE_COMPUTE_BIT, 1));

        REQUIRE(verify_queues(list, properties) == verify_queues_result::ok);
    }
}
