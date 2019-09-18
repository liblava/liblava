// file      : liblava/fwd.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

namespace lava {

    // liblava/core.h
    struct data_provider;
    struct data;
    struct scope_data;
    struct id;
    struct ids;
    struct rect;
    struct time_info;
    struct timer;
    struct run_time;
    struct internal_version;
    struct version;

    // liblava/utils.h
    struct file_guard;
    struct file_system;
    struct file;
    struct file_data;
    struct file_dialog;
    struct log_config;
    struct config_file_callback;
    struct config_file;
    struct irandom;
    struct random_generator;
    struct random;
    struct pseudo_random_generator;
    struct telegram;
    struct dispatcher;
    struct thread_pool;

    // liblava/base.h
    struct device_manager;
    struct device;
    struct instance;
    struct allocator;
    struct memory;
    struct physical_device;

    // liblava/resource.h
    struct buffer;
    struct image;
    struct vertex;
    struct mesh_data;
    struct mesh;
    struct texture_file_format;
    struct texture;

    // liblava/frame.h
    struct frame_config;
    struct frame;
    struct key_event;
    struct scroll_offset;
    struct scroll_event;
    struct mouse_button_event;
    struct mouse_position;
    struct mouse_move_event;
    struct mouse_active_event;
    struct input_callback;
    struct input;
    struct render_target;
    struct render_thread;
    struct renderer;
    struct swapchain;
    struct window;

} // lava
