// file      : liblava/frame/frame.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/instance.hpp>
#include <liblava/base/device.hpp>

#include <argh.h>

namespace lava {

struct frame_config {

    explicit frame_config() = default;
    explicit frame_config(name app, argh::parser) 
                         : app(app), cmd_line(cmd_line) {}

    argh::parser cmd_line;

    name org = _liblava_;
    name app = _lava_;
    name ext = _zip_;

    log_config log;
    instance::debug_config debug;

    bool data_folder = false;
};

enum error {

    not_ready       = -1,
    create_failed   = -2,
    aborted         = -3,
};

milliseconds now();

struct frame : no_copy_no_move, interface {

    using ptr = std::shared_ptr<frame>;

    explicit frame(argh::parser cmd_line);
    explicit frame(frame_config config);
    ~frame() override;

    bool ready() const;

    bool run();

    bool shut_down();

    using run_func = std::function<bool()>;
    using run_func_ref = run_func const&;

    id add_run(run_func_ref func);
   
    using run_end_func = std::function<void()>;
    using run_end_func_ref = run_end_func const&;

    id add_run_end(run_end_func_ref func);

    bool remove(id::ref id);

    milliseconds get_running_time() const { return now() - start_time; }

    argh::parser const& get_cmd_line() const { return config.cmd_line; }
    frame_config get_config() const { return config; }
    name get_name() const { return config.app; }

    bool waiting_for_events() const { return wait_for_events; }
    void set_wait_for_events(bool value = true) { wait_for_events = value; }

    device_ptr create_device() {

        auto device = manager.create();
        if (!device)
            return nullptr;

        return device.get();
    }

    device_manager manager;

private:
    bool setup(frame_config config);
    void teardown();

    bool run_step();

    void trigger_run_end();

    frame_config config;

    bool running = false;
    bool wait_for_events = false;
    milliseconds start_time;

    using run_func_map = std::map<id, run_func>;
    run_func_map run_map;

    using run_end_func_map = std::map<id, run_end_func>;
    run_end_func_map run_end_map;
};

void handle_events(bool wait = false);

void handle_events(milliseconds timeout);
void handle_events(seconds timeout);

void post_empty_event();

} // lava
