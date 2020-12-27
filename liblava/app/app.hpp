// file      : liblava/app/app.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/app/camera.hpp>
#include <liblava/app/config.hpp>
#include <liblava/app/forward_shading.hpp>
#include <liblava/block.hpp>
#include <liblava/frame.hpp>

namespace lava {

    struct app : frame {
        explicit app(frame_config config);
        explicit app(name name, argh::parser cmd_line = {});

        bool setup();

        lava::window window;
        lava::input input;

        lava::gui gui;
        gui::config gui_config;

        device_ptr device = nullptr;
        lava::camera camera;

        lava::staging staging;
        lava::block block;

        renderer plotter;
        forward_shading shading;
        render_target::ptr target;

        lava::run_time run_time;

        using update_func = std::function<bool(delta)>;
        update_func on_update;

        using create_func = std::function<bool()>;
        create_func on_create;

        using destroy_func = std::function<void()>;
        destroy_func on_destroy;

        bool v_sync() const {
            return config.v_sync;
        }
        ui32 get_frame_counter() const {
            return frame_counter;
        }

        void draw_about(bool separator = true) const;

        app_config config;
        json_file config_file;

        using process_func = std::function<void(VkCommandBuffer, index)>;
        process_func on_process;

        id::ref block_cmd() const {
            return block_command;
        }

    private:
        void handle_config();
        void handle_input();
        void handle_window();

        void update();
        void render();

        bool create_gui();
        void destroy_gui();

        bool create_target();
        void destroy_target();

        bool create_block();

        texture::ptr fonts;

        bool toggle_v_sync = false;
        ui32 frame_counter = 0;

        json_file::callback config_callback;

        id block_command;
    };

} // namespace lava
