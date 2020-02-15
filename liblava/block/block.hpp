// file      : liblava/block/block.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

struct command : id_obj {

    using map = std::map<id, command>;
    using list = std::vector<command*>;

    VkCommandBuffers buffers = {};

    using func = std::function<void(VkCommandBuffer)>;
    func on_func;

    bool create(device_ptr device, index frame_count, VkCommandPools command_pools);
    void destroy(device_ptr device, VkCommandPools command_pools);
};

struct block : id_obj {

    using ptr = std::shared_ptr<block>;
    using map = std::map<id, block>;
    using list = std::vector<block*>;

    ~block() { destroy(); }

    bool create(device_ptr device, index frame_count, index queue_family);
    void destroy();

    auto get_frame_count() const { return to_index(cmd_pools.size()); }

    id add_cmd(command::func func);
    id add_command(command::func func) { return add_cmd(func); }

    void remove_cmd(id::ref cmd);
    void remove_command(id::ref cmd) { remove_cmd(cmd); }

    bool process(index frame);
    
    auto get_current_frame() const { return current_frame; }
    auto get_command_buffer(id::ref cmd) const { return commands.at(cmd).buffers.at(current_frame); }
    auto get_command_buffer(id::ref cmd, index frame) const { return commands.at(cmd).buffers.at(frame); }

    auto get_buffers() {

        VkCommandBuffers result;

        for (auto& cmd : cmd_order)
            result.push_back(cmd->buffers.at(current_frame));

        return result;
    }

    auto const& get_commands() const { return commands; }
    auto const& get_cmd_order() const { return cmd_order; }

    auto get_device() { return device; }

private:
    device_ptr device = nullptr;

    index current_frame = 0;
    VkCommandPools cmd_pools = {};

    command::map commands;
    command::list cmd_order;
};

inline block::ptr make_block() { return std::make_shared<block>(); }

} // lava
