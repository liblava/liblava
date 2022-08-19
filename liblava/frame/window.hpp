/**
 * @file         liblava/frame/window.hpp
 * @brief        Window
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/data.hpp>
#include <liblava/core/math.hpp>
#include <liblava/frame/input.hpp>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <optional>

/// fwd
struct GLFWwindow;

namespace lava {

/**
 * @brief Window
 */
struct window : entity {
    /**
     * @brief Window state
     */
    struct state {
        /// Reference to window state
        using ref = state const&;

        /// Optional window state
        using optional = std::optional<window::state>;

        /**
         * @brief Construct a new state
         */
        explicit state() {}

        /// Window X position
        i32 x = 0;

        /// Window Y position
        i32 y = 0;

        /// Window width
        ui32 width = 0;

        /// Window height
        ui32 height = 0;

        /// Fullscreen window
        bool fullscreen = false;

        /// Floating window
        bool floating = false;

        /// Resizable window
        bool resizable = true;

        /// Decorated window
        bool decorated = true;

        /// Maximized window
        bool maximized = false;

        /// Monitor of window
        index monitor = 0;
    };

    /// Shared pointer to window
    using ptr = std::shared_ptr<window>;

    /// Window event function
    using event = std::function<void(ptr)>;

    /// Map of windows
    using map = std::map<id, ptr>;

    /// Reference to window
    using ref = window const&;

    /**
     * @brief Construct a new window
     */
    window() = default;

    /**
     * @brief Construct a new window
     * @param title    Title of window
     */
    explicit window(name title)
    : title(title) {}

    /**
     * @brief Create a new window with optional state
     * @param state     Window state
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create(state::optional state = {});

    /**
     * @brief Destroy the window
     */
    void destroy();

    /**
     * @brief Get the window state
     * @return state    Window state
     */
    state get_state() const;

    /**
     * @brief Set the window title
     * @param text    Title of window
     */
    void set_title(string_ref text);

    /**
     * @brief Get the window title
     * @return name    Title of window
     */
    string_ref get_title() const {
        return title;
    }

    /**
     * @brief Set the save name
     * @param save    Save name of window
     */
    void set_save_name(string_ref save) {
        save_name = save;
    }

    /**
     * @brief Get the save name
     * @return name    Save name of window
     */
    string_ref get_save_name() const {
        return save_name;
    }

    /**
     * @brief Set the position of window
     * @param x    X positoin
     * @param y    Y position
     */
    void set_position(i32 x, i32 y);

    /**
     * @brief Get the position of window
     * @param x    X position
     * @param y    Y position
     */
    void get_position(i32& x, i32& y) const;

    /**
     * @brief Set the size of window
     * @param width     Window width
     * @param height    Window height
     */
    void set_size(ui32 width, ui32 height);

    /**
     * @brief Get the size of window
     * @param width     Window width
     * @param height    Window height
     */
    void get_size(ui32& width, ui32& height) const;

    /**
     * @brief Get the framebuffer size
     * @param width     Framebuffer width
     * @param height    Framebuffer height
     */
    void get_framebuffer_size(ui32& width, ui32& height) const;

    /**
     * @brief Get the size
     * @return uv2    Size of window
     */
    uv2 get_size() const;

    /**
     * @brief Get the framebuffer size
     * @return uv2    Size of framebuffer
     */
    uv2 get_framebuffer_size() const;

    /**
     * @brief Set the mouse position
     * @param x    Mouse X position
     * @param y    Mouse Y position
     */
    void set_mouse_position(r64 x, r64 y);

    /**
     * @brief Get the mouse position
     * @param x    Mouse X position
     * @param y    Mouse Y position
     */
    void get_mouse_position(r64& x, r64& y) const;

    /**
     * @brief Get the mouse position in window
     * @return mouse_position    Position of mouse
     */
    mouse_position get_mouse_position() const;

    /**
     * @brief Hide mouse cursor
     */
    void hide_mouse_cursor();

    /**
     * @brief Show mouse cursor
     */
    void show_mouse_cursor();

    /**
     * @brief Get the aspect ratio of window
     * @return float    Aspect ratio
     */
    float get_aspect_ratio() const;

    /**
     * @brief Show the window
     */
    void show();

    /**
     * @brief Hide the window
     */
    void hide();

    /**
     * @brief Check if window is visible
     * @return true     Window is visible
     * @return false    Window is invisible
     */
    bool visible() const;

    /**
     * @brief Iconify the window
     */
    void iconify();

    /**
     * @brief Check if the window is iconified
     * @return true     Window is iconified
     * @return false    Window is not iconfied
     */
    bool iconified() const;

    /**
     * @brief Restore the window
     */
    void restore();

    /**
     * @brief Maximize the window
     */
    void maximize();

    /**
     * @brief Check if the window is maximized
     * @return true     Window is maximized
     * @return false    Window is not maximized
     */
    bool maximized() const;

    /**
     * @brief Focus the window
     */
    void focus();

    /**
     * @brief Check if the window is focused
     * @return true     Window is focused
     * @return false    Window is not focused
     */
    bool focused() const;

    /**
     * @brief Set the window to fullscreen
     * @param active    Fullscreen or windowed mode
     */
    void set_fullscreen(bool active) {
        if (fullscreen_active != active)
            switch_mode_request_active = true;
    }

    /**
     * @brief Check if the window is fullscreen
     * @return true     Window is fullscreen
     * @return false    Window is not fullscreen
     */
    bool fullscreen() const {
        return fullscreen_active;
    }

    /**
     * @brief Check if mouse hovered over the window
     * @return true     Mouse hovered
     * @return false    Mouse not hovered
     */
    bool hovered() const;

    /**
     * @brief Check if the window is resizable
     * @return true     Window is resizable
     * @return false    Window is not resizable
     */
    bool resizable() const;

    /**
     * @brief Set the window resizable
     * @param value    Resizable state
     */
    void set_resizable(bool value);

    /**
     * @brief Check if the window is decorated
     * @return true     Window is decorated
     * @return false    Window is not decorated
     */
    bool decorated() const;

    /**
     * @brief Set the window decorated
     * @param value    Decorated state
     */
    void set_decorated(bool value);

    /**
     * @brief Check if the window is floating
     * @return true     Window is floating
     * @return false    Window is not floating
     */
    bool floating() const;

    /**
     * @brief Set the window floating
     * @param value    Floating state
     */
    void set_floating(bool value);

    /**
     * @brief Check if the window request to close
     * @return true     Window has close request
     * @return false    No close request
     */
    bool close_request() const;

    /**
     * @brief Check if the window request to switch mode
     * @return true     Window has switch mode request
     * @return false    No switch mode request
     */
    bool switch_mode_request() const {
        return switch_mode_request_active;
    }

    /**
     * @brief Switch mode of the window
     * @param state     Target window state
     * @return true     Switch was successful
     * @return false    Switch failed
     */
    bool switch_mode(state::optional state = {});

    /**
     * @brief Get GLFW handle
     * @return GLFWwindow*    GLFW window handle
     */
    GLFWwindow* get() const {
        return handle;
    }

    /**
     * @brief Check if the window request to resize
     * @return true     Window has resize request
     * @return false    No resize request
     */
    bool resize_request() const {
        return resize_request_active;
    }

    /**
     * @brief Handle window resize
     * @return true     Resize was successful
     * @return false    Resize failed
     */
    bool handle_resize() {
        if (on_resize)
            if (!on_resize(framebuffer_width,
                           framebuffer_height))
                return false;

        resize_request_active = false;
        return true;
    }

    /**
     * @brief Update window state
     */
    void update_state() {
        get_position(pos_x, pos_y);
        get_size(width, height);
    }

    /// Resize window function
    using resize_func = std::function<bool(ui32, ui32)>;

    /// Called on window resize
    resize_func on_resize;

    /**
     * @brief Assign input callback
     * @param callback    Input callbacl
     */
    void assign(input* callback) {
        input = callback;
    }

    /**
     * @brief Show the save title in the window
     * @param value    Save title state
     */
    void show_save_title(bool value = true) {
        save_title_active = value;
    }

    /**
     * @brief Check the show save title state
     * @return true     Save title is active
     * @return false    Save title is inactive
     */
    bool save_title() const {
        return save_title_active;
    }

    /**
     * @brief Update the window title
     */
    void update_title() {
        set_title(title);
    }

    /**
     * @brief Create a surface
     * @return VkSurfaceKHR    Vulkan surface
     */
    VkSurfaceKHR create_surface();

    /**
     * @brief Set the window icon
     * @param data    Image data
     * @param size    Image size
     */
    void set_icon(data_cptr data, uv2 size);

    /**
     * @brief Get the monitor index of the window
     * @return index    Monitor index
     */
    index get_monitor() const;

    /**
     * @brief Center the window on the monitor
     */
    void center();

private:
    /**
     * @brief Handle window message
     */
    void handle_message();

    /**
     * @brief Handle window mouse message
     */
    void handle_mouse_message();

    /// GLFW window handle
    GLFWwindow* handle = nullptr;

    /// Input handling
    input_t* input = nullptr;

    /// Window title
    string title = _lava_;

    /// Window save name
    string save_name = _default_;

    /// Fullscreen state
    bool fullscreen_active = false;

    /// Save title state
    bool save_title_active = false;

    /// Switch mode request state
    bool switch_mode_request_active = false;

    /// Resize request state
    bool resize_request_active = false;

    /// Framebuffer width
    ui32 framebuffer_width = 0;

    /// Framebuffer height
    ui32 framebuffer_height = 0;

    /// Window X position
    i32 pos_x = 0;

    /// Window Y position
    i32 pos_y = 0;

    /// Window width
    ui32 width = 0;

    /// Window height
    ui32 height = 0;
};

/// Window type
using window_t = window;

/**
 * @brief Create a new surface
 * @param window           GLFW window handle
 * @return VkSurfaceKHR    Vulkan surface
 */
VkSurfaceKHR create_surface(GLFWwindow* window);

/**
 * @brief Get the window by GLFW handle
 * @param handle      GLFW window handle
 * @return window*    Assigned Window
 */
window* get_window(GLFWwindow* handle);

} // namespace lava
