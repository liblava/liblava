/**
 * @file         liblava/app/app.hpp
 * @brief        Application with basic functionality
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/app/benchmark.hpp>
#include <liblava/app/camera.hpp>
#include <liblava/app/config.hpp>
#include <liblava/app/forward_shading.hpp>
#include <liblava/block.hpp>
#include <liblava/frame.hpp>

namespace lava {

/**
 * @brief Application with basic functionality
 */
struct app : frame {
    /**
     * @brief Construct a new app
     *
     * @param config    Frame configuration
     */
    explicit app(frame_config config);

    /**
     * @brief Construct a new app
     *
     * @param name        Application name
     * @param cmd_line    Command line arguments
     */
    explicit app(name name, argh::parser cmd_line = {});

    /**
     * @brief Setup the application
     *
     * @return true     Setup was successful
     * @return false    Setup failed
     */
    bool setup();

    /// Main window
    lava::window window;

    /// Window input
    lava::input input;

    /// ImGui handling
    lava::imgui imgui;

    /// ImGui configuration
    imgui::config imgui_config;

    /// Vulkan device
    device_ptr device = nullptr;

    /// Main camera
    lava::camera camera;

    /// Texture staging
    lava::staging staging;

    /// Basic block
    lava::block block;

    /// Plain renderer
    lava::renderer renderer;

    /// Forward shading
    forward_shading shading;

    /// Render target
    render_target::ptr target;

    /// Run time
    lava::run_time run_time;

    /// Update function
    using update_func = std::function<bool(delta)>;

    /// Function called on application update
    update_func on_update;

    /// Create function
    using create_func = std::function<bool()>;

    /// Function called on application create
    create_func on_create;

    /// Destroy function
    using destroy_func = std::function<void()>;

    /// Function called on application destroy
    destroy_func on_destroy;

    /**
     * @brief V-Sync setting
     *
     * @return true     V-Sync is active
     * @return false    V-Sync is inactive
     */
    bool v_sync() const {
        return config.v_sync;
    }

    /**
     * @brief Get the frame counter
     *
     * @return ui32    Number of rendered frames
     */
    ui32 get_frame_counter() const {
        return frame_counter;
    }

    /**
     * @brief Draw about information
     *
     * @param separator    Prepend separator
     */
    void draw_about(bool separator = true) const;

    /// Application configuration
    app_config config;

    /// Configuration file
    json_file config_file;

    /// Process function
    using process_func = std::function<void(VkCommandBuffer, index)>;

    /// Function called on application process
    process_func on_process;

    /**
     * @brief Get id of the block command
     *
     * @return id::ref    Id to access the command
     */
    id::ref block_cmd() const {
        return block_command;
    }

private:
    /**
     * @brief Handle configuration file
     */
    void handle_config();

    /**
     * @brief Handle inputs
     */
    void handle_input();

    /**
     * @brief Handle window states
     */
    void handle_window();

    /**
     * @brief Update the application
     */
    void update();

    /**
     * @brief Render the application
     */
    void render();

    /**
     * @brief Create ImGui
     *
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create_imgui();

    /**
     * @brief Destroy ImGui
     */
    void destroy_imgui();

    /**
     * @brief Create a render target
     *
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create_target();

    /**
     * @brief Destroy the render target
     */
    void destroy_target();

    /**
     * @brief Create a block object
     *
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create_block();

    /// Texture for ImGui fonts
    texture::ptr imgui_fonts;

    /// Toggle V-Sync state
    bool toggle_v_sync = false;

    /// Number of frames rendered
    ui32 frame_counter = 0;

    /// Configuration file callback
    json_file::callback config_callback;

    /// Block command id
    id block_command;

    /// Benchmark frames
    benchmark_data frames;
};

} // namespace lava
