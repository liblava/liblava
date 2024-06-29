/**
 * @file         liblava/block/block.hpp
 * @brief        Command buffer model
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"

namespace lava {

/**
 * @brief Block command
 */
struct command : entity {
    /// Shared pointer to command
    using s_ptr = std::shared_ptr<command>;

    /// Const pointer to command
    using c_ptr = command const*;

    /// Map of commands
    using s_map = std::map<id, s_ptr>;

    /// List of commands
    using c_list = std::vector<c_ptr>;

    /// List of command buffers
    VkCommandBuffers buffers = {};

    /// Command process function
    using process_func = std::function<void(VkCommandBuffer)>;

    /// Called on command process
    process_func on_process;

    /// Active state
    bool active = true;

    /**
     * @brief Make a new command
     * @return s_ptr    Shared pointer to command
     */
    static s_ptr make() {
        return std::make_shared<command>();
    }

    /**
     * @brief Create a new command
     * @param device           Vulkan device
     * @param frame_count      Number of frames
     * @param command_pools    List of command pools
     * @return Create was successful or failed
     */
    bool create(device::ptr device,
                index frame_count,
                VkCommandPools command_pools);

    /**
     * @brief Destroy the command
     * @param device           Vulkan device
     * @param command_pools    List of command pools
     */
    void destroy(device::ptr device,
                 VkCommandPools command_pools);
};

/**
 * @brief Block of commands
 */
struct block : entity {
    /// Pointer to block
    using ptr = block*;

    /// Shared pointer to block
    using s_ptr = std::shared_ptr<block>;

    /// Const pointer to block
    using c_ptr = block const*;

    /// Map of blocks
    using s_map = std::map<id, s_ptr>;

    /// List of blocks
    using c_list = std::vector<c_ptr>;

    /**
     * @brief Make a new block
     * @return s_ptr    Shared pointer to block
     */
    static s_ptr make() {
        return std::make_shared<block>();
    }

    /**
     * @brief Destroy the block
     */
    ~block() {
        destroy();
    }

    /**
     * @brief Create a new block
     * @param device          Vulkan device
     * @param frame_count     Number of frames
     * @param queue_family    Queue family index
     * @return Create was successful or failed
     */
    bool create(device::ptr device,
                index frame_count,
                index queue_family);

    /**
     * @brief Destroy the block
     */
    void destroy();

    /**
     * @brief Get the frame count
     * @return index    Number of frames
     */
    index get_frame_count() const {
        return to_index(m_cmd_pools.size());
    }

    /**
     * @see add_command
     */
    id add_cmd(command::process_func func, bool active = true);

    /**
     * @brief Add a command
     * @param func      Command function
     * @param active    Active state
     * @return id       Command id
     */
    id add_command(command::process_func func, bool active = true) {
        return add_cmd(func, active);
    }

    /**
     * @see remove_command
     */
    void remove_cmd(id::ref cmd_id);

    /**
     * @brief Remove the command
     * @param cmd_id    Command id
     */
    void remove_command(id::ref cmd_id) {
        remove_cmd(cmd_id);
    }

    /**
     * @brief Process the block
     * @param frame     Frame index
     * @return Process was successful or aborted
     */
    bool process(index frame);

    /**
     * @brief Get the current frame
     * @return index    Current frame
     */
    index get_current_frame() const {
        return m_current_frame;
    }

    /**
     * @brief Get the command buffer
     * @param cmd_id              Command id
     * @return VkCommandBuffer    Vulkan command buffer
     */
    VkCommandBuffer get_command_buffer(id::ref cmd_id) const {
        return m_commands.at(cmd_id)->buffers.at(m_current_frame);
    }

    /**
     * @brief Get the command buffer
     * @param cmd_id              Command id
     * @param frame               Frame index
     * @return VkCommandBuffer    Vulkan command buffer
     */
    VkCommandBuffer get_command_buffer(id::ref cmd_id, index frame) const {
        return m_commands.at(cmd_id)->buffers.at(frame);
    }

    /**
     * @brief Collect the buffers
     * @return VkCommandBuffers    List of Vulkan command buffers
     */
    VkCommandBuffers collect_buffers() {
        VkCommandBuffers result;

        for (auto& cmd : m_cmd_order)
            if (cmd->active)
                result.push_back(cmd->buffers.at(m_current_frame));

        return result;
    }

    /**
     * @brief Get the commands
     * @return command::s_map const&    Map of commands
     */
    command::s_map const& get_commands() const {
        return m_commands;
    }

    /**
     * @brief Get the cmd order
     * @return command::c_list const&    List of commands
     */
    command::c_list const& get_cmd_order() const {
        return m_cmd_order;
    }

    /**
     * @brief Check if command is activated
     * @param cmd_id    Command id
     * @return Command is active or not
     */
    bool activated(id::ref cmd_id);

    /**
     * @brief Set the command active
     * @param cmd_id    Command id
     * @param active    Active state
     * @return Set was successful or failed
     */
    bool set_active(id::ref cmd_id, bool active = true);

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

private:
    /// Vulkan device
    device::ptr m_device = nullptr;

    /// Current frame index
    index m_current_frame = 0;

    /// Command pools
    VkCommandPools m_cmd_pools = {};

    /// Map of commands
    command::s_map m_commands;

    /// Ordered list of commands
    command::c_list m_cmd_order;
};

} // namespace lava
