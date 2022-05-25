/**
 * @file         liblava-demo/spawn.cpp
 * @brief        Spawn demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    engine app("lava spawn", { argc, argv });

    app.prop.add(_vertex_, "spawn/spawn.vert");
    app.prop.add(_fragment_, "spawn/spawn.frag");

    setup_imgui_font_icons(app.config.imgui_font,
                           FONT_ICON_FILE_NAME_FAS,
                           ICON_MIN_FA, ICON_MAX_FA);
    app.prop.add(_font_icon_, app.config.imgui_font.icon_file);

    app.prop.add("spawn", "spawn/lava-spawn-game.obj");

    app.add_tooltip("first person", key::enter);
    app.add_tooltip("lock rotation", key::r);
    app.add_tooltip("lock z", key::z);

    if (!app.setup())
        return error::not_ready;

    timer load_timer;

    mesh::ptr spawn_mesh = app.producer.get_mesh("spawn");
    if (!spawn_mesh)
        return error::create_failed;

    ms mesh_load_time = load_timer.elapsed();

    texture::ptr default_texture = app.producer.create_texture({ 4096, 4096 });
    if (!default_texture)
        return error::create_failed;

    app.camera.position = v3(0.832f, 0.036f, 2.304f);
    app.camera.rotation = v3(8.42f, -29.73f, 0.f);

    mat4 spawn_model = glm::identity<mat4>();

    buffer spawn_model_buffer;
    if (!spawn_model_buffer.create_mapped(app.device, &spawn_model, sizeof(mat4), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    graphics_pipeline::ptr pipeline;
    pipeline_layout::ptr layout;

    descriptor::ptr descriptor;
    descriptor::pool::ptr descriptor_pool;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    app.on_create = [&]() {
        pipeline = make_graphics_pipeline(app.device);
        if (!pipeline->add_shader(app.producer.get_shader(_vertex_), VK_SHADER_STAGE_VERTEX_BIT))
            return false;

        if (!pipeline->add_shader(app.producer.get_shader(_fragment_), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        pipeline->add_color_blend_attachment();

        pipeline->set_depth_test_and_write();
        pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS_OR_EQUAL);

        pipeline->set_vertex_input_binding({ 0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(vertex, color)) },
            { 2, 0, VK_FORMAT_R32G32_SFLOAT, to_ui32(offsetof(vertex, uv)) },
        });

        descriptor = make_descriptor();
        descriptor->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        descriptor->add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
        descriptor->add_binding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        if (!descriptor->create(app.device))
            return false;

        descriptor_pool = make_descriptor_pool();
        if (!descriptor_pool->create(app.device, {
                                                     { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
                                                     { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2 },
                                                 }))
            return false;

        layout = make_pipeline_layout();
        layout->add(descriptor);

        if (!layout->create(app.device))
            return false;

        pipeline->set_layout(layout);

        descriptor_set = descriptor->allocate(descriptor_pool->get());

        VkWriteDescriptorSet const write_desc_ubo_camera{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = app.camera.get_descriptor_info(),
        };

        VkWriteDescriptorSet const write_desc_ubo_spawn{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 1,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = spawn_model_buffer.get_descriptor_info(),
        };

        VkWriteDescriptorSet const write_desc_sampler{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 2,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = default_texture->get_descriptor_info(),
        };

        app.device->vkUpdateDescriptorSets({ write_desc_ubo_camera, write_desc_ubo_spawn, write_desc_sampler });

        render_pass::ptr render_pass = app.shading.get_pass();

        if (!pipeline->create(render_pass->get()))
            return false;

        render_pass->add_front(pipeline);

        pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            layout->bind(cmd_buf, descriptor_set);

            spawn_mesh->bind_draw(cmd_buf);
        };

        return true;
    };

    app.on_destroy = [&]() {
        descriptor->free(descriptor_set, descriptor_pool->get());

        descriptor_pool->destroy();
        descriptor->destroy();

        pipeline->destroy();
        layout->destroy();
    };

    v3 spawn_position{ 0.f };
    v3 spawn_rotation{ 0.f };
    v3 spawn_scale{ 1.f };

    bool update_spawn_matrix = false;

    app.imgui.on_draw = [&]() {
        ImGui::SetNextWindowPos({ 30, 30 }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({ 330, 485 }, ImGuiCond_FirstUseEver);

        ImGui::Begin(app.get_name());

        imgui_left_spacing(2);

        ImGui::TextUnformatted(icon(ICON_FA_CHESS_ROOK));

        ImGui::SameLine(0.f, 15.f);

        ImGui::Text("load: %.3f sec", to_sec(mesh_load_time));

        ImGui::Spacing();

        update_spawn_matrix |= ImGui::DragFloat3("position##spawn", (r32*) &spawn_position, 0.01f);
        update_spawn_matrix |= ImGui::DragFloat3("rotation##spawn", (r32*) &spawn_rotation, 0.1f);
        update_spawn_matrix |= ImGui::DragFloat3("scale##spawn", (r32*) &spawn_scale, 0.1f);

        ImGui::Spacing();

        imgui_left_spacing();

        ImGui::Text("vertices: %d", spawn_mesh->get_vertices_count());

        ImGui::SameLine();

        uv2 texture_size = default_texture->get_size();
        ImGui::Text("texture: %d x %d", texture_size.x, texture_size.y);

        ImGui::Separator();

        ImGui::Spacing();

        imgui_left_spacing(2);

        ImGui::TextUnformatted(icon(ICON_FA_VIDEO));

        ImGui::SameLine(0.f, 15.f);

        bool camera_active = app.camera.activated();
        if (ImGui::Checkbox("active", &camera_active))
            app.camera.set_active(camera_active);

        ImGui::SameLine(0.f, 10.f);

        bool first_person = app.camera.mode == camera_mode::first_person;
        if (ImGui::Checkbox("first person##camera", &first_person))
            app.camera.mode = first_person ? camera_mode::first_person : camera_mode::look_at;

        ImGui::Spacing();

        ImGui::DragFloat3("position##camera", (r32*) &app.camera.position, 0.01f);
        ImGui::DragFloat3("rotation##camera", (r32*) &app.camera.rotation, 0.1f);

        ImGui::Spacing();

        ImGui::Checkbox("lock rotation##camera", &app.camera.lock_rotation);

        ImGui::SameLine(0.f, 10.f);

        ImGui::Checkbox("lock z##camera", &app.camera.lock_z);

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("speed")) {
            ImGui::DragFloat("movement##camera", &app.camera.movement_speed, 0.1f);
            ImGui::DragFloat("rotation##camera", &app.camera.rotation_speed, 0.1f);
            ImGui::DragFloat("zoom##camera", &app.camera.zoom_speed, 0.1f);
        }

        if (ImGui::CollapsingHeader("projection")) {
            bool update_projection = false;

            update_projection |= ImGui::DragFloat("fov", &app.camera.fov);
            update_projection |= ImGui::DragFloat("z near", &app.camera.z_near);
            update_projection |= ImGui::DragFloat("z far", &app.camera.z_far);
            update_projection |= ImGui::DragFloat("aspect", &app.camera.aspect_ratio);

            if (update_projection)
                app.camera.update_projection();
        }

        app.draw_about();

        ImGui::End();
    };

    app.input.key.listeners.add([&](key_event::ref event) {
        if (app.imgui.capture_mouse())
            return false;

        if (event.pressed(key::enter)) {
            app.camera.mode = app.camera.mode == camera_mode::first_person ? camera_mode::look_at : camera_mode::first_person;
            return true;
        }

        if (event.pressed(key::r))
            app.camera.lock_rotation = !app.camera.lock_rotation;

        if (event.pressed(key::z))
            app.camera.lock_z = !app.camera.lock_z;

        return false;
    });

    gamepad pad(gamepad_id::_1);

    app.on_update = [&](delta dt) {
        if (app.camera.activated()) {
            app.camera.update_view(dt, app.input.get_mouse_position());

            if (pad.ready() && pad.update())
                app.camera.update_view(dt, pad);
        }

        if (update_spawn_matrix) {
            spawn_model = glm::translate(mat4(1.f), spawn_position);

            spawn_model = glm::rotate(spawn_model, glm::radians(spawn_rotation.x), v3(1.f, 1.f, 0.f));
            spawn_model = glm::rotate(spawn_model, glm::radians(spawn_rotation.y), v3(0.f, 1.f, 0.f));
            spawn_model = glm::rotate(spawn_model, glm::radians(spawn_rotation.z), v3(0.f, 0.f, 1.f));

            spawn_model = glm::scale(spawn_model, spawn_scale);

            memcpy(as_ptr(spawn_model_buffer.get_mapped_data()), &spawn_model, sizeof(mat4));

            update_spawn_matrix = false;
        }

        return true;
    };

    app.add_run_end([&]() {
        spawn_model_buffer.destroy();
    });

    return app.run();
}
