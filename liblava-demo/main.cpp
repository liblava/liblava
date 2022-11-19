/**
 * @file         main.cpp
 * @brief        Demo collection
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <IconsFontAwesome6.h>
#include <imgui.h>
#include <liblava/lava.hpp>

using namespace lava;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    driver& driver = driver::instance();

    ui32 selected = driver.get_stages().size();

    driver.on_run = [&](argh::parser argh) {
        driver::result result;

        engine app("lava demo", argh);
        app.config.v_sync = true;

        setup_imgui_font_icons(app.config.imgui_font,
                               FONT_ICON_FILE_NAME_FAS,
                               ICON_MIN_FA, ICON_MAX_FA);
        app.config.imgui_font.size = 36.f;
        app.config.imgui_font.icon_size = 36.f;
        app.props.add(_font_icon_, app.config.imgui_font.icon_file);

        stage::map const& stages = driver.get_stages();
        for (auto& [id, stage] : stages)
            app.props.add(stage->name,
                          stage->name + "/" + _screenshot_ + ".png");

        app.props.add(_vertex_, "demo/demo.vert");
        app.props.add(_fragment_, "demo/demo.frag");

        if (!app.setup()) {
            result.driver = error::not_ready;
            return result;
        }

        r32 alpha = 0.f;
        app.on_update = [&](delta dt) {
            alpha = (alpha < 1.f) ? alpha + dt : 1.f;
            return run_continue;
        };

        auto next = [&]() {
            selected = (selected == 1) ? stages.size() : --selected;
            alpha = 0.f;
        };

        auto previous = [&]() {
            selected = (selected == stages.size()) ? 1 : ++selected;
            alpha = 0.f;
        };

        auto play = [&]() {
            result.selected = selected;
            app.shut_down();
        };

        render_pipeline::ptr pipeline;
        pipeline_layout::ptr layout;

        descriptor::ptr descriptor;
        descriptor::pool::ptr descriptor_pool;
        VkDescriptorSets descriptor_sets;

        app.on_create = [&]() {
            pipeline = render_pipeline::make(app.device, app.pipeline_cache);
            if (!pipeline->add_shader(app.producer.get_shader(_vertex_),
                                      VK_SHADER_STAGE_VERTEX_BIT))
                return false;

            if (!pipeline->add_shader(app.producer.get_shader(_fragment_),
                                      VK_SHADER_STAGE_FRAGMENT_BIT))
                return false;

            pipeline->add_color_blend_attachment();

            descriptor = descriptor::make();
            descriptor->add_binding(0,
                                    VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                    VK_SHADER_STAGE_FRAGMENT_BIT);

            if (!descriptor->create(app.device))
                return false;

            descriptor_pool = descriptor::pool::make();
            if (!descriptor_pool->create(app.device,
                                         {
                                             { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                               to_ui32(stages.size()) },
                                         },
                                         stages.size()))
                return false;

            layout = pipeline_layout::make();
            layout->add_push_constant_range({ VK_SHADER_STAGE_FRAGMENT_BIT,
                                              0, sizeof(r32) * 4 });

            layout->add(descriptor);

            if (!layout->create(app.device))
                return false;

            pipeline->set_layout(layout);

            for (auto& [id, stage] : stages) {
                VkDescriptorSet descriptor_set =
                    descriptor->allocate(descriptor_pool->get());

                VkWriteDescriptorSet const write_desc_sampler{
                    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                    .dstSet = descriptor_set,
                    .dstBinding = 0,
                    .descriptorCount = 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                    .pImageInfo = app.producer.get_texture(stage->name)
                                      ->get_descriptor_info(),
                };

                app.device->vkUpdateDescriptorSets({ write_desc_sampler });

                descriptor_sets.push_back(descriptor_set);
            }

            render_pass::ptr render_pass = app.shading.get_pass();

            if (!pipeline->create(render_pass->get()))
                return false;

            render_pass->add_front(pipeline);

            pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
                VkViewport viewport = pipeline->get_viewport();

                r32 pc_resolution[2];
                pc_resolution[0] = viewport.width - viewport.x;
                pc_resolution[1] = viewport.height - viewport.y;
                app.device->call().vkCmdPushConstants(cmd_buf,
                                                      layout->get(),
                                                      VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      sizeof(r32) * 0,
                                                      sizeof(r32) * 2,
                                                      pc_resolution);

                r32 pc_time = to_r32(to_sec(app.run_time.current));
                app.device->call().vkCmdPushConstants(cmd_buf,
                                                      layout->get(),
                                                      VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      sizeof(r32) * 2,
                                                      sizeof(r32),
                                                      &pc_time);

                app.device->call().vkCmdPushConstants(cmd_buf,
                                                      layout->get(),
                                                      VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      sizeof(r32) * 3,
                                                      sizeof(r32),
                                                      &alpha);

                layout->bind(cmd_buf, descriptor_sets.at(selected - 1));

                app.device->call().vkCmdDraw(cmd_buf, 3, 1, 0, 0);
            };

            return true;
        };

        app.on_destroy = [&]() {
            for (VkDescriptorSet& set : descriptor_sets)
                descriptor->free(set, descriptor_pool->get());

            descriptor_sets.clear();

            descriptor_pool->destroy();
            descriptor->destroy();

            pipeline->destroy();
            layout->destroy();
        };

        bool show_fps = false;

        app.imgui.on_draw = [&]() {
            ImGuiWindowFlags const win_flags = ImGuiWindowFlags_NoDecoration
                                               | ImGuiWindowFlags_AlwaysAutoResize
                                               | ImGuiWindowFlags_NoSavedSettings
                                               | ImGuiWindowFlags_NoFocusOnAppearing
                                               | ImGuiWindowFlags_NoNav
                                               | ImGuiWindowFlags_NoMove;

            float const pad = 50.0f;
            ImGuiViewport const* viewport = ImGui::GetMainViewport();
            ImVec2 win_pos(viewport->WorkPos.x + pad,
                           viewport->WorkPos.y + pad);
            ImGui::SetNextWindowPos(win_pos, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.35f);

            bool always_open = true;
            if (ImGui::Begin(_liblava_, &always_open, win_flags)) {
                app.draw_about(draw_no_separator, show_fps ? draw_fps : draw_no_fps,
                               draw_no_spacing);

                if (ImGui::Button(icon(ICON_FA_ARROW_LEFT)))
                    previous();

                ImGui::SameLine();

                if (ImGui::Button(icon(ICON_FA_ARROW_RIGHT)))
                    next();

                ImGui::SameLine(0.f, 15.f);

                if (ImGui::Button(str(stages.at(selected)->name)))
                    play();
            }

            ImGui::End();
        };

        app.add_tooltip("play demo", key::enter);
        app.add_tooltip("next demo", key::right);
        app.add_tooltip("previous demo", key::left);
        app.add_tooltip("fps counter", key::tab);
        app.add_tooltip("refresh", key::space);

        app.input.key.listeners.add([&](key_event::ref event) {
            if (event.pressed(key::enter))
                play();

            if (event.pressed(key::left))
                previous();

            if (event.pressed(key::right))
                next();

            if (event.pressed(key::space))
                alpha = 0.f;

            if (event.pressed(key::tab))
                show_fps = !show_fps;

            return input_done;
        });

        result.driver = app.run();
        return result;
    };

    return driver.run({ argc, argv });
}
