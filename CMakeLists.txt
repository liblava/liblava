# file      : CMakeLists.txt
# copyright : Copyright (c) 2018-present, Lava Block OÜ
# license   : MIT; see accompanying LICENSE file

cmake_minimum_required(VERSION 3.12)

project(liblava VERSION 0.4.3 LANGUAGES C CXX)

message("")
message("========================================================================")
message(" copyright (c) 2018-present, Lava Block OÜ                 MIT licensed ")
message("========================================================================")
message("                                                                        ")
message("  _|  _|  _|            _|                                              ")
message("  _|      _|_|_|        _|        _|_|_|      _|      _|        _|_|_|  ")
message("  _|  _|  _|    _|      _|      _|    _|      _|      _|      _|    _|  ")
message("  _|  _|  _|    _|      _|      _|    _|        _|  _|        _|    _|  ")
message("  _|  _|  _|_|_|        _|        _|_|_|          _|            _|_|_|  ")
message("                                                                        ")
message("========================================================================")
message(" 2019 preview 3                                                  v0.4.3 ")
message("========================================================================")
message(" https://git.io/liblava                                  lava-block.com ")
message("========================================================================")

if(CMAKE_COMPILER_IS_GNUCXX)
        set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS ON)
endif()

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

set(LIBLAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava)
set(LIBLAVA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(LIBLAVA_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# add_subdirectory(ext)

message(">> liblava/core")

if(CMAKE_COMPILER_IS_GNUCXX)
        find_package (Threads)
endif()

add_library(lava.core INTERFACE)

target_include_directories(lava.core INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${LIBLAVA_EXT_DIR}/better-enums
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

message(">> liblava/utils")

add_library(lava.utils STATIC
        ${LIBLAVA_DIR}/utils/file.cpp
        ${LIBLAVA_DIR}/utils/file.hpp
        ${LIBLAVA_DIR}/utils/log.hpp
        ${LIBLAVA_DIR}/utils/random.hpp
        ${LIBLAVA_DIR}/utils/telegram.hpp
        ${LIBLAVA_DIR}/utils/thread.hpp
        ${LIBLAVA_DIR}/utils/utility.hpp
        ${LIBLAVA_EXT_DIR}/tinyfiledialogs/tinyfiledialogs.c
        )

if(WIN32)
        set_source_files_properties(${LIBLAVA_EXT_DIR}/tinyfiledialogs/tinyfiledialogs.c PROPERTIES COMPILE_FLAGS " /W0 ")
endif()

target_include_directories(lava.utils PUBLIC
        ${LIBLAVA_EXT_DIR}/spdlog/include
        ${LIBLAVA_EXT_DIR}/physfs/src
        ${LIBLAVA_EXT_DIR}/tinyfiledialogs
        ${LIBLAVA_EXT_DIR}/json/single_include
        )

message(">>> ext/physfs")

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

message("<<< ext/physfs")

target_link_libraries(lava.utils
        lava.core
        physfs-static
        )

message(">> liblava/base")

add_library(lava.base STATIC
        ${LIBLAVA_DIR}/base/base.cpp
        ${LIBLAVA_DIR}/base/base.hpp
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
        lava.utils 
        ${CMAKE_DL_LIBS}
        )

message(">> liblava/resource")

option(LIBLAVA_ASSIMP "build assimp library" ON)

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

if(LIBLAVA_ASSIMP)
        set(LIBLAVA_ASSIMP_INC ${LIBLAVA_EXT_DIR}/assimp/include)
        set(LIBLAVA_ASSIMP_LIB assimp)
        target_compile_definitions(lava.resource PRIVATE LIBLAVA_ASSIMP=1)
else()
        set(LIBLAVA_ASSIMP_INC "")
        set(LIBLAVA_ASSIMP_LIB "")
endif()

target_include_directories(lava.resource PUBLIC
        ${LIBLAVA_EXT_DIR}/stb
        ${LIBLAVA_EXT_DIR}/gli
        ${LIBLAVA_EXT_DIR}/tinyobjloader
        ${LIBLAVA_EXT_DIR}/bitmap
        ${LIBLAVA_EXT_DIR}/selene
        ${LIBLAVA_ASSIMP_INC}
        )

message(">>> ext/assimp")

if(LIBLAVA_ASSIMP)
        set(BUILD_SHARED_LIBS OFF)
        set(ASSIMP_BUILD_TESTS OFF)
        set(INJECT_DEBUG_POSTFIX OFF)
        set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
        add_subdirectory(${LIBLAVA_EXT_DIR}/assimp assimp EXCLUDE_FROM_ALL)
endif()

message("<<< ext/assimp")
message(">>> ext/selene")

add_subdirectory(${LIBLAVA_EXT_DIR}/selene selene EXCLUDE_FROM_ALL)

message("<<< ext/glfw")

target_link_libraries(lava.resource
        lava.base
        ${LIBLAVA_ASSIMP_LIB}
        )

message(">> liblava/frame")

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

message(">>> ext/glfw")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBLAVA_EXT_DIR}/glfw glfw EXCLUDE_FROM_ALL)

message("<<< ext/glfw")

target_link_libraries(lava.frame
        lava.resource
        glfw
        ${GLFW_LIBRARIES}
        )

message(">> tests/driver")

add_executable(lava 
        ${LIBLAVA_TESTS_DIR}/driver.cpp
        ${LIBLAVA_TESTS_DIR}/driver.hpp
        ${LIBLAVA_TESTS_DIR}/tests.cpp
        )

target_link_libraries(lava lava.frame)

message("========================================================================")