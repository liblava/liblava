/**
 * @file         liblava/app/def.hpp
 * @brief        Application defines
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/types.hpp"

namespace lava {

/// app

constexpr name _pause_ = "pause";
constexpr name _screenshot_ = "screenshot";
constexpr name _quit_ = "quit";
constexpr name _reload_ = "reload";
constexpr name _on_ = "on";
constexpr name _off_ = "off";

/// config

constexpr name _app_ = "app";
constexpr name _delta_ = "delta";
constexpr name _imgui_ = "imgui";
constexpr name _paused_ = "paused";
constexpr name _physical_device_ = "physical device";
constexpr name _speed_ = "speed";
constexpr name _v_sync_ = "v-sync";

constexpr name _window_ = "window";
constexpr name _decorated_ = "decorated";
constexpr name _floating_ = "floating";
constexpr name _fullscreen_ = "fullscreen";
constexpr name _height_ = "height";
constexpr name _maximized_ = "maximized";
constexpr name _monitor_ = "monitor";
constexpr name _resizable_ = "resizable";
constexpr name _width_ = "width";
constexpr name _x_ = "x";
constexpr name _y_ = "y";

/// debug utils

constexpr name _lava_block_ = "lava block";
constexpr name _lava_gui_ = "lava gui";
constexpr name _lava_texture_staging_ = "lava texture staging";

/// res folder

constexpr name _font_path_ = "font/";
constexpr name _font_icon_path_ = "font/icon/";

/// benchmark

constexpr name _benchmark_ = "benchmark";
constexpr name _time_ = "time";
constexpr name _offset_ = "offset";
constexpr name _count_ = "count";
constexpr name _min_ = "min";
constexpr name _max_ = "max";
constexpr name _avg_ = "avg";
constexpr name _frames_ = "frames";
constexpr name _timestamps_ = "timestamps";
constexpr name _benchmark_json_ = "benchmark.json";

/// ImGui

constexpr name _imgui_file_ = "imgui.ini";
constexpr name _font_icon_ = "font_icon";

/// Cache

constexpr name _cache_path_ = "cache/";
constexpr name _pipeline_cache_file_ = "pipeline.cache";

} // namespace lava
