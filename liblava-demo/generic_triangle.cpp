/**
 * @file         liblava-demo/triangle.cpp
 * @brief        Triangle demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <cstddef>
#include <demo.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    app app("lava triangle", { argc, argv });

    setup_imgui_font_icons(app.config.imgui_font);

    if (!app.setup())
        return error::not_ready;

    generic_mesh<lava::vertex>::ptr lava_triangle;
    lava_triangle = generic_create_mesh<lava::vertex, float>(app.device, mesh_type::triangle, offsetof(lava::vertex, position));

    if (!lava_triangle)
        return error::create_failed;

    auto& lava_triangle_data = lava_triangle->get_data();
    lava_triangle_data.vertices.at(0).color = v4(1.f, 0.f, 0.f, 1.f);
    lava_triangle_data.vertices.at(1).color = v4(0.f, 1.f, 0.f, 1.f);
    lava_triangle_data.vertices.at(2).color = v4(0.f, 0.f, 1.f, 1.f);

    if (!lava_triangle->reload())
        return error::create_failed;

    graphics_pipeline::ptr pipeline;
    pipeline_layout::ptr layout;

    app.on_create = [&]() {
        pipeline = make_graphics_pipeline(app.device);
        pipeline->add_color_blend_attachment();
        layout = make_pipeline_layout();
        if (!layout->create(app.device))
            return false;

        pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            lava_triangle->bind_draw(cmd_buf);
        };

        // Describe the lava triangle.
        if (!pipeline->add_shader(file_data("generic_triangle/lava_triangle.spirv"), VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!pipeline->add_shader(file_data("generic_triangle/triangle_frag.spirv"), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;
        pipeline->set_vertex_input_binding({ 0, sizeof(lava::vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(lava::vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(lava::vertex, color)) },
        });

        render_pass::ptr render_pass = app.shading.get_pass();
        pipeline->set_layout(layout);
        if (!pipeline->create(render_pass->get()))
            return false;
        render_pass->add_front(pipeline);
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
        lava_triangle->destroy();
    });

    return app.run();
}
