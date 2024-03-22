/**
 * @file         liblava/app/app.hpp
 * @brief        Application with basic functionality
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/app/benchmark.hpp"
#include "liblava/app/camera.hpp"
#include "liblava/app/config.hpp"
#include "liblava/app/forward_shading.hpp"
#include "liblava/block.hpp"
#include "liblava/frame.hpp"

namespace lava {

/// Draw with separator
constexpr bool const draw_separator = true;

/// Draw without separator
constexpr bool const draw_no_separator = false;

/// Draw with fps
constexpr bool const draw_fps = true;

/// Draw without fps
constexpr bool const draw_no_fps = false;

/// Draw with spacing
constexpr bool const draw_spacing = true;

/// Draw without spacing
constexpr bool const draw_no_spacing = false;

/**
 * @brief Application with basic functionality
 */
struct app : frame {
    /**
     * @brief Construct a new app
     * @param env    Frame environment
     */
    explicit app(frame_env::ref env);

    /**
     * @brief Construct a new app
     * @param name        Application name
     * @param cmd_line    Command line arguments
     */
    explicit app(name name, argh::parser cmd_line = {});

    /**
     * @brief Set up the application
     * @return Setup was successful or failed
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

    /// Tooltip list
    tooltip_list tooltips;

    /// Vulkan device
    device_p device = nullptr;

    /// Main camera
    camera_t camera;

    /// Gamepad
    gamepad pad;

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

    /// File system
    file_system fs;

    /// Pipeline cache
    VkPipelineCache pipeline_cache = nullptr;

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
     * @return V-Sync is active or not
     */
    bool v_sync() const {
        return config.v_sync;
    }

    /**
     * @brief Triple buffering setting
     * @return VK_PRESENT_MODE_MAILBOX_KHR preferred over VK_PRESENT_MODE_IMMEDIATE_KHR or not
     */
    bool triple_buffer() const {
        return config.triple_buffer;
    }

    /**
     * @brief Frames per second cap setting
     * @return Frames per second cap value (deactived: 0)
     */
    ui32 fps_cap() const {
        return config.fps_cap;
    }

    /**
     * @brief Get the frame counter
     * @return ui32    Number of rendered frames
     */
    ui32 get_frame_counter() const {
        return frame_counter;
    }

    /**
     * @brief Draw about information
     * @param separator    Prepend separator
     * @param fps          FPS counter
     * @param spacing      Spacing
     */
    void draw_about(bool separator = draw_separator,
                    bool fps = draw_fps,
                    bool spacing = draw_spacing) const;

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
     * @return id::ref    Id to access the command
     */
    id::ref block_cmd() const {
        return block_command;
    }

    /// Set up function
    using setup_func = std::function<bool()>;

    /// Function called on application setup
    setup_func on_setup;

    /**
     * @brief Take screenshot and save it to file
     * @return string    Screenshot file path (empty: failed)
     */
    string screenshot();

    /**
     * Switch config id
     * @param id    Config id
     * @return Switch was successful of failed
     */
    bool switch_config(string_ref id);

private:
    /**
     * @brief Mount resource paths and files
     */
    void mount_resource();

    /**
     * @brief Set up file system
     * @return Setup was successful or failed
     */
    bool setup_file_system();

    /**
     * @brief Set up window
     * @return Setup was successful or failed
     */
    bool setup_window();

    /**
     * @brief Set up device
     * @return Setup was successful or failed
     */
    bool setup_device();

    /**
     * @brief Set up render
     * @return Setup was successful or failed
     */
    bool setup_render();

    /**
     * @brief Set up run
     */
    void setup_run();

    /**
     * @brief Parse command line
     */
    void parse_cmd_line();

    /**
     * @brief Load configuration file
     * @param config_id    Config id
     * @return Load was successful or failed
     */
    bool load_config(string_ref config_id);

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
     * @return Create was successful or failed
     */
    bool create_imgui();

    /**
     * @brief Destroy ImGui
     */
    void destroy_imgui();

    /**
     * @brief Create a render target
     * @return Create was successful or failed
     */
    bool create_target();

    /**
     * @brief Destroy the render target
     */
    void destroy_target();

    /**
     * @brief Create a block
     * @return Create was successful or failed
     */
    bool create_block();

    /**
     * @brief Create a pipeline cache
     * @return Create was successful or failed
     */
    bool create_pipeline_cache();

    /**
     * @brief Destroy the pipeline cache
     */
    void destroy_pipeline_cache();

    /// Texture for ImGui fonts
    texture::ptr imgui_fonts;

    /// Toggle V-Sync state
    bool toggle_v_sync = false;

    /// Number of frames rendered
    ui32 frame_counter = 0;

    /// Last render time
    us last_render_time{0};

    /// Configuration file callback
    json_file::callback config_callback;

    /// Block command id
    id block_command;

    /// Benchmark frames
    benchmark_data frames;
};

} // namespace lava
