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

/// Draw with separator
constexpr bool const draw_with_separator = true;

/// Draw without separator
constexpr bool const draw_without_separator = false;

/**
 * @brief Application with basic functionality
 */
struct app : frame {
    /**
     * @brief Construct a new app
     *
     * @param env    Frame environment
     */
    explicit app(frame_env env);

    /**
     * @brief Construct a new app
     *
     * @param name        Application name
     * @param cmd_line    Command line arguments
     */
    explicit app(name name, argh::parser cmd_line = {});

    /**
     * @brief Set up the application
     *
     * @return true     Setup was successful
     * @return false    Setup failed
     */
    virtual bool setup();

    /// Main window
    window_t window;

    /// Window input
    input_t input;

    /// ImGui handling
    imgui_t imgui;

    /// ImGui configuration
    imgui::config imgui_config;

    /// Vulkan device
    device_p device = nullptr;

    /// Main camera
    camera_t camera;

    /// Texture staging
    staging_t staging;

    /// Basic block
    block_t block;

    /// Plain renderer
    renderer_t renderer;

    /// Forward shading
    forward_shading shading;

    /// Render target
    render_target::ptr target;

    /// Run time
    run_time_t run_time;

    // File system
    file_system fs;

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
    void draw_about(bool separator = draw_with_separator) const;

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

    /// Setup function
    using setup_func = std::function<bool()>;

    /// Function called on application setup
    setup_func on_setup;

    /**
     * @brief Take screenshot and save it to file
     *
     * @return string    Screenshot file path (empty: failed)
     */
    string screenshot();

    /**
     * @brief Add a tooltip
     *
     * @param n    Name of tooltip
     * @param k    Input key
     * @param m    Input mod (default: none)
     */
    void add_tooltip(name n, key k, mod m = mod::none);

    /**
     * @brief Clear tooltips
     */
    void clear_tooltips() {
        tooltips.clear();
    }

    /**
     * @brief Get tooltips
     *
     * @return tooltip::list    List of tooltips
     */
    tooltip::list get_tooltips() const {
        return tooltips;
    }

private:
    /**
     * @brief Setup file system
     *
     * @param cmd_line    Command line arguments
     *
     * @return true       Setup was successful
     * @return false      Setup failed
     */
    bool setup_file_system(cmd_line cmd_line);

    /**
     * @brief Setup window
     *
     * @param cmd_line    Command line arguments
     *
     * @return true       Setup was successful
     * @return false      Setup failed
     */
    bool setup_window(cmd_line cmd_line);

    /**
     * @brief Setup device
     *
     * @param cmd_line    Command line arguments
     *
     * @return true       Setup was successful
     * @return false      Setup failed
     */
    bool setup_device(cmd_line cmd_line);

    /**
     * @brief Setup render
     *
     * @return true       Setup was successful
     * @return false      Setup failed
     */
    bool setup_render();

    /**
     * @brief Setup run
     *
     * @return true       Setup was successful
     * @return false      Setup failed
     */
    void setup_run();

    /**
     * @brief Handle configuration file
     */
    void handle_config();

    /**
     * @brief Handle inputs
     */
    void handle_input();

    /**
     * @brief Handle key inputs
     */
    void handle_keys();

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

    /// List of tooltips
    tooltip::list tooltips;
};

} // namespace lava
