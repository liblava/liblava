<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.6.2-blue)](https://git.io/liblava)

[Features](#features) &nbsp; **[Tutorial](#tutorial)** &nbsp; [Requirements](#requirements) &nbsp; **[Modules](#modules)** &nbsp; [Third-Party](#third-party) &nbsp; **[Guide](#guide)** &nbsp; [Tests](#tests) &nbsp; **[Demo](#demo)** &nbsp; [Template](#template) &nbsp; **[Build](#build)** &nbsp; [Install](#install)

<br />

## Features

*WIP*

<br />

## Tutorial

<a href="https://www.khronos.org/vulkan/" target="_blank"><img align="right" hspace="20" src="res/Vulkan_170px_Dec16.png" width="300"></a>

Let's write **Hello World** in Vulkan...

<br />

**"a simple app that renders a colored window"**

➜ All we need is a `window` + `device` and `renderer`

<br />

**Vulkan** is a low-level, verbose graphics API and such a program can take several hundred lines of code

The good news is that **liblava** will help you!

```c++
#include <liblava/lava.hpp>

using namespace lava;
```
<br />

Here are a few examples to get to know `lava`

<br />

#### 1. frame init

```c++
int main(int argc, char* argv[]) {

    lava::frame frame( {argc, argv} );
    
    return frame.ready() ? 0 : error::not_ready;
}
```

This is how to initialize `lava frame` with command line arguments

<br />

#### 2. run loop

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

auto count = 0;

frame.add_run([&]() {
    sleep(one_second);
    count++;

    log()->debug("{} - running {} sec", count, frame.get_running_time_sec());

    if (count == 3)
        return frame.shut_down();

    return run_continue;
});

return frame.run();
```

The last line performs a loop with the **run** we added before - If *count* reaches 3 that **loop** will exit

<br />

#### 3. window input

Here is another example that shows how to create a `lava window` and handle `lava input`

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

lava::window window;
if (!window.create())
    return error::create_failed;

lava::input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {
    if (event.pressed(key::escape))
        return frame.shut_down();
    
    return input_ignore;
});

frame.add_run([&]() {
    input.handle_events();

    if (window.close_request())
        return frame.shut_down();

    return run_continue;
});

return frame.run();
```

<br />

Straightforward ➜ With this knowledge in hand let's write our **Hello World** now...

#### 4. clear color

```c++
lava::frame frame(argh);
if (!frame.ready())
    return error::not_ready;

lava::window window;
if (!window.create())
    return error::create_failed;

lava::input input;
window.assign(&input);

input.key.listeners.add([&](key_event::ref event) {
    if (event.pressed(key::escape))
        return frame.shut_down();

    return input_ignore;
});

auto device = frame.create_device();
if (!device)
    return error::create_failed;

auto render_target = create_target(&window, device);
if (!render_target)
    return error::create_failed;

lava::renderer plotter;
if (!plotter.create(render_target->get_swapchain()))
    return error::create_failed;

auto frame_count = render_target->get_frame_count();

VkCommandPool cmd_pool;
VkCommandBuffers cmd_bufs(frame_count);

auto build_cmd_bufs = [&]() {
    if (!device->vkCreateCommandPool(device->graphics_queue().family, &cmd_pool))
        return build_failed;

    if (!device->vkAllocateCommandBuffers(cmd_pool, frame_count, cmd_bufs.data()))
        return build_failed;

    VkCommandBufferBeginInfo const begin_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    VkClearColorValue const clear_color = { random(1.f), random(1.f), random(1.f), 0.f };

    VkImageSubresourceRange const image_range{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .levelCount = 1,
        .layerCount = 1,
    };

    for (auto i = 0u; i < frame_count; i++) {
        auto cmd_buf = cmd_bufs[i];
        auto frame_image = render_target->get_image(i);

        if (failed(device->call().vkBeginCommandBuffer(cmd_buf, &begin_info)))
            return build_failed;

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
            return build_failed;
    }

    return build_done;
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
        return run_continue;

    return plotter.end_frame({ cmd_bufs[*frame_index] });
});

frame.add_run_end([&]() {
    clean_cmd_bufs();

    plotter.destroy();
    render_target->destroy();
});

return frame.run();
```

##### Welcome on **Planet Vulkan** - That's a lot to display a colored window

<br />

Phew! Take a closer look at the `build_cmd_bufs` function:

* We create a **command pool** and **command buffers** for each frame of the render target
* And set each command buffer to clear the frame image with some random color

<br />

The *VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT* flag specifies the reusage of command buffers

`clean_cmd_bufs` frees all buffers and destroys the command pool

In case of swap chain restoration we simply recreate command buffers with a new random color

This happens for example on window resize

<br />

After all, this is a very static example 

➜ Vulkan supports a more *dynamic* and common usage by resetting a command pool before recording new commands

<br />

Ok, it's time for... `lava block`

#### 5. color block

```c++
lava::block block;

if (!block.create(device, frame_count, device->graphics_queue().family))
    return error::create_failed;

block.add_command([&](VkCommandBuffer cmd_buf) {
    VkClearColorValue const clear_color = { random(1.f), random(1.f), random(1.f), 0.f };

    VkImageSubresourceRange const image_range{
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

##### Nice, this is much more simpler than before

<br />

We create a `lava block` and add just one **command** that clears the current frame image

<br />

All we need to do now is to process the block in the run loop:

```c++
if (!block.process(*frame_index))
    return run_abort;

return plotter.end_frame(block.get_buffers());
```

And call the renderer with our recorded command buffers

<br />

Don't forget to clean it up:

```c++
block.destroy();
```

<br />

##### New to Vulkan? Take a look at this [Vulkan Guide](https://github.com/KhronosGroup/Vulkan-Guide)

Check [Awesome Vulkan ecosystem](http://www.vinjn.com/awesome-vulkan/) for tutorials, samples and books

<br />

#### 8. imgui demo

Out of blocks, `lava app` supports [Dear ImGui](https://github.com/ocornut/imgui) for **tooling** and **easy prototyping**

```c++
int main(int argc, char* argv[]) {

    lava::app app("demo", { argc, argv });
    if (!app.setup())
        return error::not_ready;

    app.imgui.on_draw = []() {

        ImGui::ShowDemoWindow();
    };

    return app.run();
}
```

<br />

##### What's next? ➜ Check some [demos](liblava-demo) and use the [template](#template) to try it out

<br />

## Requirements

* **C++20** compatible compiler
* CMake **3.20+**
* [Vulkan SDK](https://vulkan.lunarg.com)

<br />

## Modules

#### lava [demo](liblava-demo) &nbsp; ➜ app

[![lamp](https://img.shields.io/badge/lava-lamp-brightgreen.svg)](liblava-demo/lamp.cpp) [![spawn](https://img.shields.io/badge/lava-spawn-brightgreen.svg)](liblava-demo/spawn.cpp) [![triangle](https://img.shields.io/badge/lava-triangle-brightgreen.svg)](liblava-demo/triangle.cpp)

#### lava [app](liblava/app) &nbsp; ➜ block + frame + asset

[![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](liblava/app/app.hpp) [![camera](https://img.shields.io/badge/lava-camera-brightgreen.svg)](liblava/app/camera.hpp) [![config](https://img.shields.io/badge/lava-config-brightgreen.svg)](liblava/app/config.hpp) [![forward_shading](https://img.shields.io/badge/lava-forward_shading-brightgreen.svg)](liblava/app/forward_shading.hpp) [![imgui](https://img.shields.io/badge/lava-imgui-brightgreen.svg)](liblava/app/imgui.hpp)

#### lava [block](liblava/block) &nbsp; ➜ base

[![attachment](https://img.shields.io/badge/lava-attachment-red.svg)](liblava/block/attachment.hpp) [![block](https://img.shields.io/badge/lava-block-red.svg)](liblava/block/block.hpp) [![descriptor](https://img.shields.io/badge/lava-descriptor-red.svg)](liblava/block/descriptor.hpp) [![pipeline](https://img.shields.io/badge/lava-pipeline-red.svg)](liblava/block/pipeline.hpp) [![render_pass](https://img.shields.io/badge/lava-render_pass-red.svg)](liblava/block/render_pass.hpp) [![subpass](https://img.shields.io/badge/lava-subpass-red.svg)](liblava/block/subpass.hpp)

#### lava [frame](liblava/frame) &nbsp; ➜ resource

[![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.hpp) [![input](https://img.shields.io/badge/lava-input-red.svg)](liblava/frame/input.hpp) [![render_target](https://img.shields.io/badge/lava-render_target-red.svg)](liblava/frame/render_target.hpp) [![renderer](https://img.shields.io/badge/lava-renderer-red.svg)](liblava/frame/renderer.hpp) [![swapchain](https://img.shields.io/badge/lava-swapchain-red.svg)](liblava/frame/swapchain.hpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.hpp)

#### lava [asset](liblava/asset) &nbsp; ➜ resource + file

[![mesh_loader](https://img.shields.io/badge/lava-mesh_loader-orange.svg)](liblava/asset/mesh_loader.hpp) [![scope_image](https://img.shields.io/badge/lava-scope_image-orange.svg)](liblava/asset/scope_image.hpp) [![texture_loader](https://img.shields.io/badge/lava-texture_loader-orange.svg)](liblava/asset/texture_loader.hpp)

#### lava [resource](liblava/resource) &nbsp; ➜ base

[![buffer](https://img.shields.io/badge/lava-buffer-orange.svg)](liblava/resource/buffer.hpp) [![format](https://img.shields.io/badge/lava-format-orange.svg)](liblava/resource/format.hpp) [![image](https://img.shields.io/badge/lava-image-orange.svg)](liblava/resource/image.hpp) [![mesh](https://img.shields.io/badge/lava-mesh-orange.svg)](liblava/resource/mesh.hpp) [![texture](https://img.shields.io/badge/lava-texture-orange.svg)](liblava/resource/texture.hpp)

#### lava [base](liblava/base) &nbsp; ➜ util

[![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp) [![device](https://img.shields.io/badge/lava-device-orange.svg)](liblava/base/device.hpp) [![instance](https://img.shields.io/badge/lava-instance-orange.svg)](liblava/base/instance.hpp) [![memory](https://img.shields.io/badge/lava-memory-orange.svg)](liblava/base/memory.hpp) [![physical_device](https://img.shields.io/badge/lava-physical_device-orange.svg)](liblava/base/physical_device.hpp) [![queue](https://img.shields.io/badge/lava-queue-orange.svg)](liblava/base/queue.hpp)

#### lava [file](liblava/file) &nbsp; ➜ util

[![file](https://img.shields.io/badge/lava-file-blue.svg)](liblava/file/file.hpp) [![file_system](https://img.shields.io/badge/lava-file_system-blue.svg)](liblava/file/file_system.hpp) [![file_utils](https://img.shields.io/badge/lava-file_utils-blue.svg)](liblava/file/file_utils.hpp) [![json_file](https://img.shields.io/badge/lava-json_file-blue.svg)](liblava/file/json_file.hpp)

#### lava [util](liblava/util) &nbsp; ➜ core

[![log](https://img.shields.io/badge/lava-log-blue.svg)](liblava/util/log.hpp) [![random](https://img.shields.io/badge/lava-random-blue.svg)](liblava/util/random.hpp) [![telegram](https://img.shields.io/badge/lava-telegram-blue.svg)](liblava/util/telegram.hpp) [![thread](https://img.shields.io/badge/lava-thread-blue.svg)](liblava/util/thread.hpp) [![utility](https://img.shields.io/badge/lava-utility-blue.svg)](liblava/util/utility.hpp)

#### lava [core](liblava/core)

[![data](https://img.shields.io/badge/lava-data-blue.svg)](liblava/core/data.hpp) [![id](https://img.shields.io/badge/lava-id-blue.svg)](liblava/core/id.hpp) [![math](https://img.shields.io/badge/lava-math-blue.svg)](liblava/core/math.hpp) [![time](https://img.shields.io/badge/lava-time-blue.svg)](liblava/core/time.hpp) [![types](https://img.shields.io/badge/lava-types-blue.svg)](liblava/core/types.hpp) [![version](https://img.shields.io/badge/lava-version-blue.svg)](liblava/core/version.hpp)

<br />

## Third-Party

* [argh](https://github.com/adishavit/argh) &nbsp; **Argh! A minimalist argument handler** &nbsp; *3-clause BSD*

  [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.hpp)

* [Catch2](https://github.com/catchorg/Catch2) &nbsp; **A modern, C++-native, header-only, test framework for unit-tests, TDD and BDD** &nbsp; *BSL 1.0*

* [glfw](https://github.com/glfw/glfw) &nbsp; **A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input** &nbsp; *zlib*

  [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame/frame.cpp) [![input](https://img.shields.io/badge/lava-input-red.svg)](liblava/frame/input.cpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.cpp)

* [gli](https://github.com/g-truc/gli) &nbsp; **OpenGL Image (GLI)** &nbsp; *MIT*

  [![texture_loader](https://img.shields.io/badge/lava-texture_loader-orange.svg)](liblava/asset/texture_loader.cpp)

* [glm](https://github.com/g-truc/glm) &nbsp; **OpenGL Mathematics (GLM)** &nbsp; *MIT*

  [![math](https://img.shields.io/badge/lava-math-blue.svg)](liblava/core/math.hpp)

* [IconFontCppHeaders](https://github.com/juliettef/IconFontCppHeaders) &nbsp; **C, C++ headers and C# classes for icon fonts** &nbsp; *zlib*

  [![demo](https://img.shields.io/badge/lava-demo-brightgreen.svg)](liblava-demo)

* [imgui](https://github.com/ocornut/imgui) &nbsp; **Dear ImGui - Bloat-free Graphical User interface for C++ with minimal dependencies** &nbsp; *MIT*

  [![imgui](https://img.shields.io/badge/lava-imgui-brightgreen.svg)](liblava/app/imgui.cpp)

* [json](https://github.com/nlohmann/json) &nbsp; **JSON for Modern C++** &nbsp; *MIT*

  [![json_file](https://img.shields.io/badge/lava-json_file-blue.svg)](liblava/file/json_file.hpp)

* [physfs](https://github.com/Didstopia/physfs) &nbsp; **A portable, flexible file i/o abstraction** &nbsp; *zlib*

  [![file](https://img.shields.io/badge/lava-file-blue.svg)](liblava/file/file.cpp) [![file_system](https://img.shields.io/badge/lava-file_system-blue.svg)](liblava/file/file_system.cpp)

* [spdlog](https://github.com/gabime/spdlog) &nbsp; **Fast C++ logging library** &nbsp; *MIT*

  [![log](https://img.shields.io/badge/lava-log-blue.svg)](liblava/util/log.hpp)

* [stb](https://github.com/nothings/stb) &nbsp; **Single-file public domain libraries for C/C++** &nbsp; *MIT*

  [![scope_image](https://img.shields.io/badge/lava-scope_image-orange.svg)](liblava/asset/scope_image.cpp) [![texture_loader](https://img.shields.io/badge/lava-texture_loader-orange.svg)](liblava/asset/texture_loader.cpp)

* [tinyobjloader](https://github.com/syoyo/tinyobjloader) &nbsp; **Tiny but powerful single file wavefront obj loader** &nbsp; *MIT*

  [![mesh_loader](https://img.shields.io/badge/lava-mesh_loader-orange.svg)](liblava/asset/mesh_loader.hpp)

* [volk](https://github.com/zeux/volk) &nbsp; **Meta loader for Vulkan API** &nbsp; *MIT*

  [![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp)

* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) &nbsp; **Vulkan Header files and API registry** &nbsp; *Apache 2.0*

  [![base](https://img.shields.io/badge/lava-base-orange.svg)](liblava/base/base.hpp) [![window](https://img.shields.io/badge/lava-window-red.svg)](liblava/frame/window.hpp)

* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) &nbsp; **Easy to integrate Vulkan memory allocation library** &nbsp; *MIT*

  [![memory](https://img.shields.io/badge/lava-memory-orange.svg)](liblava/base/memory.hpp)

<br />

## Guide

*WIP*

#### Lifetime of an Object

Before you create new objects or use existing ones, you should get familiar with the lifetime of objects

It is basically possible to create all objects in **liblava** on the stack or on the heap

But be careful. You have to take care of the lifetime yourself

##### make ➜ create ➜ destroy

This is the general pattern that is used in this library:

1. **make** - Use constructor or factory method *(static function to get a shared pointer)*
2. **create** - Build the respective object
3. **destroy** - Discard it after your use

The destructor calls the **destroy** method if it was not called before

##### For example: buffer object

```c++
void use_buffer_on_stack() {

    buffer buf; // make

    auto created = buf.create(device, data, size, usage);
    if (created) {
        // ...

        buf.destroy();
    }
}
```

Or look at this method where it is returned as a shared pointer:

```c++
buffer::ptr use_buffer_on_heap() {

    auto buf = make_buffer();

    if (buf->create(device, data, size, usage))
        return buf;

    return nullptr;
}
```

<br />

## Tests

Run the **lava** executable to test our [Tutorial examples](tests/tests.cpp)

Let it simply flow...

##### List all tests:

```bash
lava -t
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

```bash
lava 2
```

The **driver** starts the *last* test when you provide *no* command line arguments

<br />

### Unit testing

In addition run **lava-unit** that will check parts of **liblava** using [Catch2](https://github.com/catchorg/Catch2) test framework

<br />

## Demo

The demonstration projects are in the [liblava-demo](liblava-demo) folder. Screenshots and a brief description in [README](README.md/#demos)

#### Roboto

*Apache License, Version 2.0* &nbsp; [GitHub](https://github.com/google/fonts/tree/master/apache/roboto) &nbsp; [Website](https://fonts.google.com/specimen/Roboto)

* [Roboto-Regular.ttf](res/font/imgui/Roboto-Regular.ttf)

#### Font Awesome

*Font Awesome Free License* &nbsp; [GitHub](https://github.com/FortAwesome/Font-Awesome) &nbsp; [Website](https://fontawesome.com)

* [fa-solid-900.ttf](res/font/icon/fa-solid-900.ttf)

#### Barbarella

*Shader by Weyland Yutani* &nbsp; [Website](https://www.shadertoy.com/view/XdfGDr)

* [lamp.frag](res/lamp/lamp.frag)

#### Spawn Model

*CC BY-SA 3.0* &nbsp; [Website](https://opengameart.org/content/lava-spawn)

* [lava-spawn-game.mtl](res/spawn/lava-spawn-game.mtl)
* [lava-spawn-game.obj](res/spawn/lava-spawn-game.obj)

<br />

## Template

You can start coding with the **template** project. If you like you can rename it in [CMakeLists](CMakeLists.txt)

➜ Just put your code in the [src](src) folder. Everything you need is in [main.cpp](src/main.cpp)

<br />

## Build

```bash
git clone https://github.com/liblava/liblava.git
cd liblava

git submodule update --init --recursive

mkdir build
cd build

cmake ..
make
```

<br />

## Install

You can use **liblava** as a *git submodule* in your project

```bash
git submodule add https://github.com/liblava/liblava.git

git submodule update --init --recursive
```

#### Conan Package Manager

If you are familiar with [Conan](https://conan.io/) then you can build our [package recipe](https://github.com/liblava/conan-liblava)

<br />

Alternatively - You can compile and install a specific version for multiple projects: 

```bash
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=../lava-install ..
cmake --build . --target install
```

#### Project setup

First find the package in your *CMakeLists.txt*

```cmake
find_package(lava 0.6.0 REQUIRED)

...

add_executable(test main.cpp)
target_link_libraries(test lava::app)
```

<br />

And then build your project with install path ➜ *lava_DIR*

```bash
mkdir build
cd build

cmake -D lava_DIR=path/to/lava-install/lib/cmake/lava ..
cmake --build .
```

<br />

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
