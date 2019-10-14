// file      : liblava/block/block.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/block/block.hpp>

namespace lava {

bool command::create(device* device, index frame_count, VkCommandPools command_pools) {

    buffers.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {

        VkCommandBufferAllocateInfo allocate_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pools.at(i),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        if (failed(device->call().vkAllocateCommandBuffers(device->get(), &allocate_info, &buffers.at(i)))) {

            log()->error("command::create vkAllocateCommandBuffers failed");
            return false;
        }

        if (failed(device->call().vkResetCommandPool(device->get(), command_pools.at(i), 0))) {

            log()->error("command::create vkResetCommandPool failed");
            return false;
        }
    }

    return true;
}
void command::destroy(device* device, VkCommandPools command_pools) {

    for (auto i = 0u; i < buffers.size(); ++i)
        device->call().vkFreeCommandBuffers(device->get(), command_pools.at(i), 1, &buffers.at(i));
}

bool block::create(device* device, index frame_count, index queue_family) {

    dev = device;

    current_frame = 0;

    command_pools.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {

        VkCommandPoolCreateInfo create_info 
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_family,
        };
        if (failed(device->call().vkCreateCommandPool(dev->get(), &create_info, memory::alloc(), &command_pools.at(i)))) {

            log()->error("block::create vkCreateCommandPool failed");
            return false;
        }
    }

    for (auto& command : commands)
        if (!command.second.create(dev, frame_count, command_pools))
            return false;

    return true;
}

void block::destroy() {

    for (auto& command : commands)
        command.second.destroy(dev, command_pools);

    for (auto i = 0u; i < command_pools.size(); ++i)
        dev->call().vkDestroyCommandPool(dev->get(), command_pools.at(i), memory::alloc());

    command_pools.clear();
    cmd_order.clear();
    commands.clear();
}

id block::add_cmd(command::func func) {

    command cmd;
    cmd.on_func = func;

    if (dev && !command_pools.empty())
        if (!cmd.create(dev, get_frame_count(), command_pools))
            return undef_id;

    auto result = cmd.get_id();

    commands.emplace(result, std::move(cmd));
    cmd_order.push_back(&commands.at(result));

    return result;
}

void block::remove_cmd(id::ref cmd) {

    if (!commands.count(cmd))
        return;

    auto& command = commands.at(cmd);
    command.destroy(dev, command_pools);

    remove(cmd_order, &command);

    commands.erase(cmd);
}

bool block::process(index frame) {

    current_frame = frame;

    if (failed(dev->call().vkResetCommandPool(dev->get(), command_pools.at(frame), 0))) {

        log()->error("block::process vkResetCommandPool failed");
        return false;
    }

    for (auto& command : cmd_order) {

        auto& cmd_buf = command->buffers.at(frame);

        VkCommandBufferBeginInfo begin_info
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        if (failed(dev->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        if (command->on_func)
            command->on_func(cmd_buf);

        if (failed(dev->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
}

} // lava
