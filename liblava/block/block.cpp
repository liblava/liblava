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
            logger()->error("create command buffers");
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
    m_device = dev;

    m_current_frame = 0;

    m_cmd_pools.resize(frame_count);

    for (auto i = 0u; i < frame_count; ++i) {
        VkCommandPoolCreateInfo const create_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = 0,
            .queueFamilyIndex = queue_family,
        };
        if (failed(m_device->call().vkCreateCommandPool(m_device->get(),
                                                        &create_info,
                                                        memory::instance().alloc(),
                                                        &m_cmd_pools.at(i)))) {
            logger()->error("create block command pool");
            return false;
        }
    }

    for (auto& [id, command] : m_commands)
        if (!command->create(m_device, frame_count, m_cmd_pools))
            return false;

    return true;
}

//-----------------------------------------------------------------------------
void block::destroy() {
    for (auto& [id, command] : m_commands)
        command->destroy(m_device, m_cmd_pools);

    for (auto i = 0u; i < m_cmd_pools.size(); ++i)
        m_device->call().vkDestroyCommandPool(m_device->get(),
                                              m_cmd_pools.at(i),
                                              memory::instance().alloc());

    m_cmd_pools.clear();
    m_cmd_order.clear();
    m_commands.clear();
}

//-----------------------------------------------------------------------------
id block::add_cmd(command::process_func func, bool active) {
    auto cmd = command::make();
    cmd->on_process = func;
    cmd->active = active;

    if (m_device && !m_cmd_pools.empty())
        if (!cmd->create(m_device, get_frame_count(), m_cmd_pools))
            return undef_id;

    auto result = cmd->get_id();

    m_commands.emplace(result, cmd);
    m_cmd_order.push_back(m_commands.at(result).get());

    return result;
}

//-----------------------------------------------------------------------------
void block::remove_cmd(id::ref cmd_id) {
    if (!m_commands.count(cmd_id))
        return;

    auto command = m_commands.at(cmd_id);
    command->destroy(m_device, m_cmd_pools);

    remove(m_cmd_order, (command::c_ptr)(command.get()));

    m_commands.erase(cmd_id);
}

//-----------------------------------------------------------------------------
bool block::process(index frame) {
    m_current_frame = frame;

    if (failed(m_device->call().vkResetCommandPool(m_device->get(),
                                                   m_cmd_pools.at(frame),
                                                   0))) {
        logger()->error("block reset command pool");
        return false;
    }

    for (auto& command : m_cmd_order) {
        if (!command->active)
            continue;

        auto& cmd_buf = command->buffers.at(frame);

        VkCommandBufferBeginInfo const begin_info{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        if (failed(m_device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        if (command->on_process)
            command->on_process(cmd_buf);

        if (failed(m_device->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
bool block::activated(id::ref cmd_id) {
    if (!m_commands.count(cmd_id))
        return false;

    return m_commands.at(cmd_id)->active;
}

//-----------------------------------------------------------------------------
bool block::set_active(id::ref cmd_id, bool active) {
    if (!m_commands.count(cmd_id))
        return false;

    m_commands.at(cmd_id)->active = active;
    return true;
}

} // namespace lava
