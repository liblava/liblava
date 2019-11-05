<a href="https://lava-block.com"><img align="left" src="https://github.com/liblava.png" width="200"></a>

**liblava is a modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/">Vulkan API</a>**

**liblava** is a lean framework that provides **essentials** for low-level graphics and is specially well suited for **prototyping**, **tooling** and **education**. (<a href="https://git.io/liblava-demo">demo</a>) (<a href="https://git.io/liblava-template">template</a>)

    + C++20
    + Modular (7 modules)
    + Cross Platform (Windows | Linux | soon: macOS)

![version](https://img.shields.io/badge/version-0.4.3-blue) [![LoC](https://tokei.rs/b1/github/liblava/liblava?category=code)](https://github.com/liblava/liblava) [![Build Status](https://travis-ci.com/liblava/liblava.svg?branch=master)](https://travis-ci.com/liblava/liblava) [![Build status](https://ci.appveyor.com/api/projects/status/gxvjpo73qf637hy3?svg=true)](https://ci.appveyor.com/project/TheLavaBlock/liblava) [![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)](LICENSE) [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/thelavablock)

## features

* written in modern C++
* latest Vulkan API support
* run loop abstraction
* window and input handling
* render target and renderer
* command buffer modeling
* load texture and mesh
* file system and logging
* gui and test driver
* and much more...

Download latest **<a href="https://github.com/liblava/liblava/releases">preview 2 / v0.4.2</a>**  (Sep 18, 2019) and **<a href="https://github.com/liblava/liblava-demo/releases">demo projects</a>**

## modules

[![core](https://img.shields.io/badge/lava-core-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/util) [![base](https://img.shields.io/badge/lava-base-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/base) [![resource](https://img.shields.io/badge/lava-resource-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/resource) [![frame](https://img.shields.io/badge/lava-frame-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/block) [![tool](https://img.shields.io/badge/lava-tool-yellow.svg)](https://github.com/liblava/liblava/tree/master/liblava/tool) 

## hello frame

Let's write **Hello World** in Vulkan: *"a simple program that renders a colored window"*

All we need is a window, a device and a renderer.

Vulkan is a low-level, verbose graphics API and such a program can take several hundred lines of code.

The good news is that **liblava** will set it up for you.

```c++
#include <liblava/lava.h>

using namespace lava;
```

Here are a few examples to get to know `lava`.

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

return frame.run() ? 0 : error::aborted;
```

The last line performs a loop with the run we added before. If *count* reaches 3 it will exit. 

#### 3. window input

Here is another example that shows how to create a window and handle input:

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

    if (window.has_close_request())
        frame.shut_down();

    return true;
});

return frame.run() ? 0 : error::aborted;
```

Straightforward. With this knowledge in hand let's write **hello frame**...

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

renderer simple_renderer;
if (!simple_renderer.create(render_target->get_swapchain()))
    return error::create_failed;

auto frame_count = render_target->get_frame_count();

VkCommandPool cmd_pool;
VkCommandBuffers cmd_bufs(frame_count);

auto build_cmd_bufs = [&]() {

    if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
        return false;

    if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
        return false;

    VkCommandBufferBeginInfo begin_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    VkClearColorValue clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

    VkImageSubresourceRange image_range
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

    if (window.has_close_request())
        return frame.shut_down();

    if (window.has_resize_request())
        return window.handle_resize();

    if (window.iconified()) {

        frame.set_wait_for_events(true);
        return true;

    } else {

        if (frame.waiting_for_events())
            frame.set_wait_for_events(false);
    }

    auto frame_index = simple_renderer.begin_frame();
    if (!frame_index)
        return true;

    return simple_renderer.end_frame({ cmd_bufs[*frame_index] });
});

frame.add_run_end([&]() {

    clean_cmd_bufs();
    
    simple_renderer.destroy();
    render_target->destroy();
});

return frame.run() ? 0 : error::aborted;
```

Welcome on **Planet Vulkan**. That's a lot to display a colored window.

Take a closer look at the `build_cmd_bufs` function: 

- We create a command pool and command buffers for each frame of the render target. 
- We set each command buffer to clear the frame image with some random color. 

Watch out the *VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT* flag that specifies the reusage of command buffers.

The `clean_cmd_bufs` function frees all buffers and destroys the command pool. 

In case of swap chain restoration we simply recreate the command buffers with a new random color. This happens for example on window resize.

That's a static example with command buffer reusage.

Vulkan supports a more dynamic and common usage by resetting the command pool before recording commands. 

It's time for... `lava::block`

#### 5. color block

```c++
block block;

if (!block.create(device, frame_count, device->graphics_queue().family))
    return error::create_failed;

block.add_command([&](VkCommandBuffer cmd_buf) {

    VkClearColorValue clear_color = { random(0.f, 1.f), random(0.f, 1.f), random(0.f, 1.f), 0.f };

    VkImageSubresourceRange image_range
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

This is much more simpler than before: 

We create a `lava::block` and add the command that clears the current frame image.

Now all we need to do is to process the block in the run loop...

```c++
if (!block.process(*frame_index))
    return false;

return simple_renderer.end_frame(block.get_buffers());
```

... and call the renderer with the recorded command buffers.

Don't forget to destroy the block at the run end:

```c++
block.destroy();
```

Check [Awesome Vulkan ecosystem](http://www.vinjn.com/awesome-vulkan/) for tutorials, samples and books.

## tests

Run the driver to test [the above examples](https://github.com/liblava/liblava/blob/master/tests/tests.cpp).

List all tests:

```
$ lava -t
```

1. frame init
2. run loop
3. window input
4. clear color
5. color block
6. gamepad listener
7. forward shading
8. imgui demo

Run test 2 for example:

```
$ lava 2
```

## requirements

* **C++20** compatible compiler
* CMake **3.12+**
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

Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a>

<a href="https://lava-block.com"><img src="https://github.com/liblava.png" width="50"></a>
