/**
 * @file         liblava/frame/frame.cpp
 * @brief        Framework
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/frame/frame.hpp"
#include "liblava/base/memory.hpp"
#include "liblava/core/misc.hpp"
#include "liblava/core/time.hpp"
#include "liblava/util/log.hpp"

#if _WIN32 && LAVA_DEBUG
    #include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace lava {

//-----------------------------------------------------------------------------
ms now() {
    return to_ms(glfwGetTime());
}

//-----------------------------------------------------------------------------
frame::frame(argh::parser cmd_line) {
    env.cmd_line = cmd_line;
    setup();
}

//-----------------------------------------------------------------------------
frame::frame(frame_env env)
: env(env) {
    setup();
}

//-----------------------------------------------------------------------------
frame::~frame() {
    teardown();
}

//-----------------------------------------------------------------------------
void frame_env::set_default() {
#if LAVA_DEBUG_CONFIG
    log.debug = true;
    debug.validation = true;
    debug.utils = true;
#endif
}

/**
 * @brief Handle environment
 * @param env    Frame environment
 */
void handle_env(frame_env& env) {
    auto cmd_line = env.cmd_line;

    if (cmd_line[{ "-d", "--debug" }])
        env.debug.validation = true;

    if (cmd_line[{ "-r", "--renderdoc" }])
        env.debug.render_doc = true;

    if (cmd_line[{ "-u", "--utils" }])
        env.debug.utils = true;

    if (auto log_level = -1; cmd_line({ "-l", "--log" }) >> log_level) {
        env.log.level = log_level;

        if (log_level == SPDLOG_LEVEL_TRACE)
            env.debug.verbose = true;
    }

    global_logger::singleton().set(setup_log(env.log));

    if (int_version{} != env.info.app_version) {
        log()->info(">>> {} / {} - {} / {} - {} {}", version_string(),
                    int_version_string(),
                    env.info.app_name,
                    to_string(env.info.app_version),
                    _build_date, _build_time);
    } else {
        log()->info(">>> {} / {} - {} - {} {}", version_string(),
                    int_version_string(),
                    env.info.app_name,
                    _build_date, _build_time);
    }

    log_command_line(cmd_line);

    if (env.log.level >= 0)
        log()->info("log level: {}", spdlog::level::to_string_view(
                                         (spdlog::level::level_enum) env.log.level));
}

//-----------------------------------------------------------------------------
bool frame::setup() {
    if (initialized)
        return false;

#if _WIN32 && LAVA_DEBUG
    AllocConsole();
#endif

    handle_env(env);

    glfwSetErrorCallback([](i32 error, name description) {
        log()->error("glfw: {} - {}", error, description);
    });

    log()->info("glfw: {}", glfwGetVersionString());

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

    if (failed(volkInitialize())) {
        log()->error("init volk");
        return false;
    }

    log()->info("vulkan: {}", to_string(get_instance_version()));

    auto glfw_extensions_count = 0u;
    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
    for (auto i = 0u; i < glfw_extensions_count; ++i)
        env.param.extensions.push_back(glfw_extensions[i]);

#ifdef __APPLE__
    env.param.extensions.push_back("VK_KHR_portability_enumeration");
    env.param.extensions.push_back("VK_KHR_get_physical_device_properties2");
#endif

    if (!instance::singleton().create(env.param, env.debug, env.info)) {
        log()->error("create instance");
        return false;
    }

    initialized = true;

    log()->info("---");

    return true;
}

//-----------------------------------------------------------------------------
void frame::teardown() {
    if (!initialized)
        return;

    platform.clear();

    instance::singleton().destroy();

    glfwTerminate();

    log()->info("<<<");
    log()->flush();

    global_logger::singleton().reset();
    teardown_log(env.log);

    initialized = false;
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

    platform.wait_idle();

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
        for (auto& func : run_once_list) {
            if (!func())
                return false;
        }

        run_once_list.clear();
    }

    for (auto& [id, func] : run_map) {
        if (!func(id))
            return false;
    }

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
    auto id = ids::instance().next();
    run_map.emplace(id, func);
    return id;
}

//-----------------------------------------------------------------------------
id frame::add_run_end(run_end_func_ref func) {
    auto id = ids::instance().next();
    run_end_map.emplace(id, func);
    return id;
}

//-----------------------------------------------------------------------------
bool frame::remove(id::ref func_id) {
    if (contains(run_remove_list, func_id))
        return false;

    run_remove_list.push_back(func_id);
    return true;
}

//-----------------------------------------------------------------------------
void frame::trigger_run_remove() {
    for (auto& func_id : run_remove_list) {
        if (run_map.count(func_id))
            run_map.erase(func_id);
        else if (run_end_map.count(func_id))
            run_end_map.erase(func_id);
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
