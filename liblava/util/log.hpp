/**
 * @file         liblava/util/log.hpp
 * @brief        Logging
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <liblava/core/version.hpp>
#include <memory>

namespace lava {

/// Default log file
constexpr name _lava_log_file_ = "lava.log";

/// Logger
using logger = std::shared_ptr<spdlog::logger>;

/**
 * @brief Convert id and name to string
 * @param id         Id to convert
 * @param name       Name to convert
 * @return string    String representation
 */
inline string to_string(string_ref id, string_ref name) {
    return fmt::format("{} | {}", id, name);
}

/**
 * @brief Convert internal version to string
 * @param version    Internal version to convert
 * @return string    String representation
 */
inline string to_string(int_version const& version) {
    return fmt::format("{}.{}.{}",
                       version.major,
                       version.minor,
                       version.patch);
}

/**
 * @brief Convert global internal version to string
 * @return string    String representation
 */
inline string internal_version_string() {
    return to_string(int_version{});
}

/**
 * @see internal_version_string
 */
inline string int_version_string() {
    return internal_version_string();
}

/**
 * @brief Convert version stage to string
 * @param stage    Version stage to convert
 * @return name    Name representation
 */
inline name to_string(version_stage stage) {
    switch (stage) {
    case version_stage::preview:
        return "preview";
    case version_stage::alpha:
        return "alpha";
    case version_stage::beta:
        return "beta";
    case version_stage::rc:
        return "rc";
    default:
        return "";
    }
}

/**
 * @brief Convert version to string
 * @param version    Version to convert
 * @return string    String representation
 */
inline string to_string(version const& version) {
    string stage_str = to_string(version.stage);
    if ((version.rev > 1) && (version.stage != version_stage::release))
        stage_str += fmt::format(" {}", version.rev);

    if (version.release == 0) {
        if (stage_str.empty())
            return fmt::format("{}", version.year);
        else
            return fmt::format("{} {}",
                               version.year, stage_str);
    } else
        return fmt::format("{}.{} {}",
                           version.year, version.release, stage_str);
}

/**
 * @brief Convert global version to string
 * @return string    String representation
 */
inline string version_string() {
    return to_string(version{});
}

/**
 * @brief Log configuration
 */
struct log_config {
    /// Logger name
    name logger = _lava_;

    /// Log file
    name file = _lava_log_file_;

    /// Log level
    i32 level = -1;

    /// Log to console, else file
    bool debug = false;
};

/**
 * @brief Set up logging
 * @param config    Log configuration
 */
inline logger setup_log(log_config config = {}) {
    if (config.debug) {
        auto log = spdlog::stdout_color_mt(config.logger);
        log->set_level((config.level < 0)
                           ? spdlog::level::debug
                           : (spdlog::level::level_enum) config.level);
        return log;
    } else {
        auto log = spdlog::basic_logger_mt(config.logger, config.file);
        log->set_level((config.level < 0)
                           ? spdlog::level::warn
                           : (spdlog::level::level_enum) config.level);
        return log;
    }
}

/**
 * @brief Tear down logging
 * @param config    Log configuration
 */
inline void teardown_log(log_config config = {}) {
    spdlog::drop(config.logger);
}

/**
 * @brief Global logger
 */
struct global_logger {
    /**
     * @brief Get global logger singleton
     * @return log_global&    Global logger
     */
    static global_logger& singleton() {
        static global_logger global_logger;
        return global_logger;
    }

    /**
     * @brief Get logger
     * @return logger    Logger
     */
    logger get() {
        return log;
    }

    /**
     * @brief Set logger
     * @param l    Logger
     */
    void set(lava::logger l) {
        log = l;
    }

    /**
     * @brief Reset logger
     */
    void reset() {
        log = nullptr;
    }

private:
    /// Logger
    logger log;
};

/**
 * @brief Get global logger
 * @return logger    Logger
 */
inline logger log() {
    return global_logger::singleton().get();
}

} // namespace lava
