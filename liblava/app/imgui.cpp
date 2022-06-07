/**
 * @file         liblava/app/imgui.cpp
 * @brief        ImGui integration
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/app/def.hpp>
#include <liblava/app/imgui.hpp>
#include <liblava/base/debug_utils.hpp>
#include <liblava/resource/format.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
    #undef APIENTRY
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h> // glfwGetWin32Window
#endif

#include <imgui.h>
#include <glm/gtc/color_space.hpp>

namespace lava {

/// ImGui vertex shader
ui32 imgui_vert_shader[] = {
#include "res/imgui/imgui.vert.u32"
};

/// ImGui fragment shader
ui32 imgui_frag_shader[] = {
#include "res/imgui/imgui.frag.u32"
};

/**
 * @brief Get the clipboard text
 *
 * @param user_data    Window handle
 *
 * @return name        Clipboard text
 */
name get_clipboard_text(void* user_data) {
    return glfwGetClipboardString(static_cast<GLFWwindow*>(user_data));
}

/**
 * @brief Set the clipboard text
 *
 * @param user_data    Window handle
 * @param text         Clipboard text
 */
void set_clipboard_text(void* user_data, name text) {
    glfwSetClipboardString(static_cast<GLFWwindow*>(user_data), text);
}

//-----------------------------------------------------------------------------
void imgui::handle_mouse_button_event(i32 button, i32 action, i32 mods) {
    if (action == GLFW_PRESS && button >= 0
        && button < IM_ARRAYSIZE(mouse_just_pressed))
        mouse_just_pressed[button] = true;
}

//-----------------------------------------------------------------------------
void imgui::handle_scroll_event(r64 x_offset, r64 y_offset) {
    auto& io = ImGui::GetIO();
    io.MouseWheelH += static_cast<float>(x_offset);
    io.MouseWheel += static_cast<float>(y_offset);
}

//-----------------------------------------------------------------------------
void imgui::handle_key_event(i32 key, i32 scancode, i32 action, i32 mods) {
    auto& io = ImGui::GetIO();

    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;

    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL]
                 || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT]
                  || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT]
                || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER]
                  || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

//-----------------------------------------------------------------------------
void imgui::update_mouse_pos_and_buttons() {
    auto& io = ImGui::GetIO();

    for (auto i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i) {
        io.MouseDown[i] = mouse_just_pressed[i]
                          || glfwGetMouseButton(window, i) != 0;
        mouse_just_pressed[i] = false;
    }

    auto const mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        if (io.WantSetMousePos) {
            glfwSetCursorPos(window,
                             (double) mouse_pos_backup.x, (double) mouse_pos_backup.y);
        } else {
            double mouse_x, mouse_y;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float) mouse_x, (float) mouse_y);
        }
    }
}

//-----------------------------------------------------------------------------
void imgui::update_mouse_cursor() {
    auto& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    auto const imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
        glfwSetCursor(window, mouse_cursors[imgui_cursor]
                                  ? mouse_cursors[imgui_cursor]
                                  : mouse_cursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

//-----------------------------------------------------------------------------
void imgui::setup(GLFWwindow* w, config config) {
    window = w;
    current_time = 0.0;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    auto& style = ImGui::GetStyle();
    if (config.style) {
        style = *config.style;
    } else {
        ImGui::StyleColorsDark();
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.8f, 0.f, 0.f, 0.4f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.8f, 0.f, 0.0f, 1.f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.f, 0.f, 0.f, 0.1f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 0.f, 0.f, 0.4f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.f, 0.f, 0.4f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.f, 0.f, 0.f, 0.4f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.f, 0.f, 0.f, 0.5f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(1.f, 0.f, 0.f, 0.8f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.059f, 0.059f, 0.059f, 0.863f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.f, 0.f, 0.f, 0.0f);
    }

    if (config.font_data.ptr) {
        ImFontConfig font_config;
        font_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF(config.font_data.ptr,
                                       to_i32(config.font_data.size),
                                       config.font_size,
                                       &font_config);
    } else {
        io.Fonts->AddFontDefault();
    }

    if (config.icon.font_data.ptr) {
        static const ImWchar icons_ranges[] = { config.icon.range_begin,
                                                config.icon.range_end, 0 };

        ImFontConfig icon_config;
        icon_config.MergeMode = true;
        icon_config.GlyphMinAdvanceX = config.icon.size;
        icon_config.PixelSnapH = true;
        icon_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF(config.icon.font_data.ptr,
                                       to_i32(config.icon.font_data.size),
                                       config.icon.size,
                                       &icon_config,
                                       icons_ranges);
    }

    io.SetClipboardTextFn = set_clipboard_text;
    io.GetClipboardTextFn = get_clipboard_text;
    io.ClipboardUserData = window;
#if defined(_WIN32)
    ImGui::GetMainViewport()->PlatformHandleRaw = (void*) glfwGetWin32Window(window);
#endif // _WIN32

    mouse_cursors.resize(ImGuiMouseCursor_COUNT);
    mouse_cursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    glfwSetCharCallback(window, [](GLFWwindow*, ui32 c) {
        if (c > 0 && c < 0x10000)
            ImGui::GetIO().AddInputCharacter(static_cast<unsigned short>(c));
    });

    set_ini_file(config.ini_file_dir);

    callback.on_key_event = [&](key_event const& event) {
        if (activated())
            handle_key_event(to_i32(event.key),
                             event.scancode,
                             to_i32(event.action),
                             to_i32(event.mod));

        return capture_keyboard();
    };

    callback.on_scroll_event = [&](scroll_event const& event) {
        if (activated())
            handle_scroll_event(event.offset.x, event.offset.y);

        return capture_mouse();
    };

    callback.on_mouse_button_event = [&](mouse_button_event const& event) {
        if (activated())
            handle_mouse_button_event(to_i32(event.button),
                                      to_i32(event.action),
                                      to_i32(event.mod));

        return capture_mouse();
    };
}

#define MAP_BUTTON(NAV_NO, BUTTON_NO) \
    { \
        if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) \
            io.NavInputs[NAV_NO] = 1.0f; \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) \
    { \
        float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; \
        v = (v - V0) / (V1 - V0); \
        if (v > 1.f) \
            v = 1.f; \
        if (io.NavInputs[NAV_NO] < v) \
            io.NavInputs[NAV_NO] = v; \
    }

//-----------------------------------------------------------------------------
void imgui::new_frame() {
    auto& io = ImGui::GetIO();
#if LIBLAVA_DEBUG_ASSERT
    IM_ASSERT(io.Fonts->IsBuilt());
#endif

    i32 w, h = 0;
    i32 display_w, display_h = 0;

    glfwGetWindowSize(window, &w, &h);
    glfwGetFramebufferSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((r32) w, (r32) h);
    io.DisplayFramebufferScale = ImVec2(w > 0
                                            ? ((r32) display_w / w)
                                            : 0,
                                        h > 0
                                            ? ((r32) display_h / h)
                                            : 0);

    auto now = glfwGetTime();
    io.DeltaTime = current_time > 0.0
                       ? (r32) (now - current_time)
                       : (1.f / 60.f);
    current_time = now;

    update_mouse_pos_and_buttons();
    update_mouse_cursor();

    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) {
        i32 axes_count = 0, buttons_count = 0;
        auto const* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
        auto const* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);

        MAP_BUTTON(ImGuiNavInput_Activate, 0); // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel, 1); // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu, 2); // Square / X
        MAP_BUTTON(ImGuiNavInput_Input, 3); // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft, 13); // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight, 11); // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp, 10); // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown, 12); // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev, 4); // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext, 5); // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow, 4); // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast, 5); // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
        MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);

        if (axes_count > 0 && buttons_count > 0)
            io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
        else
            io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
    }

    ImGui::NewFrame();
}

#undef MAP_BUTTON
#undef MAP_ANALOG

//-----------------------------------------------------------------------------
bool imgui::create(graphics_pipeline::ptr p, index mf) {
    pipeline = std::move(p);

    device = pipeline->get_device();
    max_frames = mf;

    for (auto i = 0u; i < max_frames; ++i) {
        vertex_buffers.push_back(make_buffer());
        index_buffers.push_back(make_buffer());
    }

    pipeline->set_vertex_input_binding({ 0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX });
    pipeline->set_vertex_input_attributes({
        { 0, 0, VK_FORMAT_R32G32_SFLOAT, to_ui32(offsetof(ImDrawVert, pos)) },
        { 1, 0, VK_FORMAT_R32G32_SFLOAT, to_ui32(offsetof(ImDrawVert, uv)) },
        { 2, 0, VK_FORMAT_R8G8B8A8_UNORM, to_ui32(offsetof(ImDrawVert, col)) },
    });

    if (!pipeline->add_shader({ imgui_vert_shader, sizeof(imgui_vert_shader) },
                              VK_SHADER_STAGE_VERTEX_BIT))
        return false;

    if (!pipeline->add_shader({ imgui_frag_shader, sizeof(imgui_frag_shader) },
                              VK_SHADER_STAGE_FRAGMENT_BIT))
        return false;

    pipeline->add_color_blend_attachment();

    descriptor = make_descriptor();
    descriptor->add_binding(0,
                            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                            VK_SHADER_STAGE_FRAGMENT_BIT);
    if (!descriptor->create(device))
        return false;

    descriptor_pool = make_descriptor_pool();
    if (!descriptor_pool->create(device,
                                 { { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 } }))
        return false;

    layout = make_pipeline_layout();
    layout->add(descriptor);
    layout->add({ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(r32) * 4 });

    if (!layout->create(device))
        return false;

    pipeline->set_layout(layout);
    pipeline->set_auto_size(false);

    descriptor_set = descriptor->allocate(descriptor_pool->get());

    pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
        if (!activated() || !on_draw)
            return;

        new_frame();

        if (on_draw)
            on_draw();

        scoped_label label(cmd_buf,
                           _lava_gui_,
                           { 0.9f, 0.75f, 0.f, 1.f });

        render(cmd_buf);
    };

    initialized = true;

    return true;
}

//-----------------------------------------------------------------------------
void imgui::destroy() {
    if (!initialized)
        return;

    for (auto& mouse_cursor : mouse_cursors) {
        glfwDestroyCursor(mouse_cursor);
        mouse_cursor = nullptr;
    }

    invalidate_device_objects();
    ImGui::DestroyContext();

    initialized = false;
}

//-----------------------------------------------------------------------------
bool imgui::capture_mouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

//-----------------------------------------------------------------------------
bool imgui::capture_keyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}

//-----------------------------------------------------------------------------
void imgui::set_ini_file(std::filesystem::path dir) {
    dir.append(_imgui_file_);

    ini_file = dir.string();

    ImGui::GetIO().IniFilename = str(ini_file);
}

//-----------------------------------------------------------------------------
void imgui::convert_style_to_srgb() {
    ImGuiStyle& style = ImGui::GetStyle();
    for (auto i = 0u; i < ImGuiCol_COUNT; ++i) {
        glm::vec3 srgb = glm::make_vec3(&style.Colors[i].x);
        glm::vec3 linear = glm::convertSRGBToLinear(srgb);
        style.Colors[i] = ImVec4(linear.x, linear.y, linear.z, style.Colors[i].w);
    }
}

//-----------------------------------------------------------------------------
void imgui::invalidate_device_objects() {
    vertex_buffers.clear();
    index_buffers.clear();

    descriptor->free(descriptor_set, descriptor_pool->get());

    descriptor_pool->destroy();
    descriptor->destroy();
    descriptor = nullptr;

    pipeline = nullptr;

    layout->destroy();
    layout = nullptr;
}

//-----------------------------------------------------------------------------
void imgui::render(VkCommandBuffer cmd_buf) {
    ImGui::Render();

    render_draw_lists(cmd_buf);

    frame = (frame + 1) % max_frames;
}

//-----------------------------------------------------------------------------
void imgui::prepare_draw_lists(ImDrawData* draw_data) {
    auto vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
    if (!vertex_buffers[frame]->valid()
        || vertex_buffers[frame]->get_size() < vertex_size) {
        if (vertex_buffers[frame]->valid())
            vertex_buffers[frame]->destroy();

        if (!vertex_buffers[frame]->create(device,
                                           nullptr,
                                           ((vertex_size - 1) / buffer_memory_alignment + 1)
                                               * buffer_memory_alignment,
                                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           true,
                                           VMA_MEMORY_USAGE_CPU_TO_GPU))
            return;
    }

    auto index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    if (!index_buffers[frame]->valid()
        || index_buffers[frame]->get_size() < index_size) {
        if (index_buffers[frame]->valid())
            index_buffers[frame]->destroy();

        if (!index_buffers[frame]->create(device,
                                          nullptr,
                                          ((index_size - 1) / buffer_memory_alignment + 1)
                                              * buffer_memory_alignment,
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                          true,
                                          VMA_MEMORY_USAGE_CPU_TO_GPU))
            return;
    }

    auto vtx_dst = (ImDrawVert*) vertex_buffers[frame]->get_mapped_data();
    auto idx_dst = (ImDrawIdx*) index_buffers[frame]->get_mapped_data();

    for (auto i = 0; i < draw_data->CmdListsCount; ++i) {
        auto const* cmd_list = draw_data->CmdLists[i];

        memcpy(vtx_dst, cmd_list->VtxBuffer.Data,
               cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data,
               cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }

    VkMappedMemoryRange const vertex_range{
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = vertex_buffers[frame]->get_device_memory(),
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    VkMappedMemoryRange const index_range{
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = index_buffers[frame]->get_device_memory(),
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    std::array<VkMappedMemoryRange, 2> const ranges = { vertex_range, index_range };
    check(device->call().vkFlushMappedMemoryRanges(device->get(),
                                                   to_ui32(ranges.size()),
                                                   ranges.data()));
}

//-----------------------------------------------------------------------------
void imgui::render_draw_lists(VkCommandBuffer cmd_buf) {
    auto draw_data = ImGui::GetDrawData();
    if (draw_data->TotalVtxCount == 0)
        return;

    prepare_draw_lists(draw_data);

    layout->bind(cmd_buf, descriptor_set);

    std::array<VkDeviceSize, 1> const vertex_offset = { 0 };
    std::array<VkBuffer, 1> const buffers = { vertex_buffers[frame]->get() };
    device->call().vkCmdBindVertexBuffers(cmd_buf,
                                          0,
                                          to_ui32(buffers.size()),
                                          buffers.data(),
                                          vertex_offset.data());

    device->call().vkCmdBindIndexBuffer(cmd_buf,
                                        index_buffers[frame]->get(),
                                        0,
                                        VK_INDEX_TYPE_UINT16);

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = ImGui::GetIO().DisplaySize.x;
    viewport.height = ImGui::GetIO().DisplaySize.y;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    std::array<VkViewport, 1> const viewports = { viewport };
    device->call().vkCmdSetViewport(cmd_buf,
                                    0,
                                    to_ui32(viewports.size()),
                                    viewports.data());

    auto& io = ImGui::GetIO();

    float scale[2];
    scale[0] = 2.f / io.DisplaySize.x;
    scale[1] = 2.f / io.DisplaySize.y;
    device->call().vkCmdPushConstants(cmd_buf,
                                      layout->get(),
                                      VK_SHADER_STAGE_VERTEX_BIT,
                                      sizeof(float) * 0,
                                      sizeof(float) * 2,
                                      scale);

    float translate[2];
    translate[0] = -1.f;
    translate[1] = -1.f;
    device->call().vkCmdPushConstants(cmd_buf,
                                      layout->get(),
                                      VK_SHADER_STAGE_VERTEX_BIT,
                                      sizeof(float) * 2,
                                      sizeof(float) * 2,
                                      translate);

    auto vtx_offset = 0u;
    auto idx_offset = 0u;
    for (auto i = 0; i < draw_data->CmdListsCount; ++i) {
        auto const* cmd_list = draw_data->CmdLists[i];
        for (auto c = 0; c < cmd_list->CmdBuffer.Size; ++c) {
            auto const* cmd = &cmd_list->CmdBuffer[c];
            if (cmd->UserCallback) {
                cmd->UserCallback(cmd_list, cmd);
            } else {
                VkRect2D scissor;
                scissor.offset = {
                    (i32) (cmd->ClipRect.x) > 0
                        ? (i32) (cmd->ClipRect.x)
                        : 0,
                    (i32) (cmd->ClipRect.y) > 0
                        ? (i32) (cmd->ClipRect.y)
                        : 0
                };
                scissor.extent = { (ui32) (cmd->ClipRect.z - cmd->ClipRect.x),
                                   (ui32) (cmd->ClipRect.w - cmd->ClipRect.y + 1) };

                std::array<VkRect2D, 1> const scissors = { scissor };
                device->call().vkCmdSetScissor(cmd_buf,
                                               0,
                                               to_ui32(scissors.size()),
                                               scissors.data());

                device->call().vkCmdDrawIndexed(cmd_buf,
                                                cmd->ElemCount,
                                                1,
                                                idx_offset,
                                                vtx_offset,
                                                0);
            }

            idx_offset += cmd->ElemCount;
        }

        vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

//-----------------------------------------------------------------------------
bool imgui::upload_fonts(texture::ptr texture) {
    uchar* pixels = nullptr;

    auto width = 0;
    auto height = 0;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    auto const font_format = VK_FORMAT_R8G8B8A8_UNORM;
    if (!texture->create(device, { width, height }, font_format))
        return false;

    auto const upload_size = width * height * format_block_size(font_format);
    if (!texture->upload(pixels, upload_size))
        return false;

    VkWriteDescriptorSet const write_desc{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = texture->get_descriptor_info(),
    };

    device->vkUpdateDescriptorSets({ write_desc });

    return true;
}

//-----------------------------------------------------------------------------
void setup_imgui_font(imgui::config& config, imgui::font::ref font) {
    if (!font.file.empty()) {
        if (load_file_data(str(font.file), config.font_data)) {
            config.font_size = font.size;

            log()->debug("load {}", str(font.file));
        } else {
            log()->error("setup_imgui_font - cannot load font file {}",
                         str(font.file));
        }
    }

    if (!font.icon_file.empty()) {
        if (load_file_data(str(font.icon_file), config.icon.font_data)) {
            config.icon.size = font.icon_size;

            config.icon.font_data = config.icon.font_data;
            config.icon.range_begin = font.icon_range_begin;
            config.icon.range_end = font.icon_range_end;

            log()->debug("load {}", str(font.icon_file));
        } else {
            log()->error("setup_imgui_font - cannot load font icon file {}",
                         str(font.icon_file));
        }
    }
}

//-----------------------------------------------------------------------------
void setup_imgui_font_icons(imgui::font& font, string filename, ui16 min, ui16 max) {
    font.icon_file = fmt::format("{}{}", _font_icon_path_, str(filename));

    font.icon_range_begin = min;
    font.icon_range_end = max;
}

//-----------------------------------------------------------------------------
void imgui_left_spacing(ui32 top) {
    for (auto i = 0u; i < top; ++i)
        ImGui::Dummy({ 0.f, 2.f });
    ImGui::SameLine(0.f, 5.f);
}

} // namespace lava
