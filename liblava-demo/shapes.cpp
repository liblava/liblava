/**
 * @file         liblava-demo/shapes.cpp
 * @brief        Shapes demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <demo.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    app app("shapes", { argc, argv });

    if (!app.setup())
        return error::not_ready;

    // Initialize camera.
    app.camera.position = v3(0.f, -2.f, 4.f);
    app.camera.rotation = v3(-25.f, 0.f, 0.f); // Degrees

    // All shapes will share the same world-space matrix in this example.
    mat4 world_matrix = glm::identity<mat4>();
    v3 rotation_vector = v3{ 0, 0, 0 };

    buffer world_matrix_buffer;
    if (!world_matrix_buffer.create_mapped(app.device, &world_matrix, sizeof(world_matrix),
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    // All shapes will share the same rotation value.
    buffer rotation_buffer;
    if (!rotation_buffer.create_mapped(app.device, &rotation_vector, sizeof(rotation_vector),
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    // Initialize meshes. By default, these make lava::vertex vertices.
    std::array<generic_mesh<>::ptr, 3> meshes;

    generic_mesh<>::ptr triangle;
    triangle = generic_create_mesh(app.device, mesh_type::triangle);
    if (!triangle)
        return error::create_failed;
    meshes[0] = triangle;

    generic_mesh<>::ptr quad;
    quad = generic_create_mesh(app.device, mesh_type::quad);
    if (!quad)
        return error::create_failed;
    meshes[1] = quad;

    generic_mesh<>::ptr cube;
    // This cube definition does not have normals:
    // cube = generic_create_mesh<lava::vertex, float, void, false>(app.device, mesh_type::cube);
    // This cube definition has normals:
    cube = generic_create_mesh(app.device, mesh_type::cube);
    if (!cube)
        return error::create_failed;
    meshes[2] = cube;

    // Color all meshes red.
    for (auto& shape : meshes) {
        for (auto& vert : shape->get_data().vertices) {
            for (size_t i = 0; i < 3; i++) {
                vert.color = v4(1.f, 0.f, 0.f, 1.f);
            }
        }
        if (!shape->reload())
            return error::create_failed;
    }

    // A descriptor is needed for representing the world-space matrix.
    descriptor::ptr descriptor_layout;
    descriptor::pool::ptr descriptor_pool;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    graphics_pipeline::ptr pipeline;
    pipeline_layout::ptr pipeline_layout;

    app.on_create = [&]() {
        pipeline = make_graphics_pipeline(app.device);
        pipeline->add_color_blend_attachment();
        pipeline->set_depth_test_and_write();
        pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS_OR_EQUAL);

        // All shapes use the same simple shaders.
        if (!pipeline->add_shader(file_data("shapes/vert.spirv"), VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!pipeline->add_shader(file_data("shapes/frag.spirv"), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        pipeline->set_vertex_input_binding({ 0, sizeof(lava::vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        // Only send position and color to shaders for this demo.
        // TODO: Update this comment.
        pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, position) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(lava::vertex, color) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(lava::vertex, normal) },
        });

        // Descriptor sets must be made to transfer the shapes' world matrix and the camera's
        // view matrix to the physical device.
        descriptor_layout = make_descriptor();
        descriptor_layout->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       VK_SHADER_STAGE_VERTEX_BIT); // View matrix
        descriptor_layout->add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       VK_SHADER_STAGE_VERTEX_BIT); // World matrix
        descriptor_layout->add_binding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                       VK_SHADER_STAGE_VERTEX_BIT); // Rotation vector
        if (!descriptor_layout->create(app.device))
            return false;
        descriptor_pool = make_descriptor_pool();
        if (!descriptor_pool->create(app.device, { { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } }))
            return false;

        pipeline_layout = make_pipeline_layout();
        pipeline_layout->add(descriptor_layout);
        if (!pipeline_layout->create(app.device))
            return false;
        pipeline->set_layout(pipeline_layout);

        descriptor_set = descriptor_layout->allocate(descriptor_pool->get());
        VkWriteDescriptorSet const write_desc_ubo_camera{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = app.camera.get_descriptor_info(),
        };
        VkWriteDescriptorSet const write_desc_ubo_world{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 1,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = world_matrix_buffer.get_descriptor_info(),
        };
        VkWriteDescriptorSet const write_desc_ubo_rotation{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = 2,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = rotation_buffer.get_descriptor_info(),
        };
        app.device->vkUpdateDescriptorSets({ write_desc_ubo_camera, write_desc_ubo_world,
                                             write_desc_ubo_rotation });

        render_pass::ptr render_pass = app.shading.get_pass();

        if (!pipeline->create(render_pass->get()))
            return false;

        // Push this render pass to the pipeline.
        render_pass->add_front(pipeline);

        // Return after no errors!
        return true;
    };

    app.on_destroy = [&]() {
        descriptor_layout->free(descriptor_set, descriptor_pool->get());
        descriptor_pool->destroy();
        descriptor_layout->destroy();
        pipeline->destroy();
        pipeline_layout->destroy();
    };

    mesh_type current_mesh = mesh_type::cube;

    app.imgui.on_draw = [&]() {
        ImGui::Begin(app.get_name());
        if (ImGui::Button("Triangle")) {
            current_mesh = mesh_type::triangle;
        }
        if (ImGui::Button("Quad")) {
            current_mesh = mesh_type::quad;
        }
        if (ImGui::Button("Cube")) {
            current_mesh = mesh_type::cube;
        }
        if (ImGui::Button("None")) {
            current_mesh = mesh_type::none;
        }
        app.draw_about();
        ImGui::End();
    };

    app.on_update = [&](delta dt) {
        rotation_vector += v3{ 0, 1.f, 0 } * dt;
        memcpy(as_ptr(rotation_buffer.get_mapped_data()), &rotation_vector, sizeof(rotation_vector));

        pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            pipeline_layout->bind(cmd_buf, descriptor_set);
            switch (current_mesh) {
            case mesh_type::cube:
                cube->bind_draw(cmd_buf);
                break;
            case mesh_type::quad:
                quad->bind_draw(cmd_buf);
            case mesh_type::triangle:
                triangle->bind_draw(cmd_buf);
            case mesh_type::none:
                return false;
            }
            return true;
        };

        if (app.camera.activated()) {
            app.camera.update_view(dt, app.input.get_mouse_position());
        }

        return true;
    };

    app.add_run_end([&]() {
        cube->destroy();
    });

    return app.run();
}
