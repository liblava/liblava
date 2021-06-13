/**
 * @file         liblava/app/def.hpp
 * @brief        Application defines
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/core/types.hpp>

namespace lava {

constexpr name _reload_ = "reload";

/// config

constexpr name _paused_ = "paused";
constexpr name _speed_ = "speed";
constexpr name _auto_save_ = "auto save";
constexpr name _save_interval_ = "save interval";
constexpr name _auto_load_ = "auto load";
constexpr name _fixed_delta_ = "fixed delta";
constexpr name _delta_ = "delta";
constexpr name _imgui_ = "imgui";
constexpr name _v_sync_ = "v-sync";
constexpr name _physical_device_ = "physical device";

/// debug utils

constexpr name _lava_block_ = "lava block";
constexpr name _lava_gui_ = "lava gui";
constexpr name _lava_texture_staging_ = "lava texture staging";

/// res folder

constexpr name _font_path_ = "font/";
constexpr name _font_icon_path_ = "font/icon/";

} // namespace lava
