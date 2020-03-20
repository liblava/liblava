<a href="https://lava-block.com"><img align="left" src="https://github.com/liblava.png" width="240"></a>

**liblava is a modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/">Vulkan</a>® API**

**lava** is a lean framework that provides **essentials** for **low-level graphics**<br /> and is specially well suited for **prototyping**, **tooling** and **education**

**C++20** • **Modular** • **Windows** • **Linux** • <a href="https://git.io/liblava-demo">demo</a>  • <a href="https://git.io/liblava-template">template</a>

[![Version](https://img.shields.io/badge/Version-0.5.1-blue)](https://git.io/liblava) [![License](https://img.shields.io/github/license/liblava/liblava)](LICENSE) [![Discord](https://img.shields.io/discord/439508141722435595)](https://discord.lava-block.com) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/liblava) [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/liblava)

<br />

#### Features

* written in **modern C++** with latest **Vulkan** support
* **[run loop](DOCS.md/#run-loop)** **abstraction** for **[window](DOCS.md/#window)** and **[input](DOCS.md/#input) handling**
* **plain** **[renderer](DOCS.md/#renderer)** and **[command buffer model](DOCS.md/#command-buffer-model)**
* **[texture](DOCS.md/#texture)** and **[mesh](DOCS.md/#mesh)** **loading** from **virtual [file system](DOCS.md/#file-system)**
* **[camera](DOCS.md/#camera)**, **[gui](DOCS.md/#gui)**, **[logging](DOCS.md/#logging)**, **test driver** and much more...

#### Docs

**[Features](DOCS.md/#features)** • **[Tutorial](DOCS.md/#tutorial)** • **[Modules](DOCS.md/#modules)** • **[Guide](DOCS.md/#guide)** • **[Tests](DOCS.md/#tests)** • **[Build](DOCS.md/#build)** • **[Install](DOCS.md/#install)**

##### Modules

[![core](https://img.shields.io/badge/lava-core-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/util) [![base](https://img.shields.io/badge/lava-base-yellow.svg)](https://github.com/liblava/liblava/tree/master/liblava/base) [![resource](https://img.shields.io/badge/lava-resource-yellow.svg)](https://github.com/liblava/liblava/tree/master/liblava/resource) [![frame](https://img.shields.io/badge/lava-frame-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/block) [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](https://github.com/liblava/liblava/tree/master/liblava/app) [![engine](https://img.shields.io/badge/lava-engine-brightgreen.svg)](https://git.io/liblava-engine)

##### Download latest **<a href="https://github.com/liblava/liblava-demo/releases">demo</a>** (Feb. 20, 2020)

<a href="https://github.com/liblava/liblava-demo/#readme"><img src="res/demo.png"></a>

[![Build status](https://ci.appveyor.com/api/projects/status/gxvjpo73qf637hy3?svg=true)](https://ci.appveyor.com/project/TheLavaBlock/liblava) [![Build Status](https://travis-ci.com/liblava/liblava.svg?branch=master)](https://travis-ci.com/liblava/liblava)

## Requirements

* **C++20** compatible compiler
* CMake **3.15+**
* [Vulkan SDK](https://vulkan.lunarg.com)

## Third-Party

* [argh](https://github.com/adishavit/argh) • 3-clause BSD
* [bitmap](https://github.com/ArashPartow/bitmap) • MIT
* [glfw](https://github.com/glfw/glfw) • zlib
* [gli](https://github.com/g-truc/gli) • MIT
* [glm](https://github.com/g-truc/glm) • MIT
* [imgui](https://github.com/ocornut/imgui) • MIT
* [json](https://github.com/nlohmann/json) • MIT
* [physfs](https://github.com/criptych/physfs) • zlib
* [selene](https://github.com/kmhofmann/selene) • MIT
* [spdlog](https://github.com/gabime/spdlog) • MIT
* [stb](https://github.com/nothings/stb) • MIT
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) • MIT
* [volk](https://github.com/zeux/volk) • MIT
* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) • Apache 2.0
* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) • MIT

## Collaborate

:heart: Thanks to all **[contributors](https://github.com/liblava/liblava/graphs/contributors)** making **liblava** flow...

You can use the [issue tracker](https://github.com/liblava/liblava/issues) to report any bug or compatibility issue.

### Support

If you want to contribute, we suggest the following:

1. Fork the [official repository](https://github.com/liblava/liblava/fork)
2. Apply your changes to your fork
3. Submit a [pull request](https://github.com/liblava/liblava/pulls) describing the changes you have made

## License

<a href="https://opensource.org"><img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png"></a>

**liblava** is licensed under [MIT License](LICENSE.md) which allows you to use the software for any purpose you might like, including commercial and for-profit use.

However, this library includes several third-party **Open Source** libraries, which are licensed under their own respective licenses. They all allow static linking with closed source software.

All copies of **liblava** must include a copy of the MIT License terms and the copyright notice.

##### Vulkan and the Vulkan logo are trademarks of the [Khronos Group Inc.](http://www.khronos.org)
##### Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a>

<a href="https://lava-block.com"><img src="https://github.com/liblava.png" width="50"></a>
