<a href="https://git.io/liblava"><img align="left" src="https://github.com/liblava.png" width="100" style="margin:0px 20px 0px 0px"></a>

# liblava docs &nbsp; [![Version](https://img.shields.io/badge/Version-0.7.1-blue)](https://git.io/liblava)

[Home](README.md) &nbsp; [Features](Features.md) &nbsp; [Tutorial](Tutorial.md) &nbsp; [Guide](Guide.md) &nbsp; [Reference](Reference.md) &nbsp; [Modules](Modules.md) &nbsp; [Third-Party](Third-Party.md) &nbsp; [Demo](Demo.md) &nbsp; [Tests](Tests.md) &nbsp; **[Install](Install.md)**

<br />

# Install

You can use **liblava** as a *git submodule* in your project

```bash
git submodule add https://github.com/liblava/liblava.git

git submodule update --init --recursive
```

Add this to your *CMakeLists.txt*

```cmake
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/liblava ${CMAKE_CURRENT_BINARY_DIR}/liblava)
target_link_libraries(${PROJECT_NAME} PRIVATE lava::app)
```

<br />

## Package setup

Alternatively ➜ compile and install a specific version for multiple projects:

```bash
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=../lava-install ..
cmake --build . --config Release --target install
```

First find the package in your *CMakeLists.txt*

```cmake
find_package(lava 0.7.1 REQUIRED)

...

add_executable(test main.cpp)
target_link_libraries(test lava::app)
```

And then build your project with install path ➜ *lava_DIR*

```bash
mkdir build
cd build

cmake -D lava_DIR=path/to/lava-install/lib/cmake/lava ..
cmake --build .
```

<br />

## Installing and using Vcpkg

[Vcpkg](https://github.com/microsoft/vcpkg) integration with 2 options ➜ use this [registry and port](https://github.com/liblava/vcpkg-liblava)

<br />

## Conan Package Manager

If you are familiar with [Conan](https://conan.io/) ➜ build this [package recipe](https://github.com/liblava/conan-liblava)

<br />

<a href="https://git.io/liblava"><img src="https://github.com/liblava.png" width="50"></a>
