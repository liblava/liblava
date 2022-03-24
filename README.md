<a href="https://git.io/liblava"><img align="right" src="res/doc/liblava_logo_200px.png" width="200" style="margin:0px 40px 0px 0px"></a>

# liblava &nbsp; [![Version](https://img.shields.io/badge/2022-preview-blue)](#demos)

**A modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/" target="_blank">Vulkan</a>® API**

**lava** &nbsp; is a lean framework that provides &nbsp; **essentials** &nbsp; for &nbsp; **low-level graphics** <br />Specially well suited for &nbsp; **prototyping** &nbsp; **tooling** &nbsp; **profiling** &nbsp; and &nbsp; **education**

<br />

[![Version](https://img.shields.io/badge/Version-0.7.1-blue)](https://git.io/liblava) &nbsp; [![License](https://img.shields.io/github/license/liblava/liblava)](LICENSE) &nbsp; [![CodeFactor](https://www.codefactor.io/repository/github/liblava/liblava/badge)](https://www.codefactor.io/repository/github/liblava/liblava) &nbsp; [![Discord](https://img.shields.io/discord/439508141722435595)](https://discord.lava-block.com) &nbsp; [![Donate](https://img.shields.io/badge/donate-PayPal-lightgrey.svg)](https://paypal.me/liblava) &nbsp; [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/liblava)

&nbsp; ➜ &nbsp; &nbsp; **C++20** &nbsp; &nbsp; &nbsp; **Modular** &nbsp; &nbsp; &nbsp; **Linux** &nbsp; &nbsp; &nbsp; **Windows**

<br />

[Requirements](#requirements) &nbsp; **[Download](https://github.com/liblava/liblava/releases)** &nbsp; [Build](#build) &nbsp; [Template](#template) 

<br />

### [Features](doc/Features.md)

<a href="https://www.khronos.org/vulkan/" target="_blank"><img align="right" hspace="20" src="res/doc/Vulkan_170px_Dec16.png" width="300"></a>

* written in &nbsp; **modern C++** &nbsp; with latest &nbsp; **Vulkan support**
* **run loop** &nbsp; abstraction for &nbsp; **window** &nbsp; and &nbsp; **input** &nbsp; handling
* plain &nbsp; **renderer** &nbsp; and &nbsp; **command buffer** &nbsp; model
* **texture** &nbsp; and &nbsp; **mesh** &nbsp; loading from &nbsp; **virtual file system**
* **camera** &nbsp; **imgui** &nbsp; **logger** &nbsp; and &nbsp; more...

<br />

## Docs

 **[Tutorial](doc/Tutorial.md)** &nbsp; [Guide](doc/Guide.md) &nbsp; [Reference](doc/Reference.md) &nbsp; [Tests](doc/Tests.md) &nbsp; [Third-Party](doc/Third-Party.md) &nbsp; [Install](doc/Install.md)

<br />

### [Modules](doc/Modules.md)

[![core](https://img.shields.io/badge/lava-core-blue.svg)](liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](liblava/util) [![file](https://img.shields.io/badge/lava-file-blue.svg)](liblava/file) &nbsp; [![base](https://img.shields.io/badge/lava-base-yellowgreen.svg)](liblava/base) [![resource](https://img.shields.io/badge/lava-resource-yellowgreen.svg)](liblava/resource) &nbsp; [![asset](https://img.shields.io/badge/lava-asset-red.svg)](liblava/asset) [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](liblava/block) &nbsp; [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](liblava/app) [![demo](https://img.shields.io/badge/lava-demo-brightgreen.svg)](liblava-demo)

<br />

## Demos

### [lava light](liblava-demo/light.cpp)
deferred shading &nbsp; + &nbsp; offscreen rendering

<a href="liblava-demo/light.cpp">![light](res/light/screenshot.png)</a>

<br />

### [lava spawn](liblava-demo/spawn.cpp)
uniform buffer &nbsp; + &nbsp; camera

<a href="liblava-demo/spawn.cpp">![spawn](res/spawn/screenshot.png)</a>

<br />

### [lava lamp](liblava-demo/lamp.cpp)
push constants &nbsp; ➜ &nbsp; shader

<a href="liblava-demo/lamp.cpp">![lamp](res/lamp/screenshot.png)</a>

<br />

### [lava shapes](liblava-demo/shapes.cpp)
generating &nbsp; primitives

<a href="liblava-demo/shapes.cpp">![shapes](res/shapes/screenshot.png)</a>

<br />

### [lava generic triangle](liblava-demo/generic_triangle.cpp)
float &nbsp; double &nbsp; & &nbsp; int &nbsp; meshes

<a href="liblava-demo/generic_triangle.cpp">![generic triangle](res/generic_triangle/screenshot.png)</a>

<br />

### [lava triangle](liblava-demo/triangle.cpp)
unique &nbsp; classic &nbsp; mesh

<a href="liblava-demo/triangle.cpp">![triangle](res/triangle/screenshot.png)</a>

<br />

## Projects

### [lava raytracing cubes](https://github.com/pezcode/lava-rt/blob/main/demo/cubes.cpp)
raytraced &nbsp; reflecting &nbsp; cubes &nbsp; ➜ &nbsp; [pezcode/lava-rt](https://github.com/pezcode/lava-rt)

<a href="https://github.com/pezcode/lava-rt/blob/main/demo/cubes.cpp">![cubes](https://raw.githubusercontent.com/pezcode/lava-rt/main/demo/res/cubes/screenshot.png)</a>

<br />

## Requirements

* **C++20** compatible compiler
* CMake **3.22+**
* [Vulkan SDK](https://vulkan.lunarg.com)

<br />

## Build

[![CMake (Linux, Windows)](https://github.com/liblava/liblava/actions/workflows/cmake.yml/badge.svg)](https://github.com/liblava/liblava/actions/workflows/cmake.yml)

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

## Template

You can start with the &nbsp; **template** &nbsp; project

Put your code in &nbsp; [src/](src) &nbsp; and begin to code in &nbsp; [main.cpp](src/main.cpp)

<br />

**NOTE** &nbsp; You can change the project name in &nbsp; `CMake` &nbsp; ➜ &nbsp; `LIBLAVA_TEMPLATE_NAME` 

```bash
cmake -DLIBLAVA_TEMPLATE_NAME="My-Project" ..
```

<br />

## Collaborate

Use the [issue tracker](https://github.com/liblava/liblava/issues) to report any bug or compatibility issue

:heart: &nbsp; Thanks to all &nbsp; **[contributors](https://github.com/liblava/liblava/graphs/contributors)** &nbsp; making &nbsp; **liblava** &nbsp; flow...

<br />

### Support

If you want to contribute - we suggest the following:

1. Fork the [official repository](https://github.com/liblava/liblava/fork)
2. Apply your changes to your fork
3. Submit a [pull request](https://github.com/liblava/liblava/pulls) describing the changes you have made

<br />

## License

<a href="https://opensource.org" target="_blank"><img align="right" width="90" src="res/doc/OSI-Approved-License-100x137.png" style="margin:0px 0px 0px 80px"></a>

**liblava** &nbsp; is licensed under [MIT License](LICENSE.md) which allows you to use the software <br />for any purpose you might like (including commercial and for-profit use!)

<br />

However - this library includes several **[Third-Party](doc/Third-Party.md)** libraries which are licensed under their own respective **Open Source** licenses &nbsp; ➜ &nbsp; They all allow static linking with closed source software

<br />

**All copies of liblava must include a copy of the MIT License terms and the copyright notice**

##### Vulkan and the Vulkan logo are trademarks of the <a href="http://www.khronos.org" target="_blank">Khronos Group Inc.</a>
##### Copyright (c) 2018-present - <a href="https://lava-block.com">Lava Block OÜ</a> and [contributors](https://github.com/liblava/liblava/graphs/contributors)

<br />

<a href="https://git.io/liblava"><img src="res/doc/liblava_logo_200px.png" width="50"></a>
