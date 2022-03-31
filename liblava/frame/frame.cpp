/**
 * @file         liblava/frame/frame.cpp
 * @brief        Framework
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/base/memory.hpp>
#include <liblava/frame/frame.hpp>

#if _WIN32 && LIBLAVA_DEBUG
    #include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace lava {

//-----------------------------------------------------------------------------
ms now() {
    return to_ms(glfwGetTime());
}

/// Frame initialized state
static bool frame_initialized = false;

//-----------------------------------------------------------------------------
frame::frame(argh::parser cmd_line) {
    frame_config c;
    c.cmd_line = cmd_line;

    setup(c);
}

//-----------------------------------------------------------------------------
frame::frame(frame_config c) {
    setup(c);
}

//-----------------------------------------------------------------------------
frame::~frame() {
    teardown();
}

//-----------------------------------------------------------------------------
bool frame::ready() const {
    return frame_initialized;
}

/**
 * @brief Handle config
 *
 * @param config    Frame config
 */
void handle_config(frame_config& config) {
#if LIBLAVA_DEBUG
    config.log.debug = true;
    config.debug.validation = true;
    config.debug.utils = true;
#endif

    auto cmd_line = config.cmd_line;

    if (cmd_line[{ "-d", "--debug" }])
        config.debug.validation = true;

    if (cmd_line[{ "-r", "--renderdoc" }])
        config.debug.render_doc = true;

    if (cmd_line[{ "-v", "--verbose" }])
        config.debug.verbose = true;

    if (cmd_line[{ "-u", "--utils" }])
        config.debug.utils = true;

    if (auto log_level = -1; cmd_line({ "-l", "--log" }) >> log_level)
        config.log.level = log_level;

    set_log(setup_log(config.log));

    if (internal_version{} != config.info.app_version) {
        log()->info(">>> {} / {} - {} / {} - {} {}", str(version_string()),
                    str(internal_version_string()),
                    config.info.app_name,
                    str(to_string(config.info.app_version)),
                    _build_date, _build_time);
    } else {
        log()->info(">>> {} / {} - {} - {} {}", str(version_string()),
                    str(internal_version_string()),
                    config.info.app_name,
                    _build_date, _build_time);
    }

    log_command_line(cmd_line);

    if (config.log.level >= 0)
        log()->info("log {}", spdlog::level::to_string_view((spdlog::level::level_enum) config.log.level));
}

//-----------------------------------------------------------------------------
bool frame::setup(frame_config c) {
    if (frame_initialized)
        return false;

#if _WIN32 && LIBLAVA_DEBUG
    AllocConsole();
#endif

    config = c;
    handle_config(config);

    glfwSetErrorCallback([](i32 error, name description) {
        log()->error("glfw {} - {}", error, description);
    });

    log()->debug("glfw {}", glfwGetVersionString());

    if (glfwInit() != GLFW_TRUE) {
        log()->error("init glfw");
        return false;
    }

    if (glfwVulkanSupported() != GLFW_TRUE) {
        log()->error("vulkan not supported");
        return false;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto result = volkInitialize();
    if (failed(result)) {
        log()->error("init volk");
        return false;
    }

    log()->info("vulkan {}", str(to_string(instance::get_version())));

    // check profile
    if (!config.profile.empty()) {
        auto profile_properties = config.profile.get();

        if (!profile_supported(profile_properties)) {
            log()->error("profile support at instance level: {} - version: {}", profile_properties.profileName, profile_properties.specVersion);
            return false;
        }

        log()->info("profile: {} - version: {}", profile_properties.profileName, profile_properties.specVersion);
    }

    auto glfw_extensions_count = 0u;
    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
    for (auto i = 0u; i < glfw_extensions_count; ++i)
        config.param.extensions.push_back(glfw_extensions[i]);

    if (!instance::singleton().create(config.param, config.debug, config.info, config.profile)) {
        log()->error("create instance");
        return false;
    }

    frame_initialized = true;

    log()->info("---");

    return true;
}

//-----------------------------------------------------------------------------
void frame::teardown() {
    if (!frame_initialized)
        return;

    manager.clear();

    instance::singleton().destroy();

    glfwTerminate();

    log()->info("<<<");
    log()->flush();

    frame_initialized = false;
}

//-----------------------------------------------------------------------------
frame::result frame::run() {
    if (running)
        return error::still_running;

    running = true;
    start_time = now();

    while (running) {
        if (!run_step())
            break;
    }

    manager.wait_idle();

    trigger_run_end();

    auto result = 0;
    if (running) {
        result = error::run_aborted;
        running = false;
    }

    start_time = {};

    return result;
}

//-----------------------------------------------------------------------------
bool frame::run_step() {
    handle_events(wait_for_events);

    if (!run_once_list.empty()) {
        for (auto& func : run_once_list)
            if (!func())
                return false;

        run_once_list.clear();
    }

    for (auto& [id, func] : run_map)
        if (!func(id))
            return false;

    if (!run_remove_list.empty())
        trigger_run_remove();

    return true;
}

//-----------------------------------------------------------------------------
bool frame::shut_down() {
    if (!running)
        return false;

    running = false;

    return true;
}

//-----------------------------------------------------------------------------
id frame::add_run(run_func_ref func) {
    auto id = ids::next();
    run_map.emplace(id, func);

    return id;
}

//-----------------------------------------------------------------------------
id frame::add_run_end(run_end_func_ref func) {
    auto id = ids::next();
    run_end_map.emplace(id, func);

    return id;
}

//-----------------------------------------------------------------------------
bool frame::remove(id::ref id) {
    if (contains(run_remove_list, id))
        return false;

    run_remove_list.push_back(id);
    return true;
}

//-----------------------------------------------------------------------------
void frame::trigger_run_remove() {
    for (auto& id : run_remove_list) {
        auto result = false;

        if (run_map.count(id)) {
            run_map.erase(id);
            result = true;
        } else if (run_end_map.count(id)) {
            run_end_map.erase(id);
            result = true;
        }

        if (result)
            ids::free(id);
    }

    run_remove_list.clear();
}

//-----------------------------------------------------------------------------
void frame::trigger_run_end() {
    for (auto& [id, func] : reverse(run_end_map))
        func();
}

//-----------------------------------------------------------------------------
void handle_events(bool wait) {
    if (wait)
        glfwWaitEvents();
    else
        glfwPollEvents();
}

//-----------------------------------------------------------------------------
void handle_events(ms timeout) {
    glfwWaitEventsTimeout(to_sec(timeout));
}

//-----------------------------------------------------------------------------
void handle_events(seconds timeout) {
    glfwWaitEventsTimeout(to_r64(timeout.count()));
}

//-----------------------------------------------------------------------------
void post_empty_event() {
    glfwPostEmptyEvent();
}

} // namespace lava
