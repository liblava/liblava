<a href="https://git.io/liblava"><img align="right" src="https://github.com/liblava.png" width="200" style="margin:0px 40px 0px 0px"></a>

# liblava &nbsp; [![Version](https://img.shields.io/badge/2021-alpha-blue)](#demos)

**A modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/" target="_blank">Vulkan</a>® API**

**lava** is a lean framework that provides **essentials** for **low-level graphics** <br />Specially well suited for **prototyping**, **tooling** and **education**

<br />

[![Version](https://img.shields.io/badge/Version-0.6.3-blue)](https://git.io/liblava) &nbsp; [![License](https://img.shields.io/github/license/liblava/liblava)](LICENSE) &nbsp; [![CodeFactor](https://www.codefactor.io/repository/github/liblava/liblava/badge)](https://www.codefactor.io/repository/github/liblava/liblava) &nbsp; [![Discord](https://img.shields.io/discord/439508141722435595)](https://discord.lava-block.com) &nbsp; [![Donate](https://img.shields.io/badge/donate-PayPal-lightgrey.svg)](https://paypal.me/liblava) &nbsp; [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/liblava)

&nbsp; ➜ &nbsp; **C++20** &nbsp; / &nbsp; **Modular** &nbsp; / &nbsp; **Linux** &nbsp; / &nbsp; **Windows**

<br />

[Requirements](#requirements) &nbsp; **[Download](https://github.com/liblava/liblava/releases)** &nbsp; [Build](#build) &nbsp; [Template](#template) 

<br />

### [Features](doc/Features.md)

<a href="https://www.khronos.org/vulkan/" target="_blank"><img align="right" hspace="20" src="res/Vulkan_170px_Dec16.png" width="300"></a>

* written in **modern C++** with latest **Vulkan** support
* **run loop** abstraction for **window** and **input** handling
* plain **renderer** and **command buffer** model
* **texture** and **mesh** loading from virtual **file system**
* **GUI** + **camera** + **logging** + **utils** and much more

<br />

## Docs

 **[Tutorial](doc/Tutorial.md)** &nbsp; [Guide](doc/Guide.md) &nbsp; [Reference](doc/Reference.md) &nbsp; [Tests](doc/Tests.md) &nbsp; [Third-Party](doc/Third-Party.md) &nbsp; [Install](doc/Install.md)

<br />

### [Modules](doc/Modules.md)

[![core](https://img.shields.io/badge/lava-core-blue.svg)](liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](liblava/util) [![file](https://img.shields.io/badge/lava-file-blue.svg)](liblava/file) &nbsp; [![base](https://img.shields.io/badge/lava-base-yellowgreen.svg)](liblava/base) [![resource](https://img.shields.io/badge/lava-resource-yellowgreen.svg)](liblava/resource) [![asset](https://img.shields.io/badge/lava-asset-yellowgreen.svg)](liblava/asset) &nbsp; [![frame](https://img.shields.io/badge/lava-frame-red.svg)](liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](liblava/block) &nbsp; [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](liblava/app) [![demo](https://img.shields.io/badge/lava-demo-brightgreen.svg)](liblava-demo)

<br />

## Demos

### [lava light](liblava-demo/light.cpp)
deferred shading / offscreen rendering

<a href="liblava-demo/light.cpp">![light](res/light/screenshot.png)</a>

<br />

### [lava spawn](liblava-demo/spawn.cpp)
uniform buffer camera

<a href="liblava-demo/spawn.cpp">![spawn](res/spawn/screenshot.png)</a>

<br />

### [lava lamp](liblava-demo/lamp.cpp)
push constants to shader

<a href="liblava-demo/lamp.cpp">![lamp](res/lamp/screenshot.png)</a>

<br />

### [lava shapes](liblava-demo/shapes.cpp)
generating primitives

<br />

### [lava generic triangles](liblava-demo/generic_triangle.cpp)
float, double, and int meshes

<br />

### [lava triangle](liblava-demo/triangle.cpp)
classic colored mesh

<a href="liblava-demo/triangle.cpp">![triangle](res/triangle/screenshot.png)</a>

<br />

## Projects

### [lava raytracing cubes](https://github.com/pezcode/lava-rt/blob/main/demo/cubes.cpp)
raytraced reflecting cubes &nbsp; ➜ &nbsp; [pezcode/lava-rt](https://github.com/pezcode/lava-rt)

<a href="https://github.com/pezcode/lava-rt/blob/main/demo/cubes.cpp">![cubes](https://raw.githubusercontent.com/pezcode/lava-rt/main/demo/res/cubes/screenshot.png)</a>

<br />

## Requirements

* **C++20** compatible compiler
* CMake **3.20+**
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

You can start coding with the **template** project. If you like you can rename it in [CMakeLists](CMakeLists.txt)

&nbsp; ➜ &nbsp; Just put your code in the [src](src) folder. Everything you need is in [main.cpp](src/main.cpp)

<br />

## Collaborate

Use the [issue tracker](https://github.com/liblava/liblava/issues) to report any bug or compatibility issue

:heart: Thanks to all **[contributors](https://github.com/liblava/liblava/graphs/contributors)** making **liblava** flow...

<br />

### Support

If you want to contribute, we suggest the following:

1. Fork the [official repository](https://github.com/liblava/liblava/fork)
2. Apply your changes to your fork
3. Submit a [pull request](https://github.com/liblava/liblava/pulls) describing the changes you have made

<br />

## License

<a href="https://opensource.org" target="_blank"><img align="right" width="90" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png" style="margin:0px 0px 0px 80px"></a>

**liblava** is licensed under [MIT License](LICENSE.md) which allows you to use the software <br />for any purpose you might like, including commercial and for-profit use!

<br />

However, this library includes several [Third-Party](doc/Third-Party.md) libraries, which are licensed under their own respective **Open Source** licenses &nbsp; ➜ &nbsp; They all allow static linking with closed source software

**All copies of liblava must include a copy of the MIT License terms and the copyright notice**

##### Vulkan and the Vulkan logo are trademarks of the <a href="http://www.khronos.org" target="_blank">Khronos Group Inc.</a>
##### Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a> and [contributors](https://github.com/liblava/liblava/graphs/contributors)

<br />

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
