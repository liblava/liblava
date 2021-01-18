// file      : liblava-demo/demo.cpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#include <demo.hpp>
#include <liblava/app/def.hpp>

void lava::setup_imgui_font_icons(imgui::font& font) {
    font.icon_file = fmt::format("{}{}", _font_icon_path_, FONT_ICON_FILE_NAME_FAS);

    font.icon_range_begin = ICON_MIN_FA;
    font.icon_range_end = ICON_MAX_FA;
}
