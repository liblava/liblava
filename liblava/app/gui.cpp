// file      : liblava/app/gui.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/app/gui.hpp>
#include <liblava/app/def.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h> // glfwGetWin32Window
#endif

#include <imgui.h>

namespace lava {

name get_clipboard_text(void* user_data) {

    return glfwGetClipboardString(static_cast<GLFWwindow*>(user_data));
}

void set_clipboard_text(void* user_data, name text) {

    glfwSetClipboardString(static_cast<GLFWwindow*>(user_data), text);
}

void gui::handle_mouse_button_event(i32 button, i32 action, i32 mods) {

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(mouse_just_pressed))
        mouse_just_pressed[button] = true;
}

void gui::handle_scroll_event(r64 x_offset, r64 y_offset) {

    auto& io = ImGui::GetIO();
    io.MouseWheelH += static_cast<float>(x_offset);
    io.MouseWheel += static_cast<float>(y_offset);
}

void gui::handle_key_event(i32 key, i32 scancode, i32 action, i32 mods) {

    auto& io = ImGui::GetIO();

    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;

    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void gui::update_mouse_pos_and_buttons() {

    auto& io = ImGui::GetIO();

    for (i32 i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {

        io.MouseDown[i] = mouse_just_pressed[i] || glfwGetMouseButton(window, i) != 0;
        mouse_just_pressed[i] = false;
    }

    ImVec2 const mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

    if (glfwGetWindowAttrib(window, GLFW_FOCUSED)) {

        if (io.WantSetMousePos) {

            glfwSetCursorPos(window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
        }
        else {

            double mouse_x, mouse_y;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
        }
    }
}

void gui::update_mouse_cursor() {

    auto& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else {

        glfwSetCursor(window, mouse_cursors[imgui_cursor] ? mouse_cursors[imgui_cursor] : mouse_cursors[ImGuiMouseCursor_Arrow]);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void gui::setup(GLFWwindow* window_, config config) {

    window = window_;
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

    if (config.font_data.ptr) {

        ImFontConfig font_config;
        font_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF(config.font_data.ptr, to_i32(config.font_data.size), config.font_size, &font_config);
    }
    else {

        io.Fonts->AddFontDefault();
    }

    if (config.icon.font_data.ptr) {

        static const ImWchar icons_ranges[] = { config.icon.range_begin, config.icon.range_end, 0 };

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = false;

        io.Fonts->AddFontFromMemoryTTF(config.icon.font_data.ptr, to_i32(config.icon.font_data.size), 
                                       config.icon.size, &icons_config, icons_ranges);
    }

    io.RenderDrawListsFn = nullptr;
    io.SetClipboardTextFn = set_clipboard_text;
    io.GetClipboardTextFn = get_clipboard_text;
    io.ClipboardUserData = window;
#if defined(_WIN32)
    io.ImeWindowHandle = (void*)glfwGetWin32Window(window);
#endif // _WIN32

    mouse_cursors.resize(ImGuiMouseCursor_Count_);
    mouse_cursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    mouse_cursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    glfwSetCharCallback(window, [](GLFWwindow*, unsigned int c) {

        if (c > 0 && c < 0x10000)
            ImGui::GetIO().AddInputCharacter(static_cast<unsigned short>(c));
    });

    set_ini_file(config.ini_file_dir);

    on_key_event = [&](key_event const& event) {

        if (is_active())
            handle_key_event(to_i32(event.key), event.scancode, to_i32(event.action), to_i32(event.mod));

        return want_capture_mouse();
    };

    on_scroll_event = [&](scroll_event const& event) {

        if (is_active())
            handle_scroll_event(event.offset.x, event.offset.y);

        return want_capture_mouse();
    };

    on_mouse_button_event = [&](mouse_button_event const& event) {

        if (is_active())
            handle_mouse_button_event(to_i32(event.button), to_i32(event.action), to_i32(event.mod));

        return want_capture_mouse();
    };
}

#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                      \
    {                                                                      \
        if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) \
            io.NavInputs[NAV_NO] = 1.0f;                                   \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                    \
    {                                                          \
        float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; \
        v = (v - V0) / (V1 - V0);                              \
        if (v > 1.f)                                           \
            v = 1.f;                                           \
        if (io.NavInputs[NAV_NO] < v)                          \
            io.NavInputs[NAV_NO] = v;                          \
    }

void gui::new_frame() {

    auto& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt());

    i32 w, h = 0;
    i32 display_w, display_h = 0;

    glfwGetWindowSize(window, &w, &h);
    glfwGetFramebufferSize(window, &display_w, &display_h);
    io.DisplaySize = ImVec2((r32)w, (r32)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((r32)display_w / w) : 0, h > 0 ? ((r32)display_h / h) : 0);

    auto now = glfwGetTime();
    io.DeltaTime = current_time > 0.0 ? (r32)(now - current_time) : (1.f / 60.f);
    current_time = now;

    update_mouse_pos_and_buttons();
    update_mouse_cursor();

    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) {

        int axes_count = 0, buttons_count = 0;
        float const* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
        unsigned char const* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);

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

static ui32 imgui_vert_shader[] = {

    0x07230203,0x00010000,0x00080007,0x0000002e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x0000000b,0x0000000f,0x00000015,
    0x0000001b,0x0000001c,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00030005,0x00000009,0x00000000,0x00050006,0x00000009,0x00000000,0x6f6c6f43,
    0x00000072,0x00040006,0x00000009,0x00000001,0x00005655,0x00030005,0x0000000b,0x0074754f,
    0x00040005,0x0000000f,0x6c6f4361,0x0000726f,0x00030005,0x00000015,0x00565561,0x00060005,
    0x00000019,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x00000019,0x00000000,
    0x505f6c67,0x7469736f,0x006e6f69,0x00030005,0x0000001b,0x00000000,0x00040005,0x0000001c,
    0x736f5061,0x00000000,0x00060005,0x0000001e,0x73755075,0x6e6f4368,0x6e617473,0x00000074,
    0x00050006,0x0000001e,0x00000000,0x61635375,0x0000656c,0x00060006,0x0000001e,0x00000001,
    0x61725475,0x616c736e,0x00006574,0x00030005,0x00000020,0x00006370,0x00040047,0x0000000b,
    0x0000001e,0x00000000,0x00040047,0x0000000f,0x0000001e,0x00000002,0x00040047,0x00000015,
    0x0000001e,0x00000001,0x00050048,0x00000019,0x00000000,0x0000000b,0x00000000,0x00030047,
    0x00000019,0x00000002,0x00040047,0x0000001c,0x0000001e,0x00000000,0x00050048,0x0000001e,
    0x00000000,0x00000023,0x00000000,0x00050048,0x0000001e,0x00000001,0x00000023,0x00000008,
    0x00030047,0x0000001e,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040017,
    0x00000008,0x00000006,0x00000002,0x0004001e,0x00000009,0x00000007,0x00000008,0x00040020,
    0x0000000a,0x00000003,0x00000009,0x0004003b,0x0000000a,0x0000000b,0x00000003,0x00040015,
    0x0000000c,0x00000020,0x00000001,0x0004002b,0x0000000c,0x0000000d,0x00000000,0x00040020,
    0x0000000e,0x00000001,0x00000007,0x0004003b,0x0000000e,0x0000000f,0x00000001,0x00040020,
    0x00000011,0x00000003,0x00000007,0x0004002b,0x0000000c,0x00000013,0x00000001,0x00040020,
    0x00000014,0x00000001,0x00000008,0x0004003b,0x00000014,0x00000015,0x00000001,0x00040020,
    0x00000017,0x00000003,0x00000008,0x0003001e,0x00000019,0x00000007,0x00040020,0x0000001a,
    0x00000003,0x00000019,0x0004003b,0x0000001a,0x0000001b,0x00000003,0x0004003b,0x00000014,
    0x0000001c,0x00000001,0x0004001e,0x0000001e,0x00000008,0x00000008,0x00040020,0x0000001f,
    0x00000009,0x0000001e,0x0004003b,0x0000001f,0x00000020,0x00000009,0x00040020,0x00000021,
    0x00000009,0x00000008,0x0004002b,0x00000006,0x00000028,0x00000000,0x0004002b,0x00000006,
    0x00000029,0x3f800000,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x00000007,0x00000010,0x0000000f,0x00050041,0x00000011,0x00000012,
    0x0000000b,0x0000000d,0x0003003e,0x00000012,0x00000010,0x0004003d,0x00000008,0x00000016,
    0x00000015,0x00050041,0x00000017,0x00000018,0x0000000b,0x00000013,0x0003003e,0x00000018,
    0x00000016,0x0004003d,0x00000008,0x0000001d,0x0000001c,0x00050041,0x00000021,0x00000022,
    0x00000020,0x0000000d,0x0004003d,0x00000008,0x00000023,0x00000022,0x00050085,0x00000008,
    0x00000024,0x0000001d,0x00000023,0x00050041,0x00000021,0x00000025,0x00000020,0x00000013,
    0x0004003d,0x00000008,0x00000026,0x00000025,0x00050081,0x00000008,0x00000027,0x00000024,
    0x00000026,0x00050051,0x00000006,0x0000002a,0x00000027,0x00000000,0x00050051,0x00000006,
    0x0000002b,0x00000027,0x00000001,0x00070050,0x00000007,0x0000002c,0x0000002a,0x0000002b,
    0x00000028,0x00000029,0x00050041,0x00000011,0x0000002d,0x0000001b,0x0000000d,0x0003003e,
    0x0000002d,0x0000002c,0x000100fd,0x00010038
};

static ui32 imgui_frag_shader[] = {

    0x07230203,0x00010000,0x00080007,0x0000001e,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x0000000d,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,0x000001c2,0x00040005,0x00000004,0x6e69616d,
    0x00000000,0x00040005,0x00000009,0x6c6f4366,0x0000726f,0x00030005,0x0000000b,0x00000000,
    0x00050006,0x0000000b,0x00000000,0x6f6c6f43,0x00000072,0x00040006,0x0000000b,0x00000001,
    0x00005655,0x00030005,0x0000000d,0x00006e49,0x00050005,0x00000016,0x78655473,0x65727574,
    0x00000000,0x00040047,0x00000009,0x0000001e,0x00000000,0x00040047,0x0000000d,0x0000001e,
    0x00000000,0x00040047,0x00000016,0x00000022,0x00000000,0x00040047,0x00000016,0x00000021,
    0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,0x00000006,
    0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,0x00000003,
    0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x00040017,0x0000000a,0x00000006,
    0x00000002,0x0004001e,0x0000000b,0x00000007,0x0000000a,0x00040020,0x0000000c,0x00000001,
    0x0000000b,0x0004003b,0x0000000c,0x0000000d,0x00000001,0x00040015,0x0000000e,0x00000020,
    0x00000001,0x0004002b,0x0000000e,0x0000000f,0x00000000,0x00040020,0x00000010,0x00000001,
    0x00000007,0x00090019,0x00000013,0x00000006,0x00000001,0x00000000,0x00000000,0x00000000,
    0x00000001,0x00000000,0x0003001b,0x00000014,0x00000013,0x00040020,0x00000015,0x00000000,
    0x00000014,0x0004003b,0x00000015,0x00000016,0x00000000,0x0004002b,0x0000000e,0x00000018,
    0x00000001,0x00040020,0x00000019,0x00000001,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x00050041,0x00000010,0x00000011,0x0000000d,
    0x0000000f,0x0004003d,0x00000007,0x00000012,0x00000011,0x0004003d,0x00000014,0x00000017,
    0x00000016,0x00050041,0x00000019,0x0000001a,0x0000000d,0x00000018,0x0004003d,0x0000000a,
    0x0000001b,0x0000001a,0x00050057,0x00000007,0x0000001c,0x00000017,0x0000001b,0x00050085,
    0x00000007,0x0000001d,0x00000012,0x0000001c,0x0003003e,0x00000009,0x0000001d,0x000100fd,
    0x00010038
};

bool gui::create(graphics_pipeline::ptr pipeline_, index max_frames_) {

    pipeline = std::move(pipeline_);

    device = pipeline->get_device();
    max_frames = max_frames_;

    for (auto i = 0u; i < max_frames; ++i) {

        vertex_buffers.push_back(make_buffer());
        index_buffers.push_back(make_buffer());
    }

    pipeline->set_vertex_input_binding({ 0, sizeof(ImDrawVert), VK_VERTEX_INPUT_RATE_VERTEX });
    pipeline->set_vertex_input_attributes({
            { 0, 0, VK_FORMAT_R32G32_SFLOAT,  to_ui32(offsetof(ImDrawVert, pos)) },
            { 1, 0, VK_FORMAT_R32G32_SFLOAT,  to_ui32(offsetof(ImDrawVert, uv)) },
            { 2, 0, VK_FORMAT_R8G8B8A8_UNORM, to_ui32(offsetof(ImDrawVert, col)) },
    });

    if (!pipeline->add_shader({ imgui_vert_shader, sizeof(imgui_vert_shader) }, VK_SHADER_STAGE_VERTEX_BIT))
        return false;

    if (!pipeline->add_shader({ imgui_frag_shader, sizeof(imgui_frag_shader) }, VK_SHADER_STAGE_FRAGMENT_BIT))
        return false;

    pipeline->add_color_blend_attachment();

    descriptor = make_descriptor();
    descriptor->add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
    if (!descriptor->create(device))
        return false;

    layout = make_pipeline_layout();
    layout->add(descriptor);
    layout->add({ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(r32) * 4 });

    if (!layout->create(device))
        return false;

    pipeline->set_layout(layout);
    pipeline->set_auto_size(false);

    descriptor_set = descriptor->allocate();

    pipeline->on_process = [&](VkCommandBuffer cmd_buf) {
        
        if (!is_active() || !on_draw)
            return;

        new_frame();

        if (on_draw)
            on_draw();

        render(cmd_buf);
    };

    initialized = true;

    return true;
}

void gui::destroy() {

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

bool gui::want_capture_mouse() const {

    return ImGui::GetIO().WantCaptureMouse;
}

void gui::set_ini_file(fs::path dir) {
    
    dir.append(_gui_file_);

    ini_file = dir.string();

    ImGui::GetIO().IniFilename = str(ini_file);
}

void gui::invalidate_device_objects() {

    vertex_buffers.clear();
    index_buffers.clear();

    descriptor->free(descriptor_set);
    descriptor->destroy();
    descriptor = nullptr;

    pipeline = nullptr;

    layout->destroy();
    layout = nullptr;
}

void gui::render(VkCommandBuffer cmd_buf) {

    ImGui::Render();

    render_draw_lists(cmd_buf);

    frame = (frame + 1) % max_frames;
}

void gui::render_draw_lists(VkCommandBuffer cmd_buf) {

    auto draw_data = ImGui::GetDrawData();
    if (draw_data->TotalVtxCount == 0)
        return;

    auto& io = ImGui::GetIO();

    auto vertex_size = draw_data->TotalVtxCount * sizeof(ImDrawVert);
    if (!vertex_buffers[frame]->is_valid() || vertex_buffers[frame]->get_size() < vertex_size) {

        if (vertex_buffers[frame]->is_valid())
            vertex_buffers[frame]->destroy();

        if (!vertex_buffers[frame]->create(device, nullptr, ((vertex_size - 1) / buffer_memory_alignment + 1) * buffer_memory_alignment,
                                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, true, VMA_MEMORY_USAGE_CPU_TO_GPU))
            return;
    }

    auto index_size = draw_data->TotalIdxCount * sizeof(ImDrawIdx);
    if (!index_buffers[frame]->is_valid() || index_buffers[frame]->get_size() < index_size) {

        if (index_buffers[frame]->is_valid())
            index_buffers[frame]->destroy();

        if (!index_buffers[frame]->create(device, nullptr, ((index_size - 1) / buffer_memory_alignment + 1) * buffer_memory_alignment,
                                            VK_BUFFER_USAGE_INDEX_BUFFER_BIT, true, VMA_MEMORY_USAGE_CPU_TO_GPU))
            return;
    }

    auto vtx_dst = (ImDrawVert*)vertex_buffers[frame]->get_mapped_data();
    auto idx_dst = (ImDrawIdx*)index_buffers[frame]->get_mapped_data();

    for (auto i = 0; i < draw_data->CmdListsCount; ++i) {

        auto const* cmd_list = draw_data->CmdLists[i];

        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

        vtx_dst += cmd_list->VtxBuffer.Size;
        idx_dst += cmd_list->IdxBuffer.Size;
    }

    VkMappedMemoryRange const vertex_range
    {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = vertex_buffers[frame]->get_device_memory(),
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    VkMappedMemoryRange const index_range
    {
        .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        .memory = index_buffers[frame]->get_device_memory(),
        .offset = 0,
        .size = VK_WHOLE_SIZE,
    };

    std::array<VkMappedMemoryRange, 2> const ranges = { vertex_range, index_range };
    check(device->call().vkFlushMappedMemoryRanges(device->get(), to_ui32(ranges.size()), ranges.data()));

    layout->bind(cmd_buf, descriptor_set);

    std::array<VkDeviceSize, 1> const vertex_offset = { 0 };
    std::array<VkBuffer, 1> const buffers = { vertex_buffers[frame]->get() };
    device->call().vkCmdBindVertexBuffers(cmd_buf, 0, to_ui32(buffers.size()), buffers.data(), vertex_offset.data());

    device->call().vkCmdBindIndexBuffer(cmd_buf, index_buffers[frame]->get(), 0, VK_INDEX_TYPE_UINT16);

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = ImGui::GetIO().DisplaySize.x;
    viewport.height = ImGui::GetIO().DisplaySize.y;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    std::array<VkViewport, 1> const viewports = { viewport };
    device->call().vkCmdSetViewport(cmd_buf, 0, to_ui32(viewports.size()), viewports.data());

    float scale[2];
    scale[0] = 2.f / io.DisplaySize.x;
    scale[1] = 2.f / io.DisplaySize.y;
    device->call().vkCmdPushConstants(cmd_buf, layout->get(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 0, sizeof(float) * 2, scale);
    
    float translate[2];
    translate[0] = -1.f;
    translate[1] = -1.f;
    device->call().vkCmdPushConstants(cmd_buf, layout->get(), VK_SHADER_STAGE_VERTEX_BIT, sizeof(float) * 2, sizeof(float) * 2, translate);

    auto vtx_offset = 0u;
    auto idx_offset = 0u;
    for (auto i = 0; i < draw_data->CmdListsCount; ++i) {

        auto const* cmd_list = draw_data->CmdLists[i];
        for (auto c = 0; c < cmd_list->CmdBuffer.Size; ++c) {

            auto const* cmd = &cmd_list->CmdBuffer[c];
            if (cmd->UserCallback) {

                cmd->UserCallback(cmd_list, cmd);
            }
            else {

                VkRect2D scissor;
                scissor.offset = {
                    (i32)(cmd->ClipRect.x) > 0 ? (i32)(cmd->ClipRect.x) : 0,
                    (i32)(cmd->ClipRect.y) > 0 ? (i32)(cmd->ClipRect.y) : 0
                };
                scissor.extent = { (ui32)(cmd->ClipRect.z - cmd->ClipRect.x),
                                   (ui32)(cmd->ClipRect.w - cmd->ClipRect.y + 1) };

                std::array<VkRect2D, 1> const scissors = { scissor };
                device->call().vkCmdSetScissor(cmd_buf, 0, to_ui32(scissors.size()), scissors.data());

                device->call().vkCmdDrawIndexed(cmd_buf, cmd->ElemCount, 1, idx_offset, vtx_offset, 0);
            }

            idx_offset += cmd->ElemCount;
        }

        vtx_offset += cmd_list->VtxBuffer.Size;
    }
}

bool gui::upload_fonts(texture::ptr texture) {

    uchar* pixels = nullptr;

    auto width = 0;
    auto height = 0;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    if (!texture->create(device, { width, height }, VK_FORMAT_R8G8B8A8_UNORM))
        return false;

    auto upload_size =  width * height * sizeof(char) * 4;
    if (!texture->upload(pixels, upload_size))
        return false;

    VkWriteDescriptorSet const write_desc
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptor_set,
        .dstBinding = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = texture->get_info(),
    };

    device->vkUpdateDescriptorSets({ write_desc });

    return true;
}

} // lava
