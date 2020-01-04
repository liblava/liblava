# file      : CMakeLists.txt
# copyright : Copyright (c) 2018-present, Lava Block OÜ
# license   : MIT; see accompanying LICENSE file

cmake_minimum_required(VERSION 3.12)

project(liblava VERSION 0.4.5 LANGUAGES C CXX)

message("")
message("========================================================================")
message(" Copyright (c) 2018-present, Lava Block OÜ                 MIT licensed ")
message("========================================================================")
message("                                                                        ")
message("  _|  _|  _|            _|                                              ")
message("  _|      _|_|_|        _|        _|_|_|      _|      _|        _|_|_|  ")
message("  _|  _|  _|    _|      _|      _|    _|      _|      _|      _|    _|  ")
message("  _|  _|  _|    _|      _|      _|    _|        _|  _|        _|    _|  ")
message("  _|  _|  _|_|_|        _|        _|_|_|          _|            _|_|_|  ")
message("                                                                        ")
message("========================================================================")
message(" 2020 preview                                                    v0.4.5 ")
message("========================================================================")
message(" https://git.io/liblava                                  lava-block.com ")
message("========================================================================")

message("> module")

if(CMAKE_COMPILER_IS_GNUCXX)
        set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)
endif()

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

set(LIBLAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava)
set(LIBLAVA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(LIBLAVA_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

message(">> lava::core")

if(CMAKE_COMPILER_IS_GNUCXX)
        find_package (Threads)
endif()

add_library(lava.core INTERFACE)

target_include_directories(lava.core INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBLAVA_EXT_DIR}/glm
        )

target_sources(lava.core INTERFACE
        ${LIBLAVA_DIR}/core/data.hpp
        ${LIBLAVA_DIR}/core/id.hpp
        ${LIBLAVA_DIR}/core/math.hpp
        ${LIBLAVA_DIR}/core/time.hpp
        ${LIBLAVA_DIR}/core/types.hpp
        ${LIBLAVA_DIR}/core/version.hpp
        )

target_compile_features(lava.core INTERFACE 
        cxx_std_20
        )

if(CMAKE_COMPILER_IS_GNUCXX)
        target_link_libraries(lava.core INTERFACE 
                stdc++fs 
                ${CMAKE_THREAD_LIBS_INIT}
                )
endif()

add_library(lava::core ALIAS lava.core)

message(">> lava::util")

add_library(lava.util STATIC
        ${LIBLAVA_DIR}/util/file.cpp
        ${LIBLAVA_DIR}/util/file.hpp
        ${LIBLAVA_DIR}/util/log.hpp
        ${LIBLAVA_DIR}/util/random.hpp
        ${LIBLAVA_DIR}/util/telegram.hpp
        ${LIBLAVA_DIR}/util/thread.hpp
        ${LIBLAVA_DIR}/util/utility.hpp
        )

target_include_directories(lava.util PUBLIC
        ${LIBLAVA_EXT_DIR}/spdlog/include
        ${LIBLAVA_EXT_DIR}/physfs/src
        ${LIBLAVA_EXT_DIR}/json/single_include
        )

message(">>> physfs")

set(PHYSFS_ARCHIVE_ZIP ON CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_7Z OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_GRP OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_WAD OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_HOG OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_MVL OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_QPAK OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_SLB OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_ISO9660 OFF CACHE BOOL "" FORCE)
set(PHYSFS_ARCHIVE_VDF OFF CACHE BOOL "" FORCE)

set(PHYSFS_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(PHYSFS_BUILD_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBLAVA_EXT_DIR}/physfs physfs EXCLUDE_FROM_ALL)

message("<<< physfs")

target_link_libraries(lava.util
        lava::core
        physfs-static
        )

add_library(lava::util ALIAS lava.util)

message(">> lava::base")

add_library(lava.base STATIC
        ${LIBLAVA_DIR}/base/base.cpp
        ${LIBLAVA_DIR}/base/base.hpp
        ${LIBLAVA_DIR}/base/device_table.hpp
        ${LIBLAVA_DIR}/base/device.cpp
        ${LIBLAVA_DIR}/base/device.hpp
        ${LIBLAVA_DIR}/base/instance.cpp
        ${LIBLAVA_DIR}/base/instance.hpp
        ${LIBLAVA_DIR}/base/memory.cpp
        ${LIBLAVA_DIR}/base/memory.hpp
        ${LIBLAVA_DIR}/base/physical_device.cpp
        ${LIBLAVA_DIR}/base/physical_device.hpp
        ${LIBLAVA_EXT_DIR}/volk/volk.c
        )

target_include_directories(lava.base PUBLIC
        ${LIBLAVA_EXT_DIR}/Vulkan-Headers/include
        ${LIBLAVA_EXT_DIR}/VulkanMemoryAllocator/src
        ${LIBLAVA_EXT_DIR}/volk
        )

target_link_libraries(lava.base 
        lava::util
        ${CMAKE_DL_LIBS}
        )

add_library(lava::base ALIAS lava.base)

message(">> lava::resource")

add_library(lava.resource STATIC
        ${LIBLAVA_DIR}/resource/buffer.cpp
        ${LIBLAVA_DIR}/resource/buffer.hpp
        ${LIBLAVA_DIR}/resource/format.cpp
        ${LIBLAVA_DIR}/resource/format.hpp
        ${LIBLAVA_DIR}/resource/image.cpp
        ${LIBLAVA_DIR}/resource/image.hpp
        ${LIBLAVA_DIR}/resource/mesh.cpp
        ${LIBLAVA_DIR}/resource/mesh.hpp
        ${LIBLAVA_DIR}/resource/texture.cpp
        ${LIBLAVA_DIR}/resource/texture.hpp
        )

target_include_directories(lava.resource PUBLIC
        ${LIBLAVA_EXT_DIR}/stb
        ${LIBLAVA_EXT_DIR}/gli
        ${LIBLAVA_EXT_DIR}/tinyobjloader
        ${LIBLAVA_EXT_DIR}/bitmap
        ${LIBLAVA_EXT_DIR}/selene
        )

message(">>> selene")

add_subdirectory(${LIBLAVA_EXT_DIR}/selene selene EXCLUDE_FROM_ALL)

message("<<< selene")

target_link_libraries(lava.resource
        lava::base
        )

add_library(lava::resource ALIAS lava.resource)

message(">> lava::frame")

add_library(lava.frame STATIC
        ${LIBLAVA_DIR}/frame/frame.cpp
        ${LIBLAVA_DIR}/frame/frame.hpp
        ${LIBLAVA_DIR}/frame/input.cpp
        ${LIBLAVA_DIR}/frame/input.hpp
        ${LIBLAVA_DIR}/frame/render_target.cpp
        ${LIBLAVA_DIR}/frame/render_target.hpp
        ${LIBLAVA_DIR}/frame/render_thread.hpp
        ${LIBLAVA_DIR}/frame/renderer.cpp
        ${LIBLAVA_DIR}/frame/renderer.hpp
        ${LIBLAVA_DIR}/frame/swapchain.cpp
        ${LIBLAVA_DIR}/frame/swapchain.hpp
        ${LIBLAVA_DIR}/frame/window.cpp
        ${LIBLAVA_DIR}/frame/window.hpp
        )

target_include_directories(lava.frame PUBLIC
        ${LIBLAVA_EXT_DIR}/glfw/include
        ${LIBLAVA_EXT_DIR}/argh
        )

message(">>> glfw")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBLAVA_EXT_DIR}/glfw glfw EXCLUDE_FROM_ALL)

message("<<< glfw")

target_link_libraries(lava.frame
        lava::resource
        glfw
        ${GLFW_LIBRARIES}
        )

add_library(lava::frame ALIAS lava.frame)

message(">> lava::block")

add_library(lava.block STATIC
        ${LIBLAVA_DIR}/block/attachment.hpp
        ${LIBLAVA_DIR}/block/block.cpp
        ${LIBLAVA_DIR}/block/block.hpp
        ${LIBLAVA_DIR}/block/descriptor.cpp
        ${LIBLAVA_DIR}/block/descriptor.hpp
        ${LIBLAVA_DIR}/block/pipeline.cpp
        ${LIBLAVA_DIR}/block/pipeline.hpp
        ${LIBLAVA_DIR}/block/render_pass.cpp
        ${LIBLAVA_DIR}/block/render_pass.hpp
        ${LIBLAVA_DIR}/block/subpass.cpp
        ${LIBLAVA_DIR}/block/subpass.hpp
        )

target_link_libraries(lava.block 
        lava::base
        )

add_library(lava::block ALIAS lava.block)

message(">> lava::app")

add_library(lava.app STATIC
        ${LIBLAVA_DIR}/app/app.cpp
        ${LIBLAVA_DIR}/app/app.hpp
        ${LIBLAVA_DIR}/app/camera.cpp
        ${LIBLAVA_DIR}/app/camera.hpp
        ${LIBLAVA_DIR}/app/def.hpp
        ${LIBLAVA_DIR}/app/forward_shading.cpp
        ${LIBLAVA_DIR}/app/forward_shading.hpp
        ${LIBLAVA_DIR}/app/gui.cpp
        ${LIBLAVA_DIR}/app/gui.hpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_draw.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_widgets.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_demo.cpp
        )

target_include_directories(lava.app PUBLIC
        ${LIBLAVA_EXT_DIR}/imgui
        )

target_link_libraries(lava.app 
        lava::frame
        lava::block
        )

add_library(lava::app ALIAS lava.app)

option(LAVA_TESTS "Enable Tests" TRUE)
if(LAVA_TESTS)
        message("> tests")

        add_executable(lava 
                ${LIBLAVA_TESTS_DIR}/driver.cpp
                ${LIBLAVA_TESTS_DIR}/driver.hpp
                ${LIBLAVA_TESTS_DIR}/tests.cpp
                )

        target_link_libraries(lava lava::app)
endif()

message("========================================================================")
