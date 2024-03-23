/**
 * @file         liblava/fwd.hpp
 * @brief        Forward declarations
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

namespace lava {

// liblava/app.hpp
struct app;
struct benchmark_data;
struct camera;
struct app_config;
struct forward_shading;
struct imgui;

// liblava/asset.hpp
struct image_loader;

// liblava/base.hpp
struct target_callback;
struct vk_result;
struct device_table;
struct device;
struct instance_info;
struct instance;
struct allocator;
struct memory;
struct physical_device;
struct platform;
struct profile_info;
struct queue;
struct queue_info;
struct queue_family_info;

// liblava/block.hpp
struct attachment;
struct command;
struct block;
struct descriptor;
struct pipeline_layout;
struct pipeline;
struct compute_pipeline;
struct render_pass;
struct render_pipeline;
struct subpass;
struct subpass_dependency;

// liblava/core.hpp
struct data_provider;
struct data;
struct cdata;
struct unique_data;
struct id;
struct ids;
struct entity;
struct layer;
struct layer_list;
struct timer;
struct run_time;
struct no_copy_no_move;
struct interface;
struct semantic_version;
struct version;

// liblava/engine.hpp
struct engine;
struct producer;
struct property;

// liblava/file.hpp
struct file_guard;
struct file_system;
struct file;
struct file_data;
struct file_callback;
struct json_file;

// liblava/frame.hpp
struct stage;
struct driver;
struct frame_env;
struct frame;
struct key_event;
struct scroll_offset;
struct scroll_event;
struct mouse_position;
struct mouse_move_event;
struct mouse_button_event;
struct mouse_active_event;
struct input_callback;
struct input;
struct gamepad;
struct gamepad_manager;
struct render_target;
struct render_thread;
struct renderer;
struct swapchain;
struct window;

// liblava/resource.hpp
struct buffer;
struct image;
struct vertex;
struct mesh_meta;
struct file_format;
struct texture;
struct staging;

// liblava/util.hpp
struct hex_point;
struct hex_cell;
struct hex_fractional_cell;
struct hex_offset_coord;
struct hex_orientation;
struct hex_layout;
struct hex_grid;
struct log_config;
struct rect;
struct random_generator;
struct pseudo_random_generator;
struct telegram;
struct telegraph;
struct message_dispatcher;
struct thread_pool;

} // namespace lava
