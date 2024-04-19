/**
 * @file         liblava-demo/shapes.cpp
 * @brief        Shapes demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "imgui.h"
#include "liblava/lava.hpp"

using namespace lava;

//-----------------------------------------------------------------------------
#ifdef LAVA_DEMO
LAVA_STAGE(3, "shapes") {
#else
int main(int argc, char* argv[]) {
    argh::parser argh(argc, argv);
#endif

    engine app("lava shapes", argh);

    app.props.add(_vertex_, "shapes/shapes.vert");
    app.props.add(_fragment_, "shapes/shapes.frag");

    if (!app.setup())
        return error::not_ready;

    // initialize camera
    app.camera.position = v3(0.f, -2.f, 4.f);
    app.camera.rotation = v3(-25.f, 0.f, 0.f); // degrees

    // all shapes will share the same world-space matrix in this example
    mat4 world_matrix = glm::identity<mat4>();
    v3 rotation_vector = v3{0, 0, 0};

    buffer world_matrix_buffer;
    if (!world_matrix_buffer.create_mapped(app.device,
                                           &world_matrix,
                                           sizeof(world_matrix),
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    // all shapes will share the same rotation value
    buffer rotation_buffer;
    if (!rotation_buffer.create_mapped(app.device,
                                       &rotation_vector,
                                       sizeof(rotation_vector),
                                       VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    // initialize meshes. By default, these make vertex
    std::array<mesh::ptr, 4> meshes;

    mesh::ptr triangle = create_mesh(app.device, mesh_type::triangle);
    if (!triangle)
        return error::create_failed;
    meshes[0] = triangle;

    mesh::ptr quad = create_mesh(app.device, mesh_type::quad);
    if (!quad)
        return error::create_failed;
    meshes[1] = quad;

    // this cube definition does not have normals:
    // cube = create_mesh<vertex, false, false, true>(app.device, mesh_type::cube);
    // this cube definition does have normals:
    mesh::ptr cube = create_mesh(app.device, mesh_type::cube);
    if (!cube)
        return error::create_failed;
    meshes[2] = cube;

    mesh::ptr hexagon = create_mesh(app.device, mesh_type::hexagon);
    if (!hexagon)
        return error::create_failed;
    meshes[3] = hexagon;

    for (auto& shape : meshes) {
        if (!shape->reload())
            return error::create_failed;
    }

    mesh_type current_mesh = mesh_type::cube;

    // a descriptor is needed for representing the world-space matrix
    descriptor::ptr descriptor;
    descriptor::pool::ptr descriptor_pool;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

    render_pipeline::ptr pipeline;
    pipeline_layout::ptr layout;

    app.on_create = [&]() {
        pipeline = render_pipeline::make(app.device, app.pipeline_cache);
        pipeline->add_color_blend_attachment();
        pipeline->set_depth_test_and_write();
        pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS_OR_EQUAL);

        // all shapes use the same simple shaders
        if (!pipeline->add_shader(app.producer.get_shader(_vertex_),
                                  VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!pipeline->add_shader(app.producer.get_shader(_fragment_),
                                  VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        pipeline->set_vertex_input_binding({0,
                                            sizeof(vertex),
                                            VK_VERTEX_INPUT_RATE_VERTEX});

        // only send position and color to shaders for this demo
        pipeline->set_vertex_input_attributes({
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, position)},
            {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(vertex, color)},
            {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, normal)},
        });

        // descriptor sets must be made to transfer the shapes' world matrix
        // and the camera's view matrix to the physical device
        descriptor = descriptor::make();
        descriptor->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                VK_SHADER_STAGE_VERTEX_BIT); // View matrix
        descriptor->add_binding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                VK_SHADER_STAGE_VERTEX_BIT); // World matrix
        descriptor->add_binding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                VK_SHADER_STAGE_VERTEX_BIT); // Rotation vector
        if (!descriptor->create(app.device))
            return false;

        descriptor_pool = descriptor::pool::make();
        if (!descriptor_pool->create(app.device, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3}}))
            return false;

        layout = pipeline_layout::make();
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
        app.device->vkUpdateDescriptorSets({write_desc_ubo_camera,
                                            write_desc_ubo_world,
                                            write_desc_ubo_rotation});

        render_pass::ptr render_pass = app.shading.get_pass();

        if (!pipeline->create(render_pass->get()))
            return false;

        // push this render pass to the pipeline
        render_pass->add_front(pipeline);

        pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            layout->bind(cmd_buf, descriptor_set);

            switch (current_mesh) {
            case mesh_type::triangle:
                triangle->bind_draw(cmd_buf);
                break;
            case mesh_type::quad:
                quad->bind_draw(cmd_buf);
                break;
            case mesh_type::cube:
                cube->bind_draw(cmd_buf);
                break;
            case mesh_type::hexagon:
                hexagon->bind_draw(cmd_buf);
                break;
            default:
                break;
            }

            return true;
        };

        return true;
    };

    app.on_destroy = [&]() {
        descriptor->free(descriptor_set,
                         descriptor_pool->get());

        descriptor_pool->destroy();
        descriptor->destroy();

        pipeline->destroy();
        layout->destroy();
    };

    app.imgui.layers.add("info", [&]() {
        ImGui::Begin(app.get_name());

        if (ImGui::Selectable("Triangle", current_mesh == mesh_type::triangle))
            current_mesh = mesh_type::triangle;
        if (ImGui::Selectable("Quad", current_mesh == mesh_type::quad))
            current_mesh = mesh_type::quad;
        if (ImGui::Selectable("Cube", current_mesh == mesh_type::cube))
            current_mesh = mesh_type::cube;
        if (ImGui::Selectable("Hexagon", current_mesh == mesh_type::hexagon))
            current_mesh = mesh_type::hexagon;
        if (ImGui::Selectable("None", current_mesh == mesh_type::none))
            current_mesh = mesh_type::none;

        app.draw_about();

        ImGui::End();
    });

    app.on_update = [&](delta dt) {
        rotation_vector += v3{0, 1.f, 0} * dt;
        memcpy(data::as_ptr(rotation_buffer.get_mapped_data()),
               &rotation_vector, sizeof(rotation_vector));

        if (app.camera.activated())
            app.camera.update_view(to_dt(app.run_time.delta),
                                   app.input.get_mouse_position());

        return run_continue;
    };

    app.add_run_end([&]() {
        triangle->destroy();
        quad->destroy();
        cube->destroy();
        hexagon->destroy();
    });

    return app.run();
}
