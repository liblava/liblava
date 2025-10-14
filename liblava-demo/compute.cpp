/**
 * @file         liblava-demo/compute.cpp
 * @brief        compute demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/lava.hpp"
#include <chrono>

using namespace lava;

#ifdef LAVA_DEMO
LAVA_STAGE(7, "compute") {
#else
int main(int argc, char* argv[]) {
    argh::parser argh(argc, argv);
#endif

    engine app("lava compute", argh);
    if (!app.setup())
        return error::not_ready;
    app.props.add("compute", "compute/mandelbrot.comp");
    app.props.add(_vertex_, "compute/present.vert");
    app.props.add(_fragment_, "compute/present.frag");

    pipeline_layout::s_ptr compute_layout;
    compute_pipeline::s_ptr compute_pipeline;
    VkDescriptorSet compute_descriptor_set = VK_NULL_HANDLE;
    descriptor::s_ptr compute_descriptor;

    pipeline_layout::s_ptr graphics_layout;
    render_pipeline::s_ptr graphics_pipeline;
    VkDescriptorSet graphics_descriptor_set = VK_NULL_HANDLE;
    descriptor::s_ptr graphics_descriptor;
    render_pass::s_ptr graphics_render_pass = render_pass::make(app.device);

    descriptor::pool::s_ptr descriptor_pool;

    VkSampler sampler = VK_NULL_HANDLE;

    struct Params {
        float time = 0.0f;
    } params;

    buffer params_buffer;
    image::s_ptr storage_image;

    auto start_time = std::chrono::high_resolution_clock::now();

    app.on_create = [&]() {
        // sampler
        VkSamplerCreateInfo const sampler_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST};
        if (!app.device->vkCreateSampler(&sampler_info, &sampler))
            return false;

        // buffer
        params_buffer.create_mapped(app.device,
                                    &params,
                                    sizeof(params),
                                    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

        // storage texture
        storage_image = image::make(VK_FORMAT_R8G8B8A8_UNORM);
        storage_image->set_usage(VK_IMAGE_USAGE_STORAGE_BIT
                                 | VK_IMAGE_USAGE_SAMPLED_BIT
                                 | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        storage_image->create(app.device, app.window.get_size());
        one_time_submit(app.device, app.device->graphics_queue(), [&](VkCommandBuffer cmd_buf) {
            insert_image_memory_barrier(app.device, cmd_buf, storage_image->get(), 0, VK_ACCESS_SHADER_WRITE_BIT,
                                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
                                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                        storage_image->get_subresource_range());
        });

        // descriptor pool
        descriptor_pool = descriptor::pool::make();
        descriptor_pool->create(app.device, {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}}, 2);

        // compute descriptor set
        compute_descriptor = descriptor::make();
        compute_descriptor->add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                        VK_SHADER_STAGE_COMPUTE_BIT);
        compute_descriptor->add_binding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                        VK_SHADER_STAGE_COMPUTE_BIT);
        if (!compute_descriptor->create(app.device))
            return false;

        compute_descriptor_set = compute_descriptor->allocate(descriptor_pool->get());
        if (!compute_descriptor_set)
            return false;
        std::vector<VkWriteDescriptorSet> write_sets;
        for (descriptor::binding::s_ptr const& binding : compute_descriptor->get_bindings()) {
            VkDescriptorSetLayoutBinding const& info = binding->get();
            write_sets.push_back({.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                  .dstSet = compute_descriptor_set,
                                  .dstBinding = info.binding,
                                  .descriptorCount = info.descriptorCount,
                                  .descriptorType = info.descriptorType});
        }

        VkDescriptorImageInfo storage_image_descriptor_info = {
            .sampler = sampler,
            .imageView = storage_image->get_view(),
            .imageLayout = VK_IMAGE_LAYOUT_GENERAL};
        write_sets[0].pBufferInfo = params_buffer.get_descriptor_info();
        write_sets[1].pImageInfo = &storage_image_descriptor_info;
        app.device->vkUpdateDescriptorSets(to_ui32(write_sets.size()),
                                           write_sets.data());

        // graphics descriptor set
        graphics_descriptor = descriptor::make();
        graphics_descriptor->add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                         VK_SHADER_STAGE_FRAGMENT_BIT);
        graphics_descriptor->create(app.device);
        graphics_descriptor_set = graphics_descriptor->allocate(descriptor_pool->get());
        if (!graphics_descriptor_set)
            return false;
        std::vector<VkWriteDescriptorSet> write_sets_graphics;
        for (descriptor::binding::s_ptr const& binding : graphics_descriptor->get_bindings()) {
            VkDescriptorSetLayoutBinding const& info = binding->get();
            write_sets_graphics.push_back({.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                           .dstSet = graphics_descriptor_set,
                                           .dstBinding = info.binding,
                                           .descriptorCount = info.descriptorCount,
                                           .descriptorType = info.descriptorType});
        }

        write_sets_graphics[0].pImageInfo = &storage_image_descriptor_info;
        app.device->vkUpdateDescriptorSets(to_ui32(write_sets_graphics.size()),
                                           write_sets_graphics.data());

        // compute pipeline
        compute_layout = pipeline_layout::make();
        compute_layout->add(compute_descriptor);
        if (!compute_layout->create(app.device))
            return false;
        compute_pipeline = compute_pipeline::make(app.device, app.pipeline_cache);
        if (!compute_pipeline->set_shader_stage({app.producer.get_shader("compute")},
                                                VK_SHADER_STAGE_COMPUTE_BIT))
            return false;
        compute_pipeline->set_layout(compute_layout);
        if (!compute_pipeline->create())
            return false;

        // graphics pipeline
        graphics_render_pass = app.shading.get_pass();
        graphics_layout = pipeline_layout::make();
        graphics_layout->add(graphics_descriptor);
        if (!graphics_layout->create(app.device))
            return false;
        graphics_pipeline = render_pipeline::make(app.device, app.pipeline_cache);
        if (!graphics_pipeline->add_shader(app.producer.get_shader(_vertex_),
                                           VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!graphics_pipeline->add_shader(app.producer.get_shader(_fragment_),
                                           VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;
        graphics_pipeline->set_layout(graphics_layout);
        graphics_pipeline->set_auto_size(true);
        graphics_pipeline->add_color_blend_attachment();
        graphics_pipeline->set_rasterization_cull_mode(VK_CULL_MODE_FRONT_BIT);
        graphics_pipeline->set_rasterization_front_face(VK_FRONT_FACE_COUNTER_CLOCKWISE);
        if (!graphics_pipeline->create(graphics_render_pass->get())) {
            return false;
        }

        graphics_render_pass->add_front(graphics_pipeline);

        graphics_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            scoped_label label(cmd_buf, "presentation");
            graphics_pipeline->bind(cmd_buf);
            graphics_layout->bind(cmd_buf, graphics_descriptor_set);
            app.device->call().vkCmdDraw(cmd_buf, 3, 1, 0, 0);
            return true;
        };

        return true;
    };

    app.on_process = [&](VkCommandBuffer cmd_buf, lava::index frame) {
        // compute dispatch
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = end - start_time;
        auto ellapsed = elapsed.count();
        vkCmdUpdateBuffer(cmd_buf, params_buffer.get(), 0, sizeof(params.time), &ellapsed);
        auto window_size = app.window.get_size();
        scoped_label label(cmd_buf, "compute");
        compute_pipeline->bind(cmd_buf);
        compute_layout->bind(cmd_buf, compute_descriptor_set, 0, {}, VK_PIPELINE_BIND_POINT_COMPUTE);
        app.device->call().vkCmdDispatch(cmd_buf, (window_size.x + 15) / 16, (window_size.y + 15) / 16, 1);

        // barrier between compute pass and graphics pass
        insert_image_memory_barrier(app.device, cmd_buf, storage_image->get(), VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, storage_image->get_subresource_range());
    };

    app.on_destroy = [&]() {
        app.device->vkDestroySampler(sampler);
        sampler = VK_NULL_HANDLE;

        params_buffer.destroy();
        storage_image->destroy();

        compute_pipeline->destroy();
        compute_layout->destroy();

        graphics_pipeline->destroy();
        graphics_layout->destroy();
        graphics_render_pass->destroy();

        compute_descriptor->destroy();
        graphics_descriptor->destroy();
        descriptor_pool->destroy();
    };

    return app.run();
}
