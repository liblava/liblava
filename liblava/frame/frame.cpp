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
bool frame::ready() const {
    return frame_initialized;
}

//-----------------------------------------------------------------------------
void frame_env::set_default() {
#if LIBLAVA_DEBUG_CONFIG
    log.debug = true;
    debug.validation = true;
    debug.utils = true;
#endif
}

/**
 * @brief Handle environment
 *
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

    set_log(setup_log(env.log));

    if (internal_version{} != env.info.app_version) {
        log()->info(">>> {} / {} - {} / {} - {} {}", version_string(),
                    internal_version_string(),
                    env.info.app_name,
                    to_string(env.info.app_version),
                    _build_date, _build_time);
    } else {
        log()->info(">>> {} / {} - {} - {} {}", version_string(),
                    internal_version_string(),
                    env.info.app_name,
                    _build_date, _build_time);
    }

    log_command_line(cmd_line);

    if (env.log.level >= 0)
        log()->info("log {}", spdlog::level::to_string_view(
                                  (spdlog::level::level_enum) env.log.level));
}

//-----------------------------------------------------------------------------
bool frame::setup() {
    if (frame_initialized)
        return false;

#if _WIN32 && LIBLAVA_DEBUG
    AllocConsole();
#endif

    handle_env(env);

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

    if (failed(volkInitialize())) {
        log()->error("init volk");
        return false;
    }

    log()->info("vulkan {}", to_string(instance::get_version()));

    if (!check_profile())
        return false;

    auto glfw_extensions_count = 0u;
    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
    for (auto i = 0u; i < glfw_extensions_count; ++i)
        env.param.extensions.push_back(glfw_extensions[i]);

#ifdef __APPLE__
    env.param.extensions.push_back("VK_KHR_portability_enumeration");
#endif

    if (!instance::singleton().create(env.param, env.debug, env.info, env.profile)) {
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

    platform.clear();

    instance::singleton().destroy();

    glfwTerminate();

    log()->info("<<<");
    log()->flush();

    reset_log();
    teardown_log(env.log);

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
bool frame::check_profile() const {
    if (env.profile.empty())
        return true;

    auto profile_properties = env.profile.get();

    if (!profile_supported(profile_properties)) {
        log()->error("profile support at instance level: {} - version: {}",
                     profile_properties.profileName, profile_properties.specVersion);
        return false;
    }

    log()->info("profile: {} - version: {}",
                profile_properties.profileName, profile_properties.specVersion);
    return true;
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
