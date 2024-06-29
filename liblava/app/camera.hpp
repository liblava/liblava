/**
 * @file         liblava/app/camera.hpp
 * @brief        First Person / Look At camera
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/frame/gamepad.hpp"
#include "liblava/frame/input.hpp"
#include "liblava/resource/buffer.hpp"

namespace lava {

/**
 * @brief First Person / Look At camera
 */
struct camera : entity {
    /// Pointer to camera
    using ptr = camera*;

    /// Shared pointer to camera
    using s_ptr = std::shared_ptr<camera>;

    /// Map of cameras
    using s_map = std::map<id, s_ptr>;

    /// List of cameras
    using s_list = std::vector<s_ptr>;

    /**
     * @brief Camera modes
     */
    enum class mode : index {
        first_person = 0,
        look_at,
    };

    /**
     * @brief Create a camera
     * @param device    Vulkan device
     * @return Create was successful or failed
     */
    bool create(device::ptr device);

    /**
     * @brief Destroy the camera
     */
    void destroy();

    /**
     * @brief Update the projection
     */
    void update_projection();

    /**
     * @brief Update the view with mouse position
     * @param dt           Delta time
     * @param mouse_pos    Mouse position
     */
    void update_view(delta dt, mouse_position mouse_pos);

    /**
     * @brief Update the view with gamepad
     * @param dt     Delta time
     * @param pad    Gamepad
     */
    void update_view(delta dt, gamepad::ref pad);

    /**
     * @brief Get the camera's 4x4 view matrix
     * @return mat4    View matrix
     */
    mat4 get_view() const;

    /**
     * @brief Get the camera's 4x4 projection matrix
     * @return mat4    Projection matrix
     */
    mat4 get_projection() const;

    /**
     * @brief Calc the camera's combined 4x4 view/projection matrix
     * @return mat4    Combined view/projection matrix
     */
    mat4 calc_view_projection() const;

    /**
     * @brief Handle key event
     * @param event     Key event
     * @return Event was handled or ignored
     */
    bool handle(key_event::ref event);

    /**
     * @brief Handle mouse button event
     * @param event        Mouse button event
     * @param mouse_pos    Mouse position
     * @return Event was handled or ignored
     */
    bool handle(mouse_button_event::ref event,
                mouse_position mouse_pos);

    /**
     * @brief Handle scroll event
     * @param event     Scroll event
     * @return Event was handled or ignored
     */
    bool handle(scroll_event::ref event);

    /**
     * @brief Check if camera is valid
     * @return Camera is valid or not
     */
    bool valid() const {
        return m_data ? m_data->valid() : false;
    }

    /**
     * @brief Get the descriptor buffer info
     * @return VkDescriptorBufferInfo const*    Descriptor buffer info
     */
    VkDescriptorBufferInfo const* get_descriptor_info() const {
        return m_data ? m_data->get_descriptor_info() : nullptr;
    }

    /**
     * @brief Upload camera state
     */
    void upload();

    /**
     * @brief Stop camera movement
     */
    void stop();

    /**
     * @brief Reset camera
     */
    void reset();

    /**
     * @brief Set camera active
     * @param value    Active state
     */
    void set_active(bool value = true) {
        m_active = value;
    }

    /**
     * @brief Check if camera is activated
     * @return Camera is active or not
     */
    bool activated() const {
        return m_active;
    }

    /**
     * @brief Check if camera is moving
     * @return Camera is moving or does not move
     */
    bool moving() const {
        return m_move_up || m_move_down || m_move_left || m_move_right;
    }

    /**
     * @brief Set keys for moving this camera
     * @param up       Up inputs
     * @param down     Down inputs
     * @param left     Left inputs
     * @param right    Right inputs
     */
    void set_movement_keys(keys_ref up, keys_ref down,
                           keys_ref left, keys_ref right) {
        m_up_keys = up;
        m_down_keys = down;
        m_left_keys = left;
        m_right_keys = right;
    }

    /// Camera position
    v3 position = v3(0.f);

    /// Camera rotation
    v3 rotation = v3(0.f);

    /// Camera rotation speed
    r32 rotation_speed = 20.f;

    /// Camera movement speed
    r32 movement_speed = 1.f;

    /// Camera zoom speed
    r32 zoom_speed = 20.f;

    /// Field of view
    r32 fov = 60.f;

    /// Distance to near clipping plane along the -Z axis
    r32 z_near = 0.1f;

    /// Distance to far clipping plane along the -Z axis
    r32 z_far = 256.f;

    /// Camera aspect ratio
    r32 aspect_ratio = 1.77f;

    /// Camera mode
    mode mode = mode::first_person;

    /// Lock Z axis movement
    bool lock_z = false;

    /// Lock camera rotation
    bool lock_rotation = false;

private:
    /**
     * @brief Move first person
     * @param dt    Delta time
     */
    void move_first_person(delta dt);

    /// Active state
    bool m_active = true;

    /// Up movement
    bool m_move_up = false;

    /// Down movement
    bool m_move_down = false;

    /// Left movement
    bool m_move_left = false;

    /// Right movement
    bool m_move_right = false;

    /// Rotation state
    bool m_rotate = false;

    /// Translate state
    bool m_translate = false;

    /// Mouse X position
    r64 m_mouse_pos_x = 0.0;

    /// Mouse Y position
    r64 m_mouse_pos_y = 0.0;

    /// Scroll position
    r64 m_scroll_pos = 0.0;

    /// Up input keys
    keys m_up_keys{key::w};

    /// Down input keys
    keys m_down_keys{key::s};

    /// Left input keys
    keys m_left_keys{key::a};

    /// Right input keys
    keys m_right_keys{key::d};

    /// Vulkan buffer
    buffer::s_ptr m_data;

    /// Buffer size (projection + view)
    size_t m_size = sizeof(mat4) * 2;

    /// Projection matrix
    mat4 m_projection = mat4(0.f);

    /// View matrix
    mat4 m_view = mat4(0.f);
};

} // namespace lava
