// file      : liblava/app/camera.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/camera.hpp>

namespace lava {

bool camera::create(device* device) {

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

void camera::update_view(milliseconds delta_, mouse_position mouse_pos) {

    auto delta = to_sec(delta_);

    if (translate || rotate) {

        auto dx = mouse_pos_x - mouse_pos.x;
        auto dy = mouse_pos_y - mouse_pos.y;

        if (rotate) {

            auto speed = delta * rotation_speed;
            rotation += v3(dy * speed, -dx * speed, 0.f);
        }

        if (translate) {

            auto speed = delta * movement_speed;
            position -= v3(-dx * speed, -dy * speed, 0.f);
        }

        mouse_pos_x = mouse_pos.x;
        mouse_pos_y = mouse_pos.y;
    }

    if ((scroll_pos != 0.0)) {

        auto speed = delta * zoom_speed;
        position -= v3(-0.f, 0.f, scroll_pos * speed);
        scroll_pos = 0.0;
    }

    if (up || down || left || right) {

        v3 front;
        front.x = to_r32(-cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y)));
        front.y = to_r32(sin(glm::radians(rotation.x)));
        front.z = to_r32(cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y)));

        front = glm::normalize(front);

        auto speed = to_r32(delta * movement_speed * 2.f);

        if (up)
            position -= (front * speed);

        if (down)
            position += (front * speed);

        if (left)
            position += glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f))) * speed;

        if (right)
            position -= glm::normalize(glm::cross(front, v3(0.f, 1.f, 0.f))) * speed;
    }

    auto trans_m = glm::translate(mat4(1.f), -position);

    auto rot_m = mat4(1.f);
    rot_m = glm::rotate(rot_m, glm::radians(rotation.x), v3(1.f, 0.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.y), v3(0.f, 1.f, 0.f));
    rot_m = glm::rotate(rot_m, glm::radians(rotation.z), v3(0.f, 0.f, 1.f));

    view = rot_m * trans_m;

    if (is_valid())
        memcpy(as_ptr(data->get_mapped_data()) + sizeof(mat4), &view, sizeof(mat4));
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
        case input_key::w: { up = event.active(); break; }
        case input_key::s: { down = event.active(); break; }
        case input_key::a: { left = event.active(); break; }
        case input_key::d: { right = event.active(); break; }
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
