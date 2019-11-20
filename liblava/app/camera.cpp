// file      : liblava/app/camera.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/camera.hpp>

namespace lava {

bool camera::create(device_ptr device) {

    update_projection();

    data = buffer::make();

    return data->create_mapped(device, &projection, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

void camera::destroy() {

    if (!data)
        return;

    data->destroy();
    data = nullptr;
}

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

    if (type == camera_type::first_person && moving()) {

        v3 front;
        front.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
        front.y = sin(glm::radians(rotation.x));
        front.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));

        front = glm::normalize(front);

        auto speed = dt * movement_speed * 2.f;

        if (up) {

            if (lock_z)
                position -= glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f))) * speed;
            else
                position -= (front * speed);
        }

        if (down) {

            if (lock_z)
                position += glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f))) * speed;
            else
                position += (front * speed);
        }

        if (left)
            position += glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f))) * speed;

        if (right)
            position -= glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f))) * speed;
    }

    auto rot_m = mat4(1.f);
    rot_m = glm::rotate(rot_m, glm::radians(rotation.x), v3(1.f, 0.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.y), v3(0.f, 1.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.z), v3(0.f, 0.f, 1.f));

    auto trans_m = glm::translate(mat4(1.f), -position);

    if (type == camera_type::first_person)
        view = rot_m * trans_m;
    else
        view = trans_m * rot_m;

    if (is_valid())
        memcpy(as_ptr(data->get_mapped_data()) + sizeof(mat4), &view, sizeof(mat4));
}

void camera::update_view(delta dt, gamepad const& pad) {

    if (type != camera_type::first_person)
        return;

    const r32 dead_zone = 0.2f;
    const r32 range = 1.f - dead_zone;

    v3 front;
    front.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    front.y = sin(glm::radians(rotation.x));
    front.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    front = glm::normalize(front);

    auto movement_factor = dt * movement_speed * 2.f;
    auto rotation_factor = dt * rotation_speed * 2.5f;

    // move

    {
        auto axis_left_y = pad.value(gamepad_axis::left_y);
        if (fabsf(axis_left_y) > dead_zone) {

            auto pos = (fabsf(axis_left_y) - dead_zone) / range;
            if (lock_z)
                position -= glm::normalize(glm::cross(front, v3(1.f, 0.f, 0.f))) * ((axis_left_y < 0.f) ? 1.f : -1.f) * movement_factor;
            else
                position -= front * pos * ((axis_left_y < 0.f) ? 1.f : -1.f) * movement_factor;
        }
    }

    {
        auto axis_left_x = pad.value(gamepad_axis::left_x);
        if (fabsf(axis_left_x) > dead_zone) {

            auto pos = (fabsf(axis_left_x) - dead_zone) / range;
            position += glm::normalize(glm::cross(front, glm::vec3(0.f, 1.f, 0.f))) * pos * ((axis_left_x < 0.f) ? 1.f : -1.f) * movement_factor;
        }
    }

    // rotate
    
    if (lock_rotation)
        return;

    {
        auto axis_right_x = pad.value(gamepad_axis::right_x);
        if (fabsf(axis_right_x) > dead_zone) {

            auto pos = (fabsf(axis_right_x) - dead_zone) / range;
            rotation.y += pos * ((axis_right_x < 0.f) ? -1.f : 1.f) * rotation_factor;
        }
    }

    {
        auto axis_right_y = pad.value(gamepad_axis::right_y);
        if (fabsf(axis_right_y) > dead_zone) {

            auto pos = (fabsf(axis_right_y) - dead_zone) / range;
            rotation.x -= pos * ((axis_right_y < 0.f) ? -1.f : 1.f) * rotation_factor;
        }
    }
}

void camera::update_projection() {

    projection = glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);

    if (is_valid())
        memcpy(as_ptr(data->get_mapped_data()), &projection, sizeof(mat4));
}

void camera::upload() {

    memcpy(data->get_mapped_data(), &projection, size);
}

bool camera::handle(key_event::ref event) {
   
    switch (event.key) 
    {
        case key::w: { up = event.active(); break; }
        case key::s: { down = event.active(); break; }
        case key::a: { left = event.active(); break; }
        case key::d: { right = event.active(); break; }
        default:
            return false;
    }

    return true;
}

bool camera::handle(mouse_button_event::ref event, mouse_position mouse_pos) {

    rotate = event.pressed(mouse_button::left);
    translate = event.pressed(mouse_button::right);

    if (rotate || translate) {

        mouse_pos_x = mouse_pos.x;
        mouse_pos_y = mouse_pos.y;

        return true;
    }

    return false;
}

bool camera::handle(scroll_event::ref event) {

    scroll_pos += event.offset.y;

    return true;
}

void camera::stop() {

    up = false;
    down = false;
    left = false;
    right = false;

    rotate = false;
    translate = false;

    mouse_pos_x = 0.0;
    mouse_pos_y = 0.0;
    scroll_pos = 0.0;
}

void camera::reset() {

    position = v3(0.f);
    rotation = v3(0.f);
}

} // lava
