/**
 * @file         liblava-demo/light.cpp
 * @brief        Light demo
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

// structs for interfacing with shaders
namespace glsl {
using namespace glm;
using uint = ui32;
#include "res/light/data.inc"
} // namespace glsl

/// G-Buffer UBO data
glsl::UboData g_ubo;

/**
 * @brief G-Buffer attachment
 */
struct gbuffer_attachment {
    /**
     * @brief Attachment types
     */
    enum type : ui32 {
        albedo = 0,
        normal,
        metallic_roughness,
        depth,
        count
    };

    /// Requested formats
    VkFormats requested_formats;

    /// Image usage flags
    VkImageUsageFlags usage;

    /// Image handle
    image::ptr image_handle;

    /// Render pass attachment
    attachment::ptr renderpass_attachment;

    /// Subpass attachment reference
    VkAttachmentReference subpass_reference;

    /**
     * @brief Create a new G-Buffer attachment
     * @param app       Application
     * @param index     Attachment index
     * @return Create was successful or failed
     */
    bool create(app const& app, ui32 index);
};

/// Array of G-Buffer attachments
using attachment_array = std::array<gbuffer_attachment, gbuffer_attachment::count>;

/// G-Buffer attachments
attachment_array g_attachments = {
    gbuffer_attachment{ { VK_FORMAT_R8G8B8A8_UNORM },
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
    gbuffer_attachment{ { VK_FORMAT_R16G16B16A16_SFLOAT },
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
    gbuffer_attachment{ { VK_FORMAT_R16G16_SFLOAT },
                        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
    gbuffer_attachment{ { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D16_UNORM },
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT },
};

/// Array of lights
using light_array = std::array<glsl::LightData, 3>;

/// Lights
light_array const g_lights = {
    glsl::LightData{ { 2.f, 2.f, 2.5f }, 10.f, { 30.f, 10.f, 10.f } },
    glsl::LightData{ { -2.f, -2.f, -0.5f }, 10.f, { 10.f, 30.f, 10.f } },
    glsl::LightData{ { 0.f, 0.f, -1.5f }, 10.f, { 10.f, 10.f, 30.f } }
};

/**
 * @brief Create a G-Buffer renderpass
 * @param app                  Application
 * @param attachments          Array of attachments
 * @return render_pass::ptr    Shared pointer to render pass
 */
render_pass::ptr create_gbuffer_renderpass(app const& app,
                                           attachment_array& attachments);

//-----------------------------------------------------------------------------
name _tex_normal_ = "tex_normal";
name _tex_roughness_ = "tex_roughness";

name _gbuffer_vertex_ = "gbuffer_vertex";
name _gbuffer_fragment_ = "gbuffer_fragment";
name _lighting_vertex_ = "lighting_vertex";
name _lighting_fragment_ = "lighting_fragment";

//-----------------------------------------------------------------------------
#ifdef LAVA_DEMO
LAVA_STAGE(6, "light") {
#else
int main(int argc, char* argv[]) {
    argh::parser argh(argc, argv);
#endif
    frame_env env("lava light", argh);

    env.profile = profile_desktop_baseline_2022();
    // env.profile = profile_roadmap_2022();

    engine app(env);

    app.prop.add(_tex_normal_, "light/normal.png");
    app.prop.add(_tex_roughness_, "light/roughness.png");

    app.prop.add(_gbuffer_vertex_, "light/gbuffer.vert");
    app.prop.add(_gbuffer_fragment_, "light/gbuffer.frag");
    app.prop.add(_lighting_vertex_, "light/lighting.vert");
    app.prop.add(_lighting_fragment_, "light/lighting.frag");

    if (!app.setup())
        return error::not_ready;

    target_callback resize_callback;
    app.target->add_callback(&resize_callback);

    // create global immutable resources
    // destroyed in app.producer

    mesh::ptr object = app.producer.create_mesh(mesh_type::quad);
    if (!object)
        return error::create_failed;

    using object_array = std::array<mat4, 2>;
    object_array object_instances;

    texture::ptr tex_normal = app.producer.get_texture(_tex_normal_);
    texture::ptr tex_roughness = app.producer.get_texture(_tex_roughness_);
    if (!tex_normal || !tex_roughness)
        return error::create_failed;

    buffer ubo_buffer;
    if (!ubo_buffer.create_mapped(app.device,
                                  nullptr, sizeof(g_ubo),
                                  VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT))
        return error::create_failed;

    buffer light_buffer;
    if (!light_buffer.create_mapped(app.device,
                                    g_lights.data(), sizeof(g_lights),
                                    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT))
        return error::create_failed;

    VkSamplerCreateInfo const sampler_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST
    };
    VkSampler sampler;
    if (!app.device->vkCreateSampler(&sampler_info, &sampler))
        return error::create_failed;

    // pipeline-specific resources
    // created in app.on_create, destroyed in app.on_destroy

    descriptor::pool descriptor_pool;

    render_pass::ptr gbuffer_renderpass = make_render_pass(app.device);
    descriptor::ptr gbuffer_set_layout = make_descriptor();
    VkDescriptorSet gbuffer_set = VK_NULL_HANDLE;
    pipeline_layout::ptr gbuffer_pipeline_layout = make_pipeline_layout();
    render_pipeline::ptr gbuffer_pipeline = make_render_pipeline(app.device);

    descriptor::ptr lighting_set_layout = make_descriptor();
    VkDescriptorSet lighting_set = VK_NULL_HANDLE;
    pipeline_layout::ptr lighting_pipeline_layout = make_pipeline_layout();
    render_pipeline::ptr lighting_pipeline = make_render_pipeline(app.device);

    app.on_create = [&]() {
        VkDescriptorPoolSizes const pool_sizes = {
            // one uniform buffer for each pass (G-Buffer + Lighting)
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1 * 2 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 }, // light buffer
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
              2 /* normal + roughness texture */ + g_attachments.size() },
        };
        constexpr ui32 max_sets = 2; // one for each pass
        if (!descriptor_pool.create(app.device, pool_sizes, max_sets))
            return false;

        // G-Buffer pass

        gbuffer_set_layout->add_binding(0,
                                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                        VK_SHADER_STAGE_VERTEX_BIT
                                            | VK_SHADER_STAGE_FRAGMENT_BIT);
        gbuffer_set_layout->add_binding(1,
                                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                        VK_SHADER_STAGE_FRAGMENT_BIT);
        gbuffer_set_layout->add_binding(2,
                                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                        VK_SHADER_STAGE_FRAGMENT_BIT);
        if (!gbuffer_set_layout->create(app.device))
            return false;

        gbuffer_set = gbuffer_set_layout->allocate(descriptor_pool.get());
        if (!gbuffer_set)
            return false;

        std::vector<VkWriteDescriptorSet> gbuffer_write_sets;
        for (descriptor::binding::ptr const& binding :
             gbuffer_set_layout->get_bindings()) {
            VkDescriptorSetLayoutBinding const& info = binding->get();

            gbuffer_write_sets.push_back({ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                           .dstSet = gbuffer_set,
                                           .dstBinding = info.binding,
                                           .descriptorCount = info.descriptorCount,
                                           .descriptorType = info.descriptorType });
        }

        gbuffer_write_sets[0].pBufferInfo = ubo_buffer.get_descriptor_info();
        gbuffer_write_sets[1].pImageInfo = tex_normal->get_descriptor_info();
        gbuffer_write_sets[2].pImageInfo = tex_roughness->get_descriptor_info();

        app.device->vkUpdateDescriptorSets(gbuffer_write_sets.size(),
                                           gbuffer_write_sets.data());

        gbuffer_pipeline_layout->add(gbuffer_set_layout);
        gbuffer_pipeline_layout->add_push_constant_range(
            { VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
              0, sizeof(glsl::PushConstantData) });

        if (!gbuffer_pipeline_layout->create(app.device))
            return false;

        VkPipelineColorBlendAttachmentState const gbuffer_blend_state = {
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT
                              | VK_COLOR_COMPONENT_G_BIT
                              | VK_COLOR_COMPONENT_B_BIT
                              | VK_COLOR_COMPONENT_A_BIT
        };

        if (!gbuffer_pipeline->add_shader(app.producer.get_shader(_gbuffer_vertex_),
                                          VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!gbuffer_pipeline->add_shader(app.producer.get_shader(_gbuffer_fragment_),
                                          VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        for (auto i = 0u; i < g_attachments.size() - 1; ++i) {
            gbuffer_pipeline->add_color_blend_attachment(gbuffer_blend_state);
        }

        gbuffer_pipeline->set_depth_test_and_write(true, true);
        gbuffer_pipeline->set_depth_compare_op(VK_COMPARE_OP_LESS);
        gbuffer_pipeline->set_rasterization_cull_mode(VK_CULL_MODE_NONE);

        gbuffer_pipeline->set_vertex_input_binding({ 0, sizeof(vertex),
                                                     VK_VERTEX_INPUT_RATE_VERTEX });

        gbuffer_pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(vertex, position)) },
            { 1, 0, VK_FORMAT_R32G32_SFLOAT, to_ui32(offsetof(vertex, uv)) },
            { 2, 0, VK_FORMAT_R32G32B32_SFLOAT, to_ui32(offsetof(vertex, normal)) },
        });

        gbuffer_pipeline->set_layout(gbuffer_pipeline_layout);
        gbuffer_pipeline->set_auto_size(true);

        gbuffer_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            scoped_label label(cmd_buf, "gbuffer");

            gbuffer_pipeline_layout->bind(cmd_buf, gbuffer_set);
            object->bind(cmd_buf);

            for (auto i = 0u; i < object_instances.size(); ++i) {
                glsl::PushConstantData const pc = {
                    .model = object_instances[i],
                    .color = v3(1.0f),
                    .metallic = float(i % 2),
                    .enableNormalMapping = 1 - (i % 2)
                };
                app.device->call().vkCmdPushConstants(
                    cmd_buf,
                    gbuffer_pipeline_layout->get(),
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0, sizeof(pc), &pc);

                object->draw(cmd_buf);
            }
        };

        gbuffer_renderpass = create_gbuffer_renderpass(app, g_attachments);
        gbuffer_renderpass->add_front(gbuffer_pipeline);

        // Lighting pass

        for (auto i = 0u; i < g_attachments.size(); ++i) {
            lighting_set_layout->add_binding(i,
                                             VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        lighting_set_layout->add_binding(g_attachments.size() + 0,
                                         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                         VK_SHADER_STAGE_FRAGMENT_BIT);
        lighting_set_layout->add_binding(g_attachments.size() + 1,
                                         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                         VK_SHADER_STAGE_FRAGMENT_BIT);
        if (!lighting_set_layout->create(app.device))
            return false;

        lighting_set = lighting_set_layout->allocate(descriptor_pool.get());
        if (!lighting_set)
            return false;

        lighting_pipeline_layout->add(lighting_set_layout);
        if (!lighting_pipeline_layout->create(app.device))
            return false;

        VkPipelineColorBlendAttachmentState const lighting_blend_state = {
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT
                | VK_COLOR_COMPONENT_G_BIT
                | VK_COLOR_COMPONENT_B_BIT
                | VK_COLOR_COMPONENT_A_BIT
        };

        if (!lighting_pipeline->add_shader(app.producer.get_shader(_lighting_vertex_),
                                           VK_SHADER_STAGE_VERTEX_BIT))
            return false;
        if (!lighting_pipeline->add_shader(app.producer.get_shader(_lighting_fragment_),
                                           VK_SHADER_STAGE_FRAGMENT_BIT))
            return false;

        lighting_pipeline->add_color_blend_attachment(lighting_blend_state);
        lighting_pipeline->set_rasterization_cull_mode(VK_CULL_MODE_NONE);
        lighting_pipeline->set_layout(lighting_pipeline_layout);
        lighting_pipeline->set_auto_size(true);

        lighting_pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
            scoped_label label(cmd_buf, "lighting");

            // run a fullscreen pass to calculate lighting,
            // the shader loops over all lights
            // - this is NOT very performant, but simplifies the demo
            // - in a proper deferred renderer you most likely want to:
            //     - render light geometries (e.g. spheres)
            //       while depth testing against the G-Buffer depth
            //     - use some kind of spatial acceleration structure for lights

            lighting_pipeline_layout->bind(cmd_buf, lighting_set);
            app.device->call().vkCmdDraw(cmd_buf, 3, 1, 0, 0);
        };

        // use lava's default backbuffer renderpass
        render_pass::ptr lighting_renderpass = app.shading.get_pass();
        lighting_renderpass->add_front(lighting_pipeline);

        // the resize callback creates the G-Buffer images and renderpass,
        // call it once manually
        if (!resize_callback.on_created({}, { { 0, 0 }, app.target->get_size() }))
            return false;

        // renderpasses have been created at this point, actually create the pipelines
        if (!gbuffer_pipeline->create(gbuffer_renderpass->get()))
            return false;

        if (!lighting_pipeline->create(lighting_renderpass->get()))
            return false;

        return true;
    };

    app.on_process = [&](VkCommandBuffer cmd_buf, index_t frame) {
        scoped_label label(cmd_buf, "on_process");

        // start custom renderpass,
        // run on_process() for each pipeline added to the renderpass
        gbuffer_renderpass->process(cmd_buf, 0);
    };

    app.on_update = [&](delta dt) {
        float const seconds = to_sec(app.run_time.current);

        constexpr float distance = 1.25f;
        float const left = -distance * (object_instances.size() - 1) * 0.5f;

        for (auto i = 0u; i < object_instances.size(); ++i) {
            float const x = left + distance * i;

            v3 axis = v3(0.f);
            axis[i % 3] = 1.f;

            mat4 model = mat4(1.0f);
            model = glm::translate(model, { x, 0.f, 0.f });
            model = glm::rotate(model,
                                glm::radians(std::fmod(seconds * 45.f, 360.f)),
                                axis);
            model = glm::scale(model, { 0.5f, 0.5f, 0.5f });
            object_instances[i] = model;
        }

        return run_continue;
    };

    // handle backbuffer resize

    resize_callback.on_created = [&](VkAttachmentsRef, rect area) {
        // update uniform buffer
        g_ubo.camPos = { 0.f, 0.f, -1.25f };
        g_ubo.lightCount = g_lights.size();
        g_ubo.view = glm::lookAtLH(g_ubo.camPos, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
        g_ubo.projection = perspective_matrix(area.get_size(), 90.f, 3.f);
        g_ubo.invProjection = glm::inverse(g_ubo.projection);
        g_ubo.resolution = area.get_size();
        *(decltype(g_ubo)*) ubo_buffer.get_mapped_data() = g_ubo;

        // (re-)create G-Buffer attachments and collect views for framebuffer creation
        VkImageViews views;
        for (gbuffer_attachment& att : g_attachments) {
            if (!att.image_handle->create(app.device, area.get_size()))
                return false;

            views.push_back(att.image_handle->get_view());
        }

        // update lighting descriptor set with new G-Buffer image handles
        std::vector<VkWriteDescriptorSet> lighting_write_sets;
        for (descriptor::binding::ptr const& binding :
             lighting_set_layout->get_bindings()) {
            VkDescriptorSetLayoutBinding const& info = binding->get();

            lighting_write_sets.push_back(
                { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                  .dstSet = lighting_set,
                  .dstBinding = info.binding,
                  .descriptorCount = info.descriptorCount,
                  .descriptorType = info.descriptorType });
        }

        std::array<VkDescriptorImageInfo, g_attachments.size()> lighting_images;
        for (auto i = 0u; i < g_attachments.size(); ++i) {
            lighting_images[i] = {
                .sampler = sampler,
                .imageView = g_attachments[i].image_handle->get_view(),
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            };

            lighting_write_sets[i].pImageInfo = &lighting_images[i];
        }

        lighting_write_sets[g_attachments.size() + 0].pBufferInfo =
            ubo_buffer.get_descriptor_info();

        lighting_write_sets[g_attachments.size() + 1].pBufferInfo =
            light_buffer.get_descriptor_info();

        app.device->vkUpdateDescriptorSets(lighting_write_sets.size(),
                                           lighting_write_sets.data());

        // create framebuffer (and renderpass if necessary)
        if (gbuffer_renderpass->get() == VK_NULL_HANDLE)
            return gbuffer_renderpass->create({ views }, area);
        else
            return gbuffer_renderpass->get_target_callback().on_created({ views }, area);
    };

    resize_callback.on_destroyed = [&]() {
        app.device->wait_for_idle();

        // destroy framebuffer
        gbuffer_renderpass->get_target_callback().on_destroyed();

        // destroy G-Buffer attachments
        for (gbuffer_attachment& att : g_attachments) {
            att.image_handle->destroy();
        }
    };

    app.imgui.on_draw = [&]() {
        ImGui::SetNextWindowPos({ 30, 30 }, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({ 210, 110 }, ImGuiCond_FirstUseEver);

        ImGui::Begin(app.get_name());

        app.draw_about(draw_no_separator);

        ImGui::End();
    };

    app.on_destroy = [&]() {
        app.target->remove_callback(&resize_callback);
        resize_callback.on_destroyed();

        lighting_pipeline->destroy();
        lighting_pipeline_layout->destroy();
        lighting_set_layout->destroy();

        gbuffer_pipeline->destroy();
        gbuffer_pipeline_layout->destroy();
        gbuffer_set_layout->destroy();
        gbuffer_renderpass->destroy();

        descriptor_pool.destroy();
    };

    app.add_run_end([&]() {
        app.device->vkDestroySampler(sampler);
        sampler = VK_NULL_HANDLE;

        light_buffer.destroy();
        ubo_buffer.destroy();
    });

    return app.run();
}

//-----------------------------------------------------------------------------
bool gbuffer_attachment::create(app const& app, ui32 index) {
    usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

    VkFormat_optional format = get_supported_format(
        app.device->get_vk_physical_device(),
        requested_formats, usage);

    if (!format.has_value())
        return false;

    image_handle = make_image(*format);
    image_handle->set_usage(usage);

    renderpass_attachment = make_attachment(*format);
    renderpass_attachment->set_op(VK_ATTACHMENT_LOAD_OP_CLEAR,
                                  VK_ATTACHMENT_STORE_OP_STORE);
    renderpass_attachment->set_stencil_op(VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                          VK_ATTACHMENT_STORE_OP_DONT_CARE);
    renderpass_attachment->set_layouts(VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    subpass_reference.attachment = index;
    subpass_reference.layout = (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                                   ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                                   : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    return true;
}

//-----------------------------------------------------------------------------
render_pass::ptr create_gbuffer_renderpass(app const& app,
                                           attachment_array& attachments) {
    VkClearValues clear_values(attachments.size(), { .color = { 0.f, 0.f, 0.f, 1.f } });
    clear_values[gbuffer_attachment::depth] = { .depthStencil = { 1.f, 0 } };

    render_pass::ptr pass = make_render_pass(app.device);
    pass->set_clear_values(clear_values);

    VkAttachmentReferences color_attachments;
    for (auto i = 0u; i < gbuffer_attachment::count; ++i) {
        if (!attachments[i].create(app, i))
            return nullptr;

        pass->add(attachments[i].renderpass_attachment);

        if (i != gbuffer_attachment::depth)
            color_attachments.push_back(attachments[i].subpass_reference);
    }

    subpass::ptr sub = make_subpass();
    sub->set_color_attachments(color_attachments);
    sub->set_depth_stencil_attachment(attachments[gbuffer_attachment::depth].subpass_reference);
    pass->add(sub);

    subpass_dependency::ptr dependency = make_subpass_dependency(VK_SUBPASS_EXTERNAL,
                                                                 0);
    // wait for previous fragment shader to finish reading before clearing attachments
    dependency->set_stage_mask(
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
    // we need a memory barrier because this isn't a standard write-after-read hazard
    // subpass deps have an implicit attachment layout transition,
    // so the dst access mask must be correct
    dependency->set_access_mask(0,
                                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
                                    | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                                    | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
                                    | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
    pass->add(dependency);

    dependency = make_subpass_dependency(pass->get_subpass_count() - 1,
                                         VK_SUBPASS_EXTERNAL);
    // don't run any fragment shader (sample attachments)
    // before we're done writing to attachments
    dependency->set_stage_mask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                   | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
    // make attachment writes visible to subsequent reads
    dependency->set_access_mask(VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                                    | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                VK_ACCESS_SHADER_READ_BIT);
    pass->add(dependency);

    return pass;
}
