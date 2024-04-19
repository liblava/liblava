/**
 * @file         liblava/frame/frame.hpp
 * @brief        Framework
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/base/device.hpp"
#include "liblava/base/instance.hpp"
#include "liblava/base/platform.hpp"
#include "liblava/core/time.hpp"
#include "liblava/frame/argh.hpp"
#include "liblava/util/log.hpp"
#include "liblava/util/telegram.hpp"

namespace lava {

/**
 * @brief Framework environment
 */
struct frame_env {
    /// Reference to frame environment
    using ref = frame_env const&;

    /**
     * @brief Construct a new frame environment
     */
    explicit frame_env() {
        set_default();
    }

    /**
     * @brief Construct a new frame environment
     * @param app_name    Name of application
     * @param cmd_line    Command line arguments
     */
    explicit frame_env(name app_name,
                       argh::parser cmd_line)
    : cmd_line(cmd_line) {
        info.app_name = app_name;
        set_default();
    }

    /**
     * @brief Set default settings
     */
    void set_default();

    /// Command line arguments
    argh::parser cmd_line;

    /// Logging configuration
    log_config log;

    /// Instance information
    instance_info info;

    /// Instance create parameters
    instance::create_param param;

    /// Intance debug configuration
    instance::debug_config debug;

    /// Message dispatcher threads
    ui32 telegraph_thread_count = 4;
};

/**
 * @brief Error codes
 */
enum error {
    not_ready = -1,
    create_failed = -2,
    init_failed = -3,
    load_failed = -4,
    run_aborted = -5,
    still_running = -6,
    program_failed = -7,
};

/**
 * @brief Get the current time
 * @return ms    Current milliseconds
 */
ms now();

/// Run abort result
constexpr bool const run_abort = false;

/// Run continue result
constexpr bool const run_continue = true;

/**
 * @brief Framework
 */
struct frame : interface, no_copy_no_move {
    /// Shared pointer to framework
    using ptr = std::shared_ptr<frame>;

    /**
     * @brief Construct a new framework
     * @param cmd_line    Command line arguments
     */
    explicit frame(argh::parser cmd_line);

    /**
     * @brief Construct a new framework
     * @param env    Framework environment
     */
    explicit frame(frame_env env);

    /**
     * @brief Destroy the framework
     */
    ~frame() override;

    /**
     * @brief Check if framework is ready
     * @return Framework is ready or not
     */
    bool ready() const {
        return initialized;
    }

    /// Framework result
    using result = i32; // error < 0

    /**
     * @brief Run the framework
     * @return result    Run result
     */
    result run();

    /**
     * @brief Shut down the framework
     * @return Shut down was successful or failed
     */
    bool shut_down();

    /// Run function
    using run_func = std::function<bool(id::ref)>;

    /// Reference to run function
    using run_func_ref = run_func const&;

    /**
     * @brief Add run to framework
     * @param func    Run function
     * @return id     Id of function
     */
    id add_run(run_func_ref func);

    /// Run end function
    using run_end_func = std::function<void()>;

    /// Reference to run end function
    using run_end_func_ref = run_end_func const&;

    /**
     * @brief Add run end to framework
     * @param func    Run end function
     * @return id     Id of function
     */
    id add_run_end(run_end_func_ref func);

    /// Run once function
    using run_once_func = std::function<bool()>;

    /// Reference to run once function
    using run_once_func_ref = run_once_func const&;

    /**
     * @brief Add run once to framework
     * @param func    Run once function
     */
    void add_run_once(run_once_func_ref func) {
        run_once_list.push_back(func);
    }

    /**
     * @brief Remove a function from framework
     * @param func_id    Id of function
     * @return Remove was successful or failed
     */
    bool remove(id::ref func_id);

    /**
     * @brief Get the running time
     * @return ms    Time since start of framework
     */
    ms get_running_time() const {
        return now() - start_time;
    }

    /**
     * @brief Get the running time in seconds
     * @return r64    Time since start of framework
     */
    r64 get_running_time_sec() const {
        return to_sec(get_running_time());
    }

    /**
     * @brief Get the command line arguments
     * @return cmd_line    Command line arguments
     */
    cmd_line get_cmd_line() const {
        return env.cmd_line;
    }

    /**
     * @brief Get the framework environment
     * @return frame_env::ref    Framework environment
     */
    frame_env::ref get_env() const {
        return env;
    }

    /**
     * @brief Get the name of application
     * @return name    Name of application
     */
    name get_name() const {
        return env.info.app_name;
    }

    /**
     * @brief Check if framework is waiting for events
     * @return Framework waits for events or not
     */
    bool waiting_for_events() const {
        return wait_for_events;
    }

    /**
     * @brief Set wait for events in framework
     * @param value    Wait for events state
     */
    void set_wait_for_events(bool value = true) {
        wait_for_events = value;
    }

    /// Run time
    run_time_t run_time;

    /// Stage platform
    platform platform;

    /// Message dispatcher
    message_dispatcher telegraph;

private:
    /**
     * @brief Set up the framework
     * @return Setup was successful or failed
     */
    bool setup();

    /**
     * @brief Tear down the framework
     */
    void teardown();

    /**
     * @brief Run a step
     * @return Run was successful or failed
     */
    bool run_step();

    /**
     * @brief Trigger run remove
     */
    void trigger_run_remove();

    /**
     * @brief Trigger run end
     */
    void trigger_run_end();

    /// Initialized state
    bool initialized = false;

    /// Framework environment
    frame_env env;

    /// Running state
    bool running = false;

    /// Wait for events state
    bool wait_for_events = false;

    /// Framework start time
    ms start_time;

    /// Map of run functions
    using run_func_map = std::map<id, run_func>;

    /// Map of run functions
    run_func_map run_map;

    /// Map fo run end functions
    using run_end_func_map = std::map<id, run_end_func>;

    /// Map fo run end functions
    run_end_func_map run_end_map;

    /// Map of run once functions
    using run_once_func_list = std::vector<run_once_func>;

    /// Map of run once functions
    run_once_func_list run_once_list;

    /// List of run ids to remove
    id::list run_remove_list;
};

/**
 * @brief Handle events
 * @param wait    Wait for events
 */
void handle_events(bool wait = false);

/**
 * @brief Handle events with timeout
 * @param timeout    Wait timeout in milliseconds
 */
void handle_events_timeout(ms timeout);

/**
 * @brief Handle events with timeout
 * @param timeout    Wait timeout in seconds
 */
void handle_events_timeout(seconds timeout);

/**
 * @brief Post an empty event
 */
void post_empty_event();

} // namespace lava
