/**
 * @file         liblava/app/camera.cpp
 * @brief        First Person / Look At camera
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/app/camera.hpp"

namespace lava {

//-----------------------------------------------------------------------------
bool camera::create(device::ptr device) {
    update_projection();

    m_data = buffer::make();

    return m_data->create_mapped(device,
                                 &m_projection, m_size,
                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

//-----------------------------------------------------------------------------
void camera::destroy() {
    if (!m_data)
        return;

    m_data->destroy();
    m_data = nullptr;
}

//-----------------------------------------------------------------------------
void camera::move_first_person(delta dt) {
    v3 front;
    front.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    front.y = sin(glm::radians(rotation.x));
    front.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));

    front = glm::normalize(front);

    auto speed = dt * movement_speed * 2.f;

    if (m_move_up) {
        if (lock_z)
            position -= glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f)))
                        * speed;
        else
            position -= (front * speed);
    }

    if (m_move_down) {
        if (lock_z)
            position += glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f)))
                        * speed;
        else
            position += (front * speed);
    }

    if (m_move_left)
        position += glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f)))
                    * speed;

    if (m_move_right)
        position -= glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f)))
                    * speed;
}

//-----------------------------------------------------------------------------
void camera::update_view(delta dt, mouse_position mouse_pos) {
    if (m_translate || m_rotate) {
        auto dx = m_mouse_pos_x - mouse_pos.x;
        auto dy = m_mouse_pos_y - mouse_pos.y;

        if (m_rotate && !lock_rotation) {
            auto speed = dt * rotation_speed;
            rotation += v3(dy * speed, -dx * speed, 0.f);
        }

        if (m_translate) {
            auto speed = dt * movement_speed;
            position -= v3(-dx * speed, -dy * speed, 0.f);
        }

        m_mouse_pos_x = mouse_pos.x;
        m_mouse_pos_y = mouse_pos.y;
    }

    if ((m_scroll_pos != 0.0)) {
        auto speed = dt * zoom_speed;
        position -= v3(-0.f, 0.f, m_scroll_pos * speed);
        m_scroll_pos = 0.0;
    }

    if (mode == mode::first_person && moving()) {
        move_first_person(dt);
    }

    auto rot_m = mat4(1.f);
    rot_m = glm::rotate(rot_m, glm::radians(rotation.x), v3(1.f, 0.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.y), v3(0.f, 1.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.z), v3(0.f, 0.f, 1.f));

    auto trans_m = glm::translate(mat4(1.f), -position);

    if (mode == mode::first_person)
        m_view = rot_m * trans_m;
    else
        m_view = trans_m * rot_m;

    if (valid())
        memcpy(data::as_ptr(m_data->get_mapped_data()) + sizeof(mat4), &m_view, sizeof(mat4));
}

//-----------------------------------------------------------------------------
void camera::update_view(delta dt, gamepad::ref pad) {
    const auto dead_zone = 0.2f;
    const auto range = 1.f - dead_zone;

    if (mode == mode::first_person) {
        v3 front;
        front.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
        front.y = sin(glm::radians(rotation.x));
        front.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
        front = glm::normalize(front);

        // move

        auto movement_factor = dt * movement_speed * 2.f;

        auto axis_left_y = pad.value(gamepad_axis::left_y);
        if (fabsf(axis_left_y) > dead_zone) {
            auto pos = (fabsf(axis_left_y) - dead_zone) / range;
            if (lock_z)
                position -= glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f)))
                            * ((axis_left_y < 0.f) ? 1.f : -1.f) * movement_factor;
            else
                position -= front * pos * ((axis_left_y < 0.f) ? 1.f : -1.f)
                            * movement_factor;
        }

        auto axis_left_x = pad.value(gamepad_axis::left_x);
        if (fabsf(axis_left_x) > dead_zone) {
            auto pos = (fabsf(axis_left_x) - dead_zone) / range;
            position += glm::normalize(glm::cross(front, glm::vec3(0.f, 1.f, 0.f)))
                        * pos * ((axis_left_x < 0.f) ? 1.f : -1.f) * movement_factor;
        }
    }

    // rotate

    if (lock_rotation)
        return;

    auto rotation_factor = dt * rotation_speed * 2.5f;

    auto axis_right_x = pad.value(gamepad_axis::right_x);
    if (fabsf(axis_right_x) > dead_zone) {
        auto pos = (fabsf(axis_right_x) - dead_zone) / range;
        rotation.y += pos * ((axis_right_x < 0.f) ? -1.f : 1.f) * rotation_factor;
    }

    auto axis_right_y = pad.value(gamepad_axis::right_y);
    if (fabsf(axis_right_y) > dead_zone) {
        auto pos = (fabsf(axis_right_y) - dead_zone) / range;
        rotation.x -= pos * ((axis_right_y < 0.f) ? -1.f : 1.f) * rotation_factor;
    }
}

//-----------------------------------------------------------------------------
void camera::update_projection() {
    m_projection = glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);

    if (valid())
        memcpy(data::as_ptr(m_data->get_mapped_data()), &m_projection, sizeof(mat4));
}

//-----------------------------------------------------------------------------
mat4 camera::get_view() const {
    return m_view;
}
//-----------------------------------------------------------------------------
mat4 camera::get_projection() const {
    return m_projection;
}

//-----------------------------------------------------------------------------
mat4 camera::calc_view_projection() const {
    return m_projection * m_view;
}

//-----------------------------------------------------------------------------
void camera::upload() {
    memcpy(m_data->get_mapped_data(), &m_projection, m_size);
}

//-----------------------------------------------------------------------------
bool camera::handle(key_event::ref event) {
    auto pressed_key = event.key;
    const auto check_key = [&](key_ref current_key, key_ref testing_key, bool& value) -> bool {
        if (current_key == testing_key) {
            value = event.active();
            return input_done;
        }
        return input_ignore;
    };

    for (auto const& current_key : m_up_keys) {
        if (check_key(current_key, pressed_key, m_move_up))
            return input_done;
    }
    for (auto const& current_key : m_down_keys) {
        if (check_key(current_key, pressed_key, m_move_down))
            return input_done;
    }
    for (auto const& current_key : m_left_keys) {
        if (check_key(current_key, pressed_key, m_move_left))
            return input_done;
    }
    for (auto const& current_key : m_right_keys) {
        if (check_key(current_key, pressed_key, m_move_right))
            return input_done;
    }

    return input_ignore;
}

//-----------------------------------------------------------------------------
bool camera::handle(mouse_button_event::ref event, mouse_position mouse_pos) {
    m_rotate = event.pressed(mouse_button::left);
    m_translate = event.pressed(mouse_button::right);

    if (m_rotate || m_translate) {
        m_mouse_pos_x = mouse_pos.x;
        m_mouse_pos_y = mouse_pos.y;

        return input_done;
    }

    return input_ignore;
}

//-----------------------------------------------------------------------------
bool camera::handle(scroll_event::ref event) {
    m_scroll_pos += event.offset.y;

    return input_done;
}

//-----------------------------------------------------------------------------
void camera::stop() {
    m_move_up = false;
    m_move_down = false;
    m_move_left = false;
    m_move_right = false;

    m_rotate = false;
    m_translate = false;

    m_mouse_pos_x = 0.0;
    m_mouse_pos_y = 0.0;
    m_scroll_pos = 0.0;
}

//-----------------------------------------------------------------------------
void camera::reset() {
    position = v3(0.f);
    rotation = v3(0.f);
}

} // namespace lava
