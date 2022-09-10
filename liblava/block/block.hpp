/**
 * @file         liblava/block/block.hpp
 * @brief        Command buffer model
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

/**
 * @brief Block command
 */
struct command : entity {
    /// Shared pointer to command
    using ptr = std::shared_ptr<command>;

    /// Const pointer to command
    using cptr = command const*;

    /// Map of commands
    using map = std::map<id, ptr>;

    /// List of commands
    using clist = std::vector<cptr>;

    /// List of command buffers
    VkCommandBuffers buffers = {};

    /// Command process function
    using process_func = std::function<void(VkCommandBuffer)>;

    /// Called on command process
    process_func on_process;

    /// Active state
    bool active = true;

    /**
     * @brief Create a new command
     * @param device           Vulkan device
     * @param frame_count      Number of frames
     * @param command_pools    List of command pools
     * @return Create was successful or failed
     */
    bool create(device_p device,
                index frame_count,
                VkCommandPools command_pools);

    /**
     * @brief Destroy the command
     * @param device           Vulkan device
     * @param command_pools    List of command pools
     */
    void destroy(device_p device,
                 VkCommandPools command_pools);
};

/**
 * @brief Block of commands
 */
struct block : entity {
    /// Shared pointer to block
    using ptr = std::shared_ptr<block>;

    /// Const pointer to block
    using cptr = command const*;

    /// Map of blocks
    using map = std::map<id, ptr>;

    /// List of blocks
    using clist = std::vector<cptr>;

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
    bool create(device_p device,
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
        return to_index(cmd_pools.size());
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
    void remove_cmd(id::ref cmd);

    /**
     * @brief Remove the command
     * @param cmd    Command id
     */
    void remove_command(id::ref cmd) {
        remove_cmd(cmd);
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
        return current_frame;
    }

    /**
     * @brief Get the command buffer
     * @param cmd                 Command id
     * @return VkCommandBuffer    Vulkan command buffer
     */
    VkCommandBuffer get_command_buffer(id::ref cmd) const {
        return commands.at(cmd)->buffers.at(current_frame);
    }

    /**
     * @brief Get the command buffer
     * @param cmd                 Command id
     * @param frame               Frame index
     * @return VkCommandBuffer    Vulkan command buffer
     */
    VkCommandBuffer get_command_buffer(id::ref cmd, index frame) const {
        return commands.at(cmd)->buffers.at(frame);
    }

    /**
     * @brief Get the buffers
     * @return VkCommandBuffers    List of Vulkan command buffers
     */
    VkCommandBuffers get_buffers() {
        VkCommandBuffers result;

        for (auto& cmd : cmd_order)
            if (cmd->active)
                result.push_back(cmd->buffers.at(current_frame));

        return result;
    }

    /**
     * @brief Get the commands
     * @return command::map const&    Map of commands
     */
    command::map const& get_commands() const {
        return commands;
    }

    /**
     * @brief Get the cmd order
     * @return command::list const&    List of commands
     */
    command::clist const& get_cmd_order() const {
        return cmd_order;
    }

    /**
     * @brief Check if command is activated
     * @param command    Command id
     * @return Command is active or not
     */
    bool activated(id::ref command);

    /**
     * @brief Set the command active
     * @param command    Command id
     * @param active     Active state
     * @return Set was successful or failed
     */
    bool set_active(id::ref command, bool active = true);

    /**
     * @brief Get the device
     * @return device_p    Vulkan device
     */
    device_p get_device() {
        return device;
    }

private:
    /// Vulkan device
    device_p device = nullptr;

    /// Current frame index
    index current_frame = 0;

    /// Command pools
    VkCommandPools cmd_pools = {};

    /// Map of commands
    command::map commands;

    /// Ordered list of commands
    command::clist cmd_order;
};

/// Block type
using block_t = block;

/**
 * @brief Make a new command
 * @return command::ptr    Shared pointer to command
 */
inline command::ptr make_command() {
    return std::make_shared<command>();
}

/**
 * @brief Make a new block
 * @return block::ptr    Shared pointer to block
 */
inline block::ptr make_block() {
    return std::make_shared<block>();
}

} // namespace lava
