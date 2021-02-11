// file      : liblava/block/block.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
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

        bool active = true;

        bool create(device_ptr device, index frame_count, VkCommandPools command_pools);
        void destroy(device_ptr device, VkCommandPools command_pools);
    };

    struct block : id_obj {
        using ptr = std::shared_ptr<block>;
        using map = std::map<id, block>;
        using list = std::vector<block*>;

        ~block() {
            destroy();
        }

        bool create(device_ptr device, index frame_count, index queue_family);
        void destroy();

        index get_frame_count() const {
            return to_index(cmd_pools.size());
        }

        id add_cmd(command::func func, bool active = true);
        id add_command(command::func func, bool active = true) {
            return add_cmd(func, active);
        }

        void remove_cmd(id::ref cmd);
        void remove_command(id::ref cmd) {
            remove_cmd(cmd);
        }

        bool process(index frame);

        index get_current_frame() const {
            return current_frame;
        }
        VkCommandBuffer get_command_buffer(id::ref cmd) const {
            return commands.at(cmd).buffers.at(current_frame);
        }
        VkCommandBuffer get_command_buffer(id::ref cmd, index frame) const {
            return commands.at(cmd).buffers.at(frame);
        }

        VkCommandBuffers get_buffers() {
            VkCommandBuffers result;

            for (auto& cmd : cmd_order)
                if (cmd->active)
                    result.push_back(cmd->buffers.at(current_frame));

            return result;
        }

        command::map const& get_commands() const {
            return commands;
        }
        command::list const& get_cmd_order() const {
            return cmd_order;
        }

        bool activated(id::ref command);
        bool set_active(id::ref command, bool active = true);

        device_ptr get_device() {
            return device;
        }

    private:
        device_ptr device = nullptr;

        index current_frame = 0;
        VkCommandPools cmd_pools = {};

        command::map commands;
        command::list cmd_order;
    };

    inline block::ptr make_block() {
        return std::make_shared<block>();
    }

} // namespace lava
