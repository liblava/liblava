// file      : liblava/app/app.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame.hpp>
#include <liblava/block.hpp>

#include <liblava/app/forward_shading.hpp>
#include <liblava/app/camera.hpp>
#include <liblava/app/gui.hpp>

namespace lava {

struct app : frame {

    explicit app(argh::parser argh);
    explicit app(frame_config config);   
    explicit app(name config_app, argh::parser argh);

    bool setup();

    lava::window window;
    lava::input input;

    lava::forward_shading forward_shading;

    lava::gui gui;
    lava::camera camera;

    lava::staging staging;
    lava::block block;

    lava::renderer renderer;
    lava::device* device = nullptr;

    lava::render_target::ptr render_target;

private:
    texture::ptr fonts;
    milliseconds last_time;
};

} // lava
