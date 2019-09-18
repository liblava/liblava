// file      : liblava/frame/frame.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/instance.hpp>
#include <liblava/base/device.hpp>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <argh.h>

namespace lava {

struct frame_config {

    argh::parser cmd_line;

    name org = _liblava_;
    name app = _lava_;
    name ext = _zip_;

    log_config log;
    instance::debug debug;

    bool data_folder = false;
};

enum error {

    not_ready       = -1,
    create_failed   = -2,
    aborted         = -3,
};

time now();

struct frame : no_copy_no_move {

    using ptr = std::shared_ptr<frame>;

    explicit frame(argh::parser cmd_line);
    explicit frame(frame_config config);
    ~frame();

    static ptr make(argh::parser cmd_line) { return std::make_shared<frame>(cmd_line); }
    static ptr make(frame_config config) { return std::make_shared<frame>(config); }

    bool ready() const;

    bool run();
    bool run_step();

    bool shut_down();

    device* create_device() {

        auto device = manager.create();
        if (!device)
            return nullptr;

        return device.get();
    }

    device_manager manager;

    using run_func = std::function<bool()>;
    using run_func_ref = run_func const&;

    id add_run(run_func_ref func);
    bool remove_run(id::ref id);

    time get_running_time() const {

        if (start_time == 0.0)
            return 0.0;

        return now() - start_time;
    }

    using run_end_func = std::function<void()>;
    using run_end_func_ref = run_end_func const&;

    id add_run_end(run_end_func_ref func);
    bool remove_run_end(id::ref id);

    void trigger_run_end();

    argh::parser& get_cmd_line() { return cmd_line; }

    bool waiting_for_events() const { return wait_for_events; }
    void set_wait_for_events(bool value = true) { wait_for_events = value; }

private:

    bool setup(frame_config config = {});
    void teardown();

    argh::parser cmd_line;

    bool running = false;
    bool wait_for_events = false;
    time start_time = 0.0;

    using run_func_map = std::map<id, run_func>;
    run_func_map run_map;

    using run_end_func_map = std::map<id, run_end_func>;
    run_end_func_map run_end_map;
};

VkSurfaceKHR create_surface(GLFWwindow* window);

void handle_events(bool wait_for_events = false);

} // lava
