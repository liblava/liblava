/**
 * @file         liblava-demo/demo.cpp
 * @brief        Demo helpers
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <imgui.h>
#include <demo.hpp>
#include <liblava/app/def.hpp>

namespace lava {

//-----------------------------------------------------------------------------
void setup_imgui_font_icons(imgui::font& font) {
    font.icon_file = fmt::format("{}{}", _font_icon_path_, FONT_ICON_FILE_NAME_FAS);

    font.icon_range_begin = ICON_MIN_FA;
    font.icon_range_end = ICON_MAX_FA;
}

} // namespace lava