// file      : liblava/app/forward_shading.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/render_target.hpp>
#include <liblava/block/render_pass.hpp>

namespace lava {

struct forward_shading : id_obj {

    explicit forward_shading() = default;
    ~forward_shading() { destroy(); }

    bool create(render_target::ptr target);
    void destroy();

    render_pass::ptr get_render_pass() const { return pass; }
    image::ptr get_depth_stencil() const { return depth_stencil; }

private:
    render_target::ptr target;

    render_pass::ptr pass;
    image::ptr depth_stencil;
};

} // lava
