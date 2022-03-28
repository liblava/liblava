/**
 * @file         liblava-demo/triangle.cpp
 * @brief        Triangle demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

/// Vertex shader
ui32 vert_shader[] = {
#include "res/triangle/vertex.u32"
};

/// Fragment shader
ui32 frag_shader[] = {
#include "res/triangle/fragment.u32"
};

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    app app("lava triangle", { argc, argv });

    if (!app.setup())
        return error::not_ready;

    mesh::ptr triangle = create_mesh(app.device, mesh_type::triangle);
    if (!triangle)
        return error::create_failed;

    mesh_data& triangle_data = triangle->get_data();
    triangle_data.vertices.at(0).color = v4(1.f, 0.f, 0.f, 1.f);
    triangle_data.vertices.at(1).color = v4(0.f, 1.f, 0.f, 1.f);
    triangle_data.vertices.at(2).color = v4(0.f, 0.f, 1.f, 1.f);

    if (!triangle->reload())
        return error::create_failed;

    graphics_pipeline::ptr pipeline;
    pipeline_layout::ptr layout;

    app.on_create = [&]() {
        pipeline = make_graphics_pipeline(app.device);

        layout = make_pipeline_layout();
        if (!layout->create(app.device))
            return false;

        pipeline->set_layout(layout);

        if (!pipeline->add_shader({ vert_shader, sizeof(vert_shader) }, VK_SHADER_STAGE_VERTEX_BIT))
            return false;

        if (!pipeline->add_shader({ frag_shader, sizeof(frag_shader) }, VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        pipeline->add_color_blend_attachment();

        pipeline->set_vertex_input_binding({ 0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(vertex, color)) },
        });

        render_pass::ptr render_pass = app.shading.get_pass();

        if (!pipeline->create(render_pass->get()))
            return false;

        render_pass->add_front(pipeline);

        pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            triangle->bind_draw(cmd_buf);
        };

        return true;
    };

    app.on_destroy = [&]() {
        pipeline->destroy();
        layout->destroy();
    };

    app.imgui.on_draw = [&]() {
        ImGui::SetNextWindowPos({ 30, 30 }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({ 210, 110 }, ImGuiCond_FirstUseEver);

        ImGui::Begin(app.get_name());

        app.draw_about(false);

        ImGui::End();
    };

    app.add_run_end([&]() {
        triangle->destroy();
    });

    return app.run();
}
