<a href="https://lava-block.com"><img align="left" src="https://github.com/liblava.png" width="200"></a>

**liblava is a modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/">Vulkan</a>® API**

**lava** is a lean framework that provides **essentials** for low-level graphics and is specially well suited for **prototyping**, **tooling** and **education** • <a href="https://git.io/liblava-demo">demo</a>  / <a href="https://git.io/liblava-template">template</a> / <a href="https://git.io/liblava-engine">engine</a>

**C++20** • **Modular** • **Cross Platform** ( Windows / Linux )

[![Version](https://img.shields.io/badge/version-0.4.5-blue)](https://git.io/liblava) [![Build Status](https://travis-ci.com/liblava/liblava.svg?branch=master)](https://travis-ci.com/liblava/liblava) [![Build status](https://ci.appveyor.com/api/projects/status/gxvjpo73qf637hy3?svg=true)](https://ci.appveyor.com/project/TheLavaBlock/liblava) [![License](https://img.shields.io/github/license/liblava/liblava)](LICENSE) [![Donate](https://img.shields.io/badge/donate-PayPal-green.svg)](https://paypal.me/liblava) [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/liblava)

#### features

* written in **modern C++** with latest **Vulkan** support
* **run loop** abstraction for **window** and **input handling**
* plain **renderer** and **command buffer model**
* **texture** and **mesh** **loading** from **virtual file system**
* **camera**, **gui**, **logging**, **test driver** and much more...

##### Download latest **<a href="https://github.com/liblava/liblava-demo/releases">2019 demo / v0.4.4</a>**  (Dec 6, 2019)

<a href="https://github.com/liblava/liblava-demo/#readme">![demo](res/demo.png)</a>

#### modules

[![core](https://img.shields.io/badge/lava-core-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/util) [![base](https://img.shields.io/badge/lava-base-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/base) [![resource](https://img.shields.io/badge/lava-resource-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/resource) [![frame](https://img.shields.io/badge/lava-frame-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/block) [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](https://github.com/liblava/liblava/tree/master/liblava/app) [![engine](https://img.shields.io/badge/lava-engine-brightgreen.svg)](https://git.io/liblava-engine)

## hello frame

Let's write **Hello World** in Vulkan...

<a href="https://www.khronos.org/vulkan/"><img align="right" hspace="20" src="res/Vulkan_100px_Dec16.png"></a>

**"a simple app that renders a colored window"**

All we need is a `window`, `device` and `renderer`. Vulkan is a low-level, verbose graphics API and such a program can take several hundred lines of code.

The good news is that **liblava** will help you!

```c++
#include <liblava/lava.hpp>

using namespace lava;
```

Here are a few examples to get to know `lava`

#### 1. frame init

```c++
int main(int argc, char* argv[]) {

    frame frame( {argc, argv} );

    return frame.ready() ? 0 : error::not_ready;
}
```

This is how to initialize `lava::frame` with command line arguments.

#### 2. run loop

```c++
frame frame(argh);
if (!frame.ready())
    return error::not_ready;

auto count = 0;

frame.add_run([&]() {

    sleep(seconds(1));
    count++;

    log()->debug("{} - running {} sec", count, to_sec(frame.get_running_time()));

    if (count == 3)
        frame.shut_down();

    return true;
});

return frame.run();
```

The last line performs a loop with a **run** we added before. If *count* reaches 3 the **loop** will exit.

#### 3. window input

Here is another example that shows how to create a `lava::window` and handle `lava::input`

```c++
frame frame(argh);
if (!frame.ready())
    return error::not_ready;

window window;
if (!window.create())
    return error::create_failed;

input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {

    if (event.pressed(key::escape))
        frame.shut_down();
});

frame.add_run([&]() {

    input.handle_events();

    if (window.close_request())
        frame.shut_down();

    return true;
});

return frame.run();
```

*Straightforward* - with this knowledge in hand let's write **"hello frame"** now...

#### 4. clear color

```c++
frame frame(argh);
if (!frame.ready())
    return error::not_ready;

window window;
if (!window.create())
    return error::create_failed;

input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {

    if (event.pressed(key::escape))
        frame.shut_down();
});

auto device = frame.create_device();
if (!device)
    return error::create_failed;

auto render_target = create_target(&window, device);
if (!render_target)
    return error::create_failed;

renderer plotter;
if (!plotter.create(render_target->get_swapchain()))
    return error::create_failed;

auto frame_count = render_target->get_frame_count();

VkCommandPool cmd_pool;
VkCommandBuffers cmd_bufs(frame_count);

auto build_cmd_bufs = [&]() {

    if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
        return false;

    if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
        return false;

    VkCommandBufferBeginInfo const begin_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    VkClearColorValue const clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

    VkImageSubresourceRange const image_range
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    for (auto i = 0u; i < frame_count; i++) {

        auto cmd_buf = cmd_bufs[i];
        auto frame_image = render_target->get_image(i);

        if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                    image_range);

        device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            &clear_color, 1, &image_range);

        insert_image_memory_barrier(device, cmd_buf, frame_image,
                                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                    image_range);

        if (failed(device->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
};

auto clean_cmd_bufs = [&]() {

    device->vkFreeCommandBuffers(cmd_pool, frame_count, cmd_bufs.data());
    device->vkDestroyCommandPool(cmd_pool);
};

if (!build_cmd_bufs())
    return error::create_failed;

render_target->on_swapchain_start = build_cmd_bufs;
render_target->on_swapchain_stop = clean_cmd_bufs;

frame.add_run([&]() {

    input.handle_events();

    if (window.close_request())
        return frame.shut_down();

    if (window.resize_request())
        return window.handle_resize();

    auto frame_index = plotter.begin_frame();
    if (!frame_index)
        return true;

    return plotter.end_frame({ cmd_bufs[*frame_index] });
});

frame.add_run_end([&]() {

    clean_cmd_bufs();

    plotter.destroy();
    render_target->destroy();
});

return frame.run();
```

##### Welcome on **Planet Vulkan**. That's a lot to render a colored window.

Phew! Take a closer look at the `build_cmd_bufs` function:

1. We create a command pool and command buffers for each frame of the render target
2. We set each command buffer to clear the frame image with some random color

Watch out the *VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT* flag that specifies the reusage of command buffers.

`clean_cmd_bufs` frees all buffers and destroys the command pool. In case of swap chain restoration we simply recreate command buffers with a new random color. This happens for example on window resize.

After all, this is a very static example. Vulkan supports a more *dynamic* and common usage by resetting a command pool before recording new commands.

Ok, it's time for... `lava::block`

#### 5. color block

```c++
block block;

if (!block.create(device, frame_count, device->graphics_queue().family))
    return error::create_failed;

block.add_command([&](VkCommandBuffer cmd_buf) {

    VkClearColorValue const clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

    VkImageSubresourceRange const image_range
    {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    auto frame_image = render_target->get_image(block.get_current_frame());

    insert_image_memory_barrier(device, cmd_buf, frame_image,
                                VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                image_range);

    device->call().vkCmdClearColorImage(cmd_buf, frame_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        &clear_color, 1, &image_range);

    insert_image_memory_barrier(device, cmd_buf, frame_image,
                                VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                image_range);
});
```

##### Nice, this is much more simpler than before:

We create a `lava::block` and add one **command** that clears the current frame image.

All we need to do is to process the block in the run loop...

```c++
if (!block.process(*frame_index))
    return false;

return plotter.end_frame(block.get_buffers());
```

... and call the renderer with our recorded command buffers.

Don't forget to clean it up:

```c++
block.destroy();
```

##### New to Vulkan? Take a look at this [Vulkan Guide](https://github.com/KhronosGroup/Vulkan-Guide)

Check [Awesome Vulkan ecosystem](http://www.vinjn.com/awesome-vulkan/) for tutorials, samples and books.

#### 8. imgui demo

Out of blocks, `lava::app` supports the awesome [Dear ImGui](https://github.com/ocornut/imgui) for **tooling** and **easy prototyping**.

```c++
int main(int argc, char* argv[]) {

    app app("demo", { argc, argv });
    if (!app.setup())
        return error::not_ready;

    app.gui.on_draw = []() {

        ImGui::ShowDemoWindow();
    };

    return app.run();
}
```

##### What's next? - Check <a href="https://git.io/liblava-demo">demonstration projects</a> and clone a <a href="https://git.io/liblava-template">starter template</a>

## tests

Run the **lava** executable to test [above examples](https://github.com/liblava/liblava/blob/master/tests/tests.cpp). Let it simply flow...

##### List all tests:

```
$ lava -t
```

1. [frame init](#1-frame-init)
2. [run loop](#2-run-loop)
3. [window input](#3-window-input)
4. [clear color](#4-clear-color)
5. [color block](#5-color-block)
6. forward shading
7. gamepad
8. [imgui demo](#8-imgui-demo)

##### Run test 2 for example:

```
$ lava 2
```

The **driver** starts per default the last test (*= without command line arguments*)

## requirements

* **C++20** compatible compiler
* CMake **3.15+**
* [Vulkan SDK](https://vulkan.lunarg.com)

## build

```
$ git clone https://github.com/liblava/liblava.git
$ cd liblava

$ git submodule update --init --recursive

$ mkdir build
$ cd build

$ cmake ..
$ make
```

## third-party / license

* [argh](https://github.com/adishavit/argh) / 3-clause BSD
* [bitmap](https://github.com/ArashPartow/bitmap) / MIT
* [glfw](https://github.com/glfw/glfw) / zlib
* [gli](https://github.com/g-truc/gli) / MIT
* [glm](https://github.com/g-truc/glm) / MIT
* [imgui](https://github.com/ocornut/imgui) / MIT
* [json](https://github.com/nlohmann/json) / MIT
* [physfs](https://github.com/criptych/physfs) / zlib
* [selene](https://github.com/kmhofmann/selene) / MIT
* [spdlog](https://github.com/gabime/spdlog) / MIT
* [stb](https://github.com/nothings/stb) / MIT
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) / MIT
* [volk](https://github.com/zeux/volk) / MIT
* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) / Apache 2.0
* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) / MIT

## license

liblava is licensed under [MIT License](LICENSE.md) which allows you to use the software for any purpose you might like, including commercial and for-profit use. However, this library includes several third-party Open-Source libraries, which are licensed under their own respective Open-Source licenses. These licenses allow static linking with closed source software. All copies of liblava must include a copy of the MIT License terms and the copyright notice.

##### Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a>
##### Vulkan and the Vulkan logo are trademarks of the [Khronos Group Inc.](http://www.khronos.org)

<a href="https://lava-block.com"><img src="https://github.com/liblava.png" width="50"></a>
