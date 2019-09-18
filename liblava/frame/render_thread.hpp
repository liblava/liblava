// file      : liblava/frame/render_thread.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/frame/renderer.hpp>

#include <thread>

namespace lava {

struct render_thread {

    ~render_thread() { destroy(); }

    bool create(swapchain* swapchain) {

        _renderer.on_destroy = [&]() { stop(); };

        return _renderer.create(swapchain);
    }

    void destroy() { _renderer.destroy(); }

    void start() { thread = std::thread(&render_thread::render, this); }

    void stop() {

        if (!running)
            return;

        running = false;
        thread.join();
    }

    using render_func = std::function<VkCommandBuffers(ui32)>;
    render_func on_render;

    renderer* get_renderer() { return &_renderer; }
    bool is_running() const { return running; }

private:
    void render() {

        running = true;

        while (running) {

            if (!_renderer.active)
                continue;

            if (!on_render)
                continue;

            auto frame_index = _renderer.begin_frame();
            if (!frame_index)
                continue;

            _renderer.end_frame(on_render(*frame_index));
        }
    }

    std::thread thread;
    bool running = false;

    renderer _renderer;
};

} // lava
