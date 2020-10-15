<a href="https://lava-block.com"><img align="right" src="https://github.com/liblava.png" width="200" style="margin:0px 40px 0px 0px"></a>

🌋 **[liblava](https://git.io/liblava) &nbsp; A modern and easy-to-use library for the <a href="https://www.khronos.org/vulkan/" target="_blank">Vulkan</a>® API**

**lava** is a lean framework that provides **essentials** for **low-level graphics**<br />and is specially well suited for **prototyping**, **tooling** and **education**

<br />

**C++20** + **Modular** &nbsp; **Windows** + **Linux** &nbsp; **<a href="https://git.io/liblava-demo">demo</a>**  + <a href="https://git.io/liblava-template">template</a>

[![Version](https://img.shields.io/badge/Version-0.5.4-blue)](https://git.io/liblava) [![License](https://img.shields.io/github/license/liblava/liblava)](LICENSE) [![CodeFactor](https://www.codefactor.io/repository/github/liblava/liblava/badge)](https://www.codefactor.io/repository/github/liblava/liblava) [![Discord](https://img.shields.io/discord/439508141722435595)](https://discord.lava-block.com) [![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/liblava) [![Twitter URL](https://img.shields.io/twitter/url/http/shields.io.svg?style=social&label=Follow)](https://twitter.com/liblava)

<br />

#### [Features](DOCS.md/#features)

* written in **modern C++** with latest **Vulkan** support
* **[run loop](DOCS.md/#run-loop)** **abstraction** for **[window](DOCS.md/#window)** and **[input](DOCS.md/#input) handling**
* **plain** **[renderer](DOCS.md/#renderer)** and **[command buffer model](DOCS.md/#command-buffer-model)**
* **[texture](DOCS.md/#texture)** and **[mesh](DOCS.md/#mesh)** **loading** from **virtual [file system](DOCS.md/#file-system)**
* **[camera](DOCS.md/#camera)** + **[gui](DOCS.md/#gui)** + **[logging](DOCS.md/#logging)** + **test driver** and much more

<br />

<a href="https://www.khronos.org/vulkan/" target="_blank"><img align="right" hspace="20" src="res/Vulkan_170px_Dec16.png" width="300"></a>

### Docs

 **[Tutorial](DOCS.md/#tutorial)** &nbsp; **[Guide](DOCS.md/#guide)** &nbsp; [Tests](DOCS.md/#tests) &nbsp; [Build](DOCS.md/#build) &nbsp; [Install](DOCS.md/#install)

<br />

### [Modules](DOCS.md/#modules)

[![core](https://img.shields.io/badge/lava-core-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/core) [![util](https://img.shields.io/badge/lava-util-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/util) [![file](https://img.shields.io/badge/lava-file-blue.svg)](https://github.com/liblava/liblava/tree/master/liblava/file) &nbsp; [![base](https://img.shields.io/badge/lava-base-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/base) [![resource](https://img.shields.io/badge/lava-resource-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/resource) [![asset](https://img.shields.io/badge/lava-asset-orange.svg)](https://github.com/liblava/liblava/tree/master/liblava/asset) &nbsp; [![frame](https://img.shields.io/badge/lava-frame-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/frame) [![block](https://img.shields.io/badge/lava-block-red.svg)](https://github.com/liblava/liblava/tree/master/liblava/block) &nbsp; [![app](https://img.shields.io/badge/lava-app-brightgreen.svg)](https://github.com/liblava/liblava/tree/master/liblava/app) [![engine](https://img.shields.io/badge/lava-engine-brightgreen.svg)](https://git.io/liblava-engine)

<br />

##### ⬇ Download latest **<a href="https://github.com/liblava/liblava-demo/releases">demo</a>** (Feb. 20, 2020)

<a href="https://github.com/liblava/liblava-demo/#readme"><img src="res/demo.png"></a>

<br />

[![Build status](https://ci.appveyor.com/api/projects/status/gxvjpo73qf637hy3?svg=true)](https://ci.appveyor.com/project/TheLavaBlock/liblava) [![Build Status](https://travis-ci.com/liblava/liblava.svg?branch=master)](https://travis-ci.com/liblava/liblava)

## Requirements

* **C++20** compatible compiler
* CMake **3.15+**
* [Vulkan SDK](https://vulkan.lunarg.com)

<br />

## Third-Party

* [argh](https://github.com/adishavit/argh) &nbsp; *3-clause BSD*
* [bitmap](https://github.com/ArashPartow/bitmap) &nbsp; *MIT*
* [glfw](https://github.com/glfw/glfw) &nbsp; *zlib*
* [gli](https://github.com/g-truc/gli) &nbsp; *MIT*
* [glm](https://github.com/g-truc/glm) &nbsp; *MIT*
* [imgui](https://github.com/ocornut/imgui) &nbsp; *MIT*
* [json](https://github.com/nlohmann/json) &nbsp; *MIT*
* [physfs](https://github.com/Didstopia/physfs) &nbsp; *zlib*
* [selene](https://github.com/kmhofmann/selene) &nbsp; *MIT*
* [spdlog](https://github.com/gabime/spdlog) &nbsp; *MIT*
* [stb](https://github.com/nothings/stb) &nbsp; *MIT*
* [tinyobjloader](https://github.com/syoyo/tinyobjloader) &nbsp; *MIT*
* [volk](https://github.com/zeux/volk) &nbsp; *MIT*
* [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers) &nbsp; *Apache 2.0*
* [VulkanMemoryAllocator](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator) &nbsp; *MIT*

<br />

## Collaborate

You can use the [issue tracker](https://github.com/liblava/liblava/issues) to report any bug or compatibility issue

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

**liblava** is licensed under [MIT License](LICENSE.md) which allows you to use the software for any purpose you might like, including commercial and for-profit use!

<br />

However, this library includes several third-party **Open Source** libraries, which are licensed under their own respective licenses ➜ They all allow static linking with closed source software

**All copies of liblava must include a copy of the MIT License terms and the copyright notice**

##### Vulkan and the Vulkan logo are trademarks of the <a href="http://www.khronos.org" target="_blank">Khronos Group Inc.</a>
##### Copyright (c) 2018-present, <a href="https://lava-block.com">Lava Block OÜ</a>

<br />

<a href="https://lava-block.com"><img src="https://github.com/liblava.png" width="50"></a>
