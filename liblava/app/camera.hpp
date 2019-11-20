// file      : liblava/app/camera.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/resource/buffer.hpp>
#include <liblava/frame/input.hpp>

namespace lava {

enum class camera_type : type
{
    first_person,
    look_at,
};

struct camera : id_obj {

    using map = std::map<id, camera>;
    using list = std::vector<camera*>;

    bool create(device_ptr device);
    void destroy();

    void update_projection();
    void update_view(delta dt, mouse_position mouse_pos);
    void update_view(delta dt, gamepad const& pad);

    bool handle(key_event::ref event);
    bool handle(mouse_button_event::ref event, mouse_position mouse_pos);
    bool handle(scroll_event::ref event);
    
    bool is_valid() const { return data ? data->is_valid() : false; }
    VkDescriptorBufferInfo const* get_info() const { return data ? data->get_info() : nullptr; }

    void upload();

    void stop();
    void reset();

    void set_active(bool value = true) { active = value; }
    bool is_active() const { return active; }
    bool moving() const { return up || down || left || right; }

    v3 position = v3(0.f);
    v3 rotation = v3(0.f);

    r32 rotation_speed = 20.f;
    r32 movement_speed = 1.f;
    r32 zoom_speed = 20.f;

    r32 fov = 60.f;
    r32 z_near = 0.1f;
    r32 z_far = 256.f;
    r32 aspect_ratio = 1.77f;

    camera_type type = camera_type::first_person;

    bool lock_z = false;
    bool lock_rotation = false;

private:
    bool active = true;

    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;

    bool rotate = false;
    bool translate = false;

    r64 mouse_pos_x = 0.0;
    r64 mouse_pos_y = 0.0;
    r64 scroll_pos = 0.0;

    buffer::ptr data;
    size_t size = sizeof(mat4) * 2; // projection + view

    mat4 projection = mat4(0.f);
    mat4 view = mat4(0.f);
};

} // lava
