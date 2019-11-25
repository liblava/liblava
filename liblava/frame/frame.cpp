// file      : liblava/frame/frame.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#include <liblava/frame/frame.hpp>
#include <liblava/base/memory.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if !LIBLAVA_DEBUG && _WIN32
#include <windows.h>
#include <iostream>
#endif

void hide_console(lava::name program) {

#if !LIBLAVA_DEBUG && _WIN32

    auto version_str = fmt::format("{} {}", lava::_liblava_, lava::str(lava::to_string(lava::_version)));
    std::cout << version_str.c_str() << std::endl;
    std::cout << program << std::endl;

    const auto dot_count = 3;
    const auto sleep_ms = lava::to_i32(1.0 / dot_count * 1000.f);

    for (auto i = 0u; i < dot_count; ++i) {

        lava::sleep(lava::milliseconds(sleep_ms));
        std::cout << ".";
    }

    FreeConsole();

#endif
}

void log_command_line(argh::parser& cmd_line) {

    if (!cmd_line.pos_args().empty()) {

        for (auto& pos_arg : cmd_line.pos_args())
            lava::log()->info("cmd line (pos) {}", lava::str(pos_arg));
    }

    if (!cmd_line.flags().empty()) {

        for (auto& flag : cmd_line.flags())
            lava::log()->info("cmd line (flag) {}", lava::str(flag));
    }

    if (!cmd_line.params().empty()) {

        for (auto& param : cmd_line.params())
            lava::log()->info("cmd line (para) {} = {}", lava::str(param.first), lava::str(param.second));
    }
}

lava::milliseconds lava::now() { return to_ms(glfwGetTime()); }

namespace lava {

static bool _initialized = false;

frame::frame(argh::parser cmd_line) { 

    frame_config config_;
    config_.cmd_line = cmd_line;

    setup(config_);
}

frame::frame(frame_config config_) {

    setup(config_);
}

frame::~frame() { teardown(); }

bool frame::ready() const { return _initialized; }

bool frame::setup(frame_config config_) {

    if (_initialized)
        return false;

    config = config_;

#if LIBLAVA_DEBUG
    config.log.debug = true;
    config.debug.validation = true;
    config.debug.utils = true;
#endif

    hide_console(config.app);

    auto cmd_line = config.cmd_line;

    if (cmd_line[{ "-d", "--debug" }])
        config.debug.validation = true;

    if (cmd_line[{ "-a", "--assist" }])
        config.debug.assistent = true;

    if (cmd_line[{ "-r", "--renderdoc" }])
        config.debug.render_doc = true;

    if (cmd_line[{ "-v", "--verbose" }])
        config.debug.verbose = true;

    if (cmd_line[{ "-u", "--utils" }])
        config.debug.utils = true;

    if (cmd_line[{ "-i", "--info" }])
        config.debug.info = true;

    if (auto log_level = -1; cmd_line({ "-l", "--log" }) >> log_level)
        config.log.level = log_level;

    setup_log(config.log);

    log()->info(">>> {} / {} - {} {}", str(to_string(_version)), str(to_string(_internal_version)), _build_date, _build_time);

    log_command_line(cmd_line);

    if (config.log.level >= 0)
        log()->info("log {}", spdlog::level::to_str((spdlog::level::level_enum)config.log.level));

    glfwSetErrorCallback([](i32 error, name description) {

        log()->error("glfw {} - {}", error, description);
    });

    log()->debug("glfw {}", glfwGetVersionString());

    if (glfwInit() != GLFW_TRUE) {

        log()->error("init glfw failed");
        return false;
    }

    if (glfwVulkanSupported() != GLFW_TRUE) {

        log()->error("glfw vulkan not supported");
        return false;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto result = volkInitialize();
    if (failed(result)) {

        log()->error("init volk failed");
        return false;
    }

    log()->info("vulkan {}", str(to_string(instance::get_version())));

    instance::create_param param;

    auto glfw_extensions_count = 0u;
    auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
    for (auto i = 0u; i < glfw_extensions_count; ++i)
        param.extension_to_enable.push_back(glfw_extensions[i]);

    if (!instance::singleton().create(param, config.debug, config.app)) {

        log()->error("create instance failed");
        return false;
    }

    log()->debug("physfs {}", str(to_string(file_system::get_version())));

    if (!file_system::get().initialize(str(cmd_line[0]), config.org, config.app, config.ext)) {

        log()->error("init file system failed");
        return false;
    }

    file_system::get().mount_res();

    if (config.data_folder)
        file_system::get().create_data_folder();

    _initialized = true;

    log()->info("---");

    return true;
}

void frame::teardown() {

    if (!_initialized)
        return;

    manager.clear();

    instance::singleton().destroy();

    glfwTerminate();

    log()->info("<<<");

    log()->flush();
    spdlog::drop_all();

    file_system::get().terminate();

    _initialized = false;
}

frame::result frame::run() {

    if (running)
        return error::running;

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

        result = error::aborted;
        running = false;
    }

    start_time = {};

    return result;
}

bool frame::run_step() {

    handle_events(wait_for_events);

    if (!run_once_list.empty()) {

        for (auto& func : run_once_list)
            if (!func())
                return false;

        run_once_list.clear();
    }

    for (auto& func : run_map)
        if (!func.second())
            return false;

    return true;
}

bool frame::shut_down() {

    if (!running)
        return false;

    running = false;

    return true;
}

id frame::add_run(run_func_ref func) {

    auto id = ids::next();
    run_map.emplace(id, func);

    return id;
}

id frame::add_run_end(run_end_func_ref func) {

    auto id = ids::next();
    run_end_map.emplace(id, func);

    return id;
}

bool frame::remove(id::ref id) {

    auto result = false;

    if (run_map.count(id)) {

        run_map.erase(id);
        result = true;
    }
    else if (run_end_map.count(id)) {

        run_end_map.erase(id);
        result = true;
    }
    
    if (result)
        ids::free(id);

    return result;
}

void frame::trigger_run_end() {

    for (auto& func : run_end_map)
        func.second();
}

} // lava

void lava::handle_events(bool wait) {

    if (wait)
        glfwWaitEvents();
    else
        glfwPollEvents();
}

void lava::handle_events(milliseconds timeout) {

    glfwWaitEventsTimeout(to_sec(timeout));
}

void lava::handle_events(seconds timeout) {

    glfwWaitEventsTimeout(to_r64(timeout.count()));
}

void lava::post_empty_event() {

    glfwPostEmptyEvent();
}
