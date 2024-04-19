/**
 * @file         liblava/block/block.cpp
 * @brief        Command buffer model
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/block/block.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/util/log.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool command::create(device::ptr device,
                     index frame_count,
                     VkCommandPools cmd_pools) {
    buffers.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandBufferAllocateInfo const allocate_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = cmd_pools.at(i),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        if (failed(device->call().vkAllocateCommandBuffers(device->get(),
                                                           &allocate_info,
                                                           &buffers.at(i)))) {
            log()->error("create command buffers");
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
void command::destroy(device::ptr device,
                      VkCommandPools cmd_pools) {
    for (auto i = 0u; i < buffers.size(); ++i)
        device->call().vkFreeCommandBuffers(device->get(),
                                            cmd_pools.at(i),
                                            1,
                                            &buffers.at(i));
}

//-----------------------------------------------------------------------------
bool block::create(device::ptr dev,
                   index frame_count,
                   index queue_family) {
    device = dev;

    current_frame = 0;

    cmd_pools.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandPoolCreateInfo const create_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = 0,
            .queueFamilyIndex = queue_family,
        };
        if (failed(device->call().vkCreateCommandPool(device->get(),
                                                      &create_info,
                                                      memory::instance().alloc(),
                                                      &cmd_pools.at(i)))) {
            log()->error("create block command pool");
            return false;
        }
    }

    for (auto& [id, command] : commands)
        if (!command->create(device, frame_count, cmd_pools))
            return false;

    return true;
}

//-----------------------------------------------------------------------------
void block::destroy() {
    for (auto& [id, command] : commands)
        command->destroy(device, cmd_pools);

    for (auto i = 0u; i < cmd_pools.size(); ++i)
        device->call().vkDestroyCommandPool(device->get(),
                                            cmd_pools.at(i),
                                            memory::instance().alloc());

    cmd_pools.clear();
    cmd_order.clear();
    commands.clear();
}

//-----------------------------------------------------------------------------
id block::add_cmd(command::process_func func, bool active) {
    auto cmd = command::make();
    cmd->on_process = func;
    cmd->active = active;

    if (device && !cmd_pools.empty())
        if (!cmd->create(device, get_frame_count(), cmd_pools))
            return undef_id;

    auto result = cmd->get_id();

    commands.emplace(result, cmd);
    cmd_order.push_back(commands.at(result).get());

    return result;
}

//-----------------------------------------------------------------------------
void block::remove_cmd(id::ref cmd_id) {
    if (!commands.count(cmd_id))
        return;

    auto command = commands.at(cmd_id);
    command->destroy(device, cmd_pools);

    remove(cmd_order, (command::c_ptr)(command.get()));

    commands.erase(cmd_id);
}

//-----------------------------------------------------------------------------
bool block::process(index frame) {
    current_frame = frame;

    if (failed(device->call().vkResetCommandPool(device->get(),
                                                 cmd_pools.at(frame),
                                                 0))) {
        log()->error("block reset command pool");
        return false;
    }

    for (auto& command : cmd_order) {
        if (!command->active)
            continue;

        auto& cmd_buf = command->buffers.at(frame);

        VkCommandBufferBeginInfo const begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        if (command->on_process)
            command->on_process(cmd_buf);

        if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool block::activated(id::ref cmd_id) {
    if (!commands.count(cmd_id))
        return false;

    return commands.at(cmd_id)->active;
}

//-----------------------------------------------------------------------------
bool block::set_active(id::ref cmd_id, bool active) {
    if (!commands.count(cmd_id))
        return false;

    commands.at(cmd_id)->active = active;
    return true;
}

} // namespace lava
