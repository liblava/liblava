<img align="left" src="https://raw.githubusercontent.com/liblava/liblava/master/res/texture/lava_block_logo_200.png">

**liblava is a modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/">Vulkan API</a>**

liblava is a lean framework that provides essentials for low-level graphics and is specially well suited for prototyping, tooling and education.

    + C++20 standard
    + Modular (5 modules)
    + Cross Platform (Windows | Linux)

![version](https://img.shields.io/badge/version-0.4.2-blue) [![LoC](https://tokei.rs/b1/github/liblava/liblava?category=code)](https://github.com/liblava/liblava) [![Build Status](https://travis-ci.com/liblava/liblava.svg?branch=master)](https://travis-ci.com/liblava/liblava) [![Build status](https://ci.appveyor.com/api/projects/status/gxvjpo73qf637hy3?svg=true)](https://ci.appveyor.com/project/TheLavaBlock/liblava) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE) [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/thelavablock)

## features

* written in modern C++
* latest Vulkan API support
* run loop abstraction
* window and input handling
* render target and renderer
* load texture and mesh
* file system and logging
* test driver
* and much more...

WIP latest **<a href="https://github.com/liblava/liblava/releases">preview 2 / v0.4.2</a>**  (Sep 18, 2019)

## hello frame

Let's write **Hello World** in Vulkan: *a simple program that just renders a colored window*

Well, just ? - Vulkan is a low-level, verbose graphics API and such a program can take several hundred lines of code.

All we need is a window, a device and a renderer.

The good news is that **liblava** will set up all for you.

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

    sleep(1.0);
    count++;

    log()->debug("{} - running {} sec", count, frame.get_running_time());

    if (count == 3)
        frame.shut_down();

    return true;
});

return frame.run() ? 0 : error::aborted;
```

The last line performs a loop with the run we added before. If *count* reaches 3 it will exit. 

#### 3. window input

Here is another example that shows how to create a window and handle input. This is straightforward.

```c++
frame frame(argh);
if (!frame.ready())
    return error::not_ready;

window window;
if (!window.create())
    return error::create_failed;

input input;
window.assign(&input);

input.key_listeners.add([&](key_event::ref event) {

    if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
        frame.shut_down();
});

frame.add_run([&]() {

    handle_events(input);

    if (window.has_close_request())
        frame.shut_down();

    return true;
});

return frame.run() ? 0 : error::aborted;
```

With this knowledge in hand let's write **hello frame**...

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

input.key_listeners.add([&](key_event::ref event) {

    if (event.key == GLFW_KEY_ESCAPE && event.action == GLFW_PRESS)
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

    VkCommandPoolCreateInfo create_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = device->get_graphics_queue().family_index,
    };
    if (!check(device->call().vkCreateCommandPool(device->get(), &create_info, memory::alloc(), &cmd_pool)))
        return false;

    VkCommandBufferAllocateInfo alloc_info
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = frame_count,
    };
    if (!check(device->call().vkAllocateCommandBuffers(device->get(), &alloc_info, cmd_bufs.data())))
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
        auto target_image = render_target->get_backbuffer_image(i);

        if (!check(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return false;

        insert_image_memory_barrier(device, cmd_buf, target_image,
                                    VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT,
                                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
                                    image_range);

        device->call().vkCmdClearColorImage(cmd_buf, target_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
                                            &clear_color, 1, &image_range);

        insert_image_memory_barrier(device, cmd_buf, target_image,
                                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT,
                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
                                    image_range);

        if (!check(device->call().vkEndCommandBuffer(cmd_buf)))
            return false;
    }

    return true;
};

auto clean_cmd_bufs = [&]() {

    for (auto& cmd_buf : cmd_bufs)
        device->call().vkFreeCommandBuffers(device->get(), cmd_pool, 1, &cmd_buf);

    device->call().vkDestroyCommandPool(device->get(), cmd_pool, memory::alloc());
};

build_cmd_bufs();

render_target->on_swapchain_start = build_cmd_bufs;
render_target->on_swapchain_stop = clean_cmd_bufs;

frame.add_run([&]() {

    handle_events(input);

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

Check the [Awesome Vulkan ecosystem](http://www.vinjn.com/awesome-vulkan/) to learn more about Vulkan (Tutorials/Samples/Books).

## tests

Run the driver to test [the above examples](https://github.com/liblava/liblava/blob/master/tests/tests.cpp).

List all tests:

```
$ lava -t
```

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
* [assimp](https://github.com/assimp/assimp) / modified 3-clause BSD
* [better-enums](https://github.com/aantron/better-enums) / 2-clause BSD
* [bitmap](https://github.com/ArashPartow/bitmap) / MIT
* [glfw](https://github.com/glfw/glfw) / zlib
* [gli](https://github.com/g-truc/gli) / MIT
* [glm](https://github.com/g-truc/glm) / MIT
* [json](https://github.com/nlohmann/json) / MIT
* [physfs](https://github.com/criptych/physfs) / zlib
* [selene](https://github.com/kmhofmann/selene) / MIT
* [spdlog](https://github.com/gabime/spdlog) / MIT
* [stb](https://github.com/nothings/stb) / MIT
* [tinyfiledialogs](https://github.com/native-toolkit/tinyfiledialogs) / zlib
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) / MIT
* [volk](https://github.com/zeux/volk) / MIT
* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) / Apache 2.0
* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) / MIT

## license

liblava is licensed under [MIT License](LICENSE.md) which allows you to use the software for any purpose you might like, including commercial and for-profit use. However, this library includes several third-party Open-Source libraries, which are licensed under their own respective Open-Source licenses. These licenses allow static linking with closed source software. All copies of liblava must include a copy of the MIT License terms and the copyright notice.

Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a>

<img src="https://raw.githubusercontent.com/liblava/liblava/master/res/texture/lava_block_logo_50.png">