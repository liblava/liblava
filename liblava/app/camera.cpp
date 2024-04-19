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

    data = buffer::make();

    return data->create_mapped(device,
                               &projection, size,
                               VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

//-----------------------------------------------------------------------------
void camera::destroy() {
    if (!data)
        return;

    data->destroy();
    data = nullptr;
}

//-----------------------------------------------------------------------------
void camera::move_first_person(delta dt) {
    v3 front;
    front.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    front.y = sin(glm::radians(rotation.x));
    front.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));

    front = glm::normalize(front);

    auto speed = dt * movement_speed * 2.f;

    if (move_up) {
        if (lock_z)
            position -= glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f)))
                        * speed;
        else
            position -= (front * speed);
    }

    if (move_down) {
        if (lock_z)
            position += glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f)))
                        * speed;
        else
            position += (front * speed);
    }

    if (move_left)
        position += glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f)))
                    * speed;

    if (move_right)
        position -= glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f)))
                    * speed;
}

//-----------------------------------------------------------------------------
void camera::update_view(delta dt, mouse_position mouse_pos) {
    if (translate || rotate) {
        auto dx = mouse_pos_x - mouse_pos.x;
        auto dy = mouse_pos_y - mouse_pos.y;

        if (rotate && !lock_rotation) {
            auto speed = dt * rotation_speed;
            rotation += v3(dy * speed, -dx * speed, 0.f);
        }

        if (translate) {
            auto speed = dt * movement_speed;
            position -= v3(-dx * speed, -dy * speed, 0.f);
        }

        mouse_pos_x = mouse_pos.x;
        mouse_pos_y = mouse_pos.y;
    }

    if ((scroll_pos != 0.0)) {
        auto speed = dt * zoom_speed;
        position -= v3(-0.f, 0.f, scroll_pos * speed);
        scroll_pos = 0.0;
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
        view = rot_m * trans_m;
    else
        view = trans_m * rot_m;

    if (valid())
        memcpy(data::as_ptr(data->get_mapped_data()) + sizeof(mat4), &view, sizeof(mat4));
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
    projection = glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);

    if (valid())
        memcpy(data::as_ptr(data->get_mapped_data()), &projection, sizeof(mat4));
}

//-----------------------------------------------------------------------------
mat4 camera::get_view() const {
    return view;
}
//-----------------------------------------------------------------------------
mat4 camera::get_projection() const {
    return projection;
}

//-----------------------------------------------------------------------------
mat4 camera::calc_view_projection() const {
    return projection * view;
}

//-----------------------------------------------------------------------------
void camera::upload() {
    memcpy(data->get_mapped_data(), &projection, size);
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

    for (auto const& current_key : up_keys) {
        if (check_key(current_key, pressed_key, move_up))
            return input_done;
    }
    for (auto const& current_key : down_keys) {
        if (check_key(current_key, pressed_key, move_down))
            return input_done;
    }
    for (auto const& current_key : left_keys) {
        if (check_key(current_key, pressed_key, move_left))
            return input_done;
    }
    for (auto const& current_key : right_keys) {
        if (check_key(current_key, pressed_key, move_right))
            return input_done;
    }

    return input_ignore;
}

//-----------------------------------------------------------------------------
bool camera::handle(mouse_button_event::ref event, mouse_position mouse_pos) {
    rotate = event.pressed(mouse_button::left);
    translate = event.pressed(mouse_button::right);

    if (rotate || translate) {
        mouse_pos_x = mouse_pos.x;
        mouse_pos_y = mouse_pos.y;

        return input_done;
    }

    return input_ignore;
}

//-----------------------------------------------------------------------------
bool camera::handle(scroll_event::ref event) {
    scroll_pos += event.offset.y;

    return input_done;
}

//-----------------------------------------------------------------------------
void camera::stop() {
    move_up = false;
    move_down = false;
    move_left = false;
    move_right = false;

    rotate = false;
    translate = false;

    mouse_pos_x = 0.0;
    mouse_pos_y = 0.0;
    scroll_pos = 0.0;
}

//-----------------------------------------------------------------------------
void camera::reset() {
    position = v3(0.f);
    rotation = v3(0.f);
}

} // namespace lava
