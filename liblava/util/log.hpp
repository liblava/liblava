// file      : liblava/util/log.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <liblava/core/types.hpp>
#include <liblava/core/version.hpp>

namespace lava {

    using logger = std::shared_ptr<spdlog::logger>;

    inline logger log(name name = _lava_) {
        return spdlog::get(name);
    }

    inline string to_string(string_ref id, string_ref name) {
        return fmt::format("{} | {}", str(id), str(name));
    }

    inline string to_string(internal_version const& version) {
        return fmt::format("{}.{}.{}", version.major, version.minor, version.patch);
    }

    inline string internal_version_string() {
        return to_string(internal_version{});
    }

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

    inline string to_string(version const& version) {
        string stage_str = to_string(version.stage);
        if ((version.rev > 1) && (version.stage != version_stage::release))
            stage_str += fmt::format(" {}", version.rev);

        if (version.release == 0) {
            if (stage_str.empty())
                return fmt::format("{}", version.year);
            else
                return fmt::format("{} {}", version.year, str(stage_str));
        } else
            return fmt::format("{}.{} {}", version.year, version.release, str(stage_str));
    }

    inline string version_string() {
        return to_string(version{});
    }

    constexpr name _lava_log_file_ = "lava.log";

    struct log_config {
        name logger = _lava_;
        name file = _lava_log_file_;

        i32 level = -1;
        bool debug = false;
    };

    inline void setup_log(log_config config = {}) {
        if (config.debug) {
            auto log = spdlog::stdout_color_mt(config.logger);
            log->set_level((config.level < 0) ? spdlog::level::debug : (spdlog::level::level_enum) config.level);
        } else {
            auto log = spdlog::basic_logger_mt(config.logger, config.file);
            log->set_level((config.level < 0) ? spdlog::level::warn : (spdlog::level::level_enum) config.level);
        }
    }

    template<typename T>
    inline string icon_text(T value) {
        return fmt::format("{}", icon(value));
    }

} // namespace lava
