/**
 * @file         liblava-demo/triangle.cpp
 * @brief        Triangle demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <cstddef>
#include <demo.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    app app("lava triangle", { argc, argv });

    if (!app.setup())
        return error::not_ready;

    // Initialize a lava triangle.
    generic_mesh<>::ptr lava_triangle;
    lava_triangle = generic_create_mesh<>(app.device, mesh_type::triangle);
    if (!lava_triangle)
        return error::create_failed;
    auto& lava_triangle_data = lava_triangle->get_data();
    lava_triangle_data.vertices.at(0).color = v4(1.f, 0.f, 0.f, 1.f);
    lava_triangle_data.vertices.at(1).color = v4(0.f, 1.f, 0.f, 1.f);
    lava_triangle_data.vertices.at(2).color = v4(0.f, 0.f, 1.f, 1.f);
    lava_triangle_data.scale<>(0.5f);
    lava_triangle_data.move({ 0.5f, 0, 0 });
    if (!lava_triangle->reload())
        return error::create_failed;

    // Initialize an int triangle.
    typedef struct {
        std::array<int, 3> position;
        lava::v4 color;
    } int_vertex;
    generic_mesh<int_vertex>::ptr int_triangle;
    int_triangle = generic_create_mesh<int_vertex, int>(app.device, mesh_type::triangle, offsetof(int_vertex, position));
    if (!int_triangle)
        return error::create_failed;
    auto& int_triangle_data = int_triangle->get_data();
    int_triangle_data.vertices.at(0).color = v4(1.f, 0.5f, 0.5f, 1.f);
    int_triangle_data.vertices.at(1).color = v4(0.5f, 1.f, 0.5f, 1.f);
    int_triangle_data.vertices.at(2).color = v4(0.5f, 0.5f, 1.f, 1.f);
    int_triangle_data.scale<int>(2, offsetof(int_vertex, position));
    if (!int_triangle->reload())
        return error::create_failed;

    // Initialize a double triangle.
    typedef struct {
        std::array<double, 3> position;
        lava::v4 color;
    } double_vertex;
    generic_mesh<double_vertex>::ptr double_triangle;
    double_triangle = generic_create_mesh<double_vertex, double>(app.device, mesh_type::triangle, offsetof(double_vertex, position));
    if (!double_triangle)
        return error::create_failed;
    auto& double_triangle_data = double_triangle->get_data();
    double_triangle_data.vertices.at(0).color = v4(1.f, 0.f, 0.5f, 1.f);
    double_triangle_data.vertices.at(1).color = v4(0.f, 1.f, 0.5f, 1.f);
    double_triangle_data.vertices.at(2).color = v4(0.f, 0.5f, 1.f, 1.f);
    int_triangle_data.scale<double>(0.854, offsetof(double_vertex, position));
    if (!double_triangle->reload())
        return error::create_failed;

    graphics_pipeline::ptr lava_pipeline;
    graphics_pipeline::ptr int_pipeline;
    graphics_pipeline::ptr double_pipeline;
    pipeline_layout::ptr lava_pipeline_layout;
    pipeline_layout::ptr int_pipeline_layout;
    pipeline_layout::ptr double_pipeline_layout;

    app.on_create = [&]() {
        render_pass::ptr render_pass = app.shading.get_pass();

        // Making a lava triangle pipeline.
        lava_pipeline = make_graphics_pipeline(app.device);
        lava_pipeline->add_color_blend_attachment();
        lava_pipeline_layout = make_pipeline_layout();
        if (!lava_pipeline_layout->create(app.device))
            return false;
        lava_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            lava_triangle->bind_draw(cmd_buf);
        };

        // Making an int triangle pipeline.
        int_pipeline = make_graphics_pipeline(app.device);
        int_pipeline->add_color_blend_attachment();
        int_pipeline_layout = make_pipeline_layout();
        if (!int_pipeline_layout->create(app.device))
            return false;
        int_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            int_triangle->bind_draw(cmd_buf);
        };

        // Making an double triangle pipeline.
        double_pipeline = make_graphics_pipeline(app.device);
        double_pipeline->add_color_blend_attachment();
        double_pipeline_layout = make_pipeline_layout();
        if (!double_pipeline_layout->create(app.device))
            return false;
        double_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            double_triangle->bind_draw(cmd_buf);
        };

        // Describe the lava triangle.
        if (!lava_pipeline->add_shader(file_data("generic_triangle/lava_triangle.spirv"), VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!lava_pipeline->add_shader(file_data("generic_triangle/triangle_frag.spirv"), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;
        lava_pipeline->set_vertex_input_binding({ 0, sizeof(lava::vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        lava_pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(lava::vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(lava::vertex, color)) },
        });
        lava_pipeline->set_layout(lava_pipeline_layout);
        if (!lava_pipeline->create(render_pass->get()))
            return false;

        // Describe the int triangle.
        if (!int_pipeline->add_shader(file_data("generic_triangle/int_triangle.spirv"), VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!int_pipeline->add_shader(file_data("generic_triangle/triangle_frag.spirv"), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;
        int_pipeline->set_vertex_input_binding({ 0, sizeof(int_vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        int_pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SINT, to_ui32(offsetof(int_vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(int_vertex, color)) },
        });
        int_pipeline->set_layout(int_pipeline_layout);
        if (!int_pipeline->create(render_pass->get()))
            return false;

        // Describe the double triangle.
        if (!double_pipeline->add_shader(file_data("generic_triangle/double_triangle.spirv"), VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!double_pipeline->add_shader(file_data("generic_triangle/triangle_frag.spirv"), VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;
        double_pipeline->set_vertex_input_binding({ 0, sizeof(double_vertex), VK_VERTEX_INPUT_RATE_VERTEX });
        double_pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R64G64B64_SFLOAT, to_ui32(offsetof(double_vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, to_ui32(offsetof(double_vertex, color)) },
        });
        double_pipeline->set_layout(double_pipeline_layout);
        if (!double_pipeline->create(render_pass->get()))
            return false;

        render_pass->add_front(lava_pipeline);
        render_pass->add_front(double_pipeline);
        render_pass->add_front(int_pipeline);

        // No errors so far!
        return true;
    };

    app.on_destroy = [&]() {
        lava_pipeline->destroy();
        lava_pipeline_layout->destroy();
        int_pipeline->destroy();
        int_pipeline_layout->destroy();
        double_pipeline->destroy();
        double_pipeline_layout->destroy();
    };

    app.add_run_end([&]() {
        lava_triangle->destroy();
        int_triangle->destroy();
        double_triangle->destroy();
    });

    return app.run();
}
