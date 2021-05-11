// file      : liblava/frame/frame.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <argh.h>
#include <liblava/base/device.hpp>
#include <liblava/base/instance.hpp>

namespace lava {

    struct frame_config {
        using ref = frame_config const&;

        explicit frame_config() = default;
        explicit frame_config(name a, argh::parser cl)
        : app(a), cmd_line(cl) {}

        argh::parser cmd_line;

        name org = _liblava_;
        name app = _lava_;
        name ext = _zip_;

        log_config log;

        instance::app_info app_info;
        instance::create_param param;
        instance::debug_config debug;
    };

    enum error {
        not_ready = -1,
        create_failed = -2,
        init_failed = -3,
        load_failed = -4,
        run_aborted = -5,
        still_running = -6,
    };

    ms now();

    constexpr bool const run_abort = false;
    constexpr bool const run_continue = true;

    struct frame : interface, no_copy_no_move {
        using ptr = std::shared_ptr<frame>;

        explicit frame(argh::parser cmd_line);
        explicit frame(frame_config config);
        ~frame() override;

        bool ready() const;

        using result = i32; // error < 0
        result run();

        bool shut_down();

        using run_func = std::function<bool()>;
        using run_func_ref = run_func const&;

        id add_run(run_func_ref func);

        using run_end_func = std::function<void()>;
        using run_end_func_ref = run_end_func const&;

        id add_run_end(run_end_func_ref func);

        using run_once_func = std::function<bool()>;
        using run_once_func_ref = run_once_func const&;

        void add_run_once(run_once_func_ref func) {
            run_once_list.push_back(func);
        }

        bool remove(id::ref id);

        ms get_running_time() const {
            return now() - start_time;
        }

        r64 get_running_time_sec() const {
            return to_sec(get_running_time());
        }

        argh::parser const& get_cmd_line() const {
            return config.cmd_line;
        }
        frame_config::ref get_config() const {
            return config;
        }
        name get_name() const {
            return config.app;
        }

        bool waiting_for_events() const {
            return wait_for_events;
        }
        void set_wait_for_events(bool value = true) {
            wait_for_events = value;
        }

        device_ptr create_device(index physical_device = 0) {
            auto device = manager.create(physical_device);
            if (!device)
                return nullptr;

            auto ptr = device.get();
            return ptr;
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
        ms start_time;

        using run_func_map = std::map<id, run_func>;
        run_func_map run_map;

        using run_end_func_map = std::map<id, run_end_func>;
        run_end_func_map run_end_map;

        using run_once_func_list = std::vector<run_once_func>;
        run_once_func_list run_once_list;
    };

    void handle_events(bool wait = false);

    void handle_events(ms timeout);
    void handle_events(seconds timeout);

    void post_empty_event();

} // namespace lava
