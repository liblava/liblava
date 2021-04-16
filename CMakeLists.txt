# file      : CMakeLists.txt
# copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
# license   : MIT; see accompanying LICENSE file

cmake_minimum_required(VERSION 3.20)

project(liblava VERSION 0.6.1 LANGUAGES C CXX)

message("")
message("========================================================================")
message(" Copyright (c) 2018-present, Lava Block OÜ and contributors             ")
message("========================================================================")
message("                                                                        ")
message("  _|  _|  _|            _|                                              ")
message("  _|      _|_|_|        _|        _|_|_|      _|      _|        _|_|_|  ")
message("  _|  _|  _|    _|      _|      _|    _|      _|      _|      _|    _|  ")
message("  _|  _|  _|    _|      _|      _|    _|        _|  _|        _|    _|  ")
message("  _|  _|  _|_|_|        _|        _|_|_|          _|            _|_|_|  ")
message("                                                                        ")
message("========================================================================")
message(" 2021 preview / 0.6.1                                      MIT licensed ")
message("========================================================================")
message(" https://git.io/liblava                                  lava-block.com ")
message("========================================================================")

message("> module")

if(NOT ${PROJECT_SOURCE_DIR}/res MATCHES ${PROJECT_BINARY_DIR}/res)
        file(CREATE_LINK "${PROJECT_SOURCE_DIR}/res" "${PROJECT_BINARY_DIR}/res" COPY_ON_ERROR SYMBOLIC)
endif()

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

set(LIBLAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava)
set(LIBLAVA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(LIBLAVA_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

message(">> lava::core")

find_package(Threads REQUIRED)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp "")

add_library(lava.core STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp
        ${LIBLAVA_DIR}/core/data.hpp
        ${LIBLAVA_DIR}/core/def.hpp
        ${LIBLAVA_DIR}/core/id.hpp
        ${LIBLAVA_DIR}/core/math.hpp
        ${LIBLAVA_DIR}/core/time.hpp
        ${LIBLAVA_DIR}/core/types.hpp
        ${LIBLAVA_DIR}/core/version.hpp
        )

target_include_directories(lava.core INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/glm>
        )

target_compile_features(lava.core INTERFACE 
        cxx_std_20
        )

target_link_libraries(lava.core INTERFACE
        Threads::Threads
        )

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(lava.core INTERFACE 
                stdc++fs
                )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(lava.core INTERFACE "-Wno-psabi")
        target_link_options(lava.core INTERFACE "-latomic")
endif()

set_target_properties(lava.core PROPERTIES FOLDER "lava")
set_property(TARGET lava.core PROPERTY EXPORT_NAME core)
add_library(lava::core ALIAS lava.core)

message(">> lava::util")

add_library(lava.util STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp
        ${LIBLAVA_DIR}/util/log.hpp
        ${LIBLAVA_DIR}/util/random.hpp
        ${LIBLAVA_DIR}/util/telegram.hpp
        ${LIBLAVA_DIR}/util/thread.hpp
        ${LIBLAVA_DIR}/util/utility.hpp
        )

message(">>> spdlog")

add_subdirectory(${LIBLAVA_EXT_DIR}/spdlog spdlog EXCLUDE_FROM_ALL)

set_target_properties(spdlog PROPERTIES FOLDER "lava/ext")

message("<<< spdlog")

target_link_libraries(lava.util INTERFACE
        lava::core
        spdlog
        )

set_target_properties(lava.util PROPERTIES FOLDER "lava")
set_property(TARGET lava.util PROPERTY EXPORT_NAME util)
add_library(lava::util ALIAS lava.util)

message(">> lava::file")

add_library(lava.file STATIC
        ${LIBLAVA_DIR}/file/file.cpp
        ${LIBLAVA_DIR}/file/file.hpp
        ${LIBLAVA_DIR}/file/file_system.cpp
        ${LIBLAVA_DIR}/file/file_system.hpp
        ${LIBLAVA_DIR}/file/file_utils.cpp
        ${LIBLAVA_DIR}/file/file_utils.hpp
        ${LIBLAVA_DIR}/file/json_file.cpp
        ${LIBLAVA_DIR}/file/json_file.hpp
        )

target_include_directories(lava.file PUBLIC
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/physfs/src>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/json/single_include>
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

set_target_properties(physfs-static PROPERTIES FOLDER "lava/ext")

message("<<< physfs")

target_link_libraries(lava.file
        lava::util
        physfs-static
        )

set_target_properties(lava.file PROPERTIES FOLDER "lava")
set_property(TARGET lava.file PROPERTY EXPORT_NAME file)
add_library(lava::file ALIAS lava.file)

message(">> lava::base")

add_library(lava.base STATIC
        ${LIBLAVA_DIR}/base/base.cpp
        ${LIBLAVA_DIR}/base/base.hpp
        ${LIBLAVA_DIR}/base/object_type.hpp
        ${LIBLAVA_DIR}/base/debug_utils.cpp
        ${LIBLAVA_DIR}/base/debug_utils.hpp
        ${LIBLAVA_DIR}/base/device_table.hpp
        ${LIBLAVA_DIR}/base/device.cpp
        ${LIBLAVA_DIR}/base/device.hpp
        ${LIBLAVA_DIR}/base/instance.cpp
        ${LIBLAVA_DIR}/base/instance.hpp
        ${LIBLAVA_DIR}/base/memory.cpp
        ${LIBLAVA_DIR}/base/memory.hpp
        ${LIBLAVA_DIR}/base/physical_device.cpp
        ${LIBLAVA_DIR}/base/physical_device.hpp
        ${LIBLAVA_DIR}/base/queue.cpp
        ${LIBLAVA_DIR}/base/queue.hpp
        ${LIBLAVA_EXT_DIR}/volk/volk.c
        )

target_include_directories(lava.base PUBLIC
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/Vulkan-Headers/include>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/VulkanMemoryAllocator/src>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/volk>
        )

target_link_libraries(lava.base
        lava::util
        ${CMAKE_DL_LIBS}
        )

set_target_properties(lava.base PROPERTIES FOLDER "lava")
set_property(TARGET lava.base PROPERTY EXPORT_NAME base)
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

target_link_libraries(lava.resource
        lava::base
        )

set_target_properties(lava.resource PROPERTIES FOLDER "lava")
set_property(TARGET lava.resource PROPERTY EXPORT_NAME resource)
add_library(lava::resource ALIAS lava.resource)

message(">> lava::asset")

add_library(lava.asset STATIC
        ${LIBLAVA_DIR}/asset/mesh_loader.cpp
        ${LIBLAVA_DIR}/asset/mesh_loader.hpp
        ${LIBLAVA_DIR}/asset/scope_image.cpp
        ${LIBLAVA_DIR}/asset/scope_image.hpp
        ${LIBLAVA_DIR}/asset/texture_loader.cpp
        ${LIBLAVA_DIR}/asset/texture_loader.hpp
        )

target_include_directories(lava.asset PUBLIC
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/stb>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/gli>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/tinyobjloader>
        )

target_link_libraries(lava.asset
        lava::resource
        lava::file
        )

set_target_properties(lava.asset PROPERTIES FOLDER "lava")
set_property(TARGET lava.asset PROPERTY EXPORT_NAME asset)
add_library(lava::asset ALIAS lava.asset)

message(">> lava::frame")

add_library(lava.frame STATIC
        ${LIBLAVA_DIR}/frame/frame.cpp
        ${LIBLAVA_DIR}/frame/frame.hpp
        ${LIBLAVA_DIR}/frame/input.cpp
        ${LIBLAVA_DIR}/frame/input.hpp
        ${LIBLAVA_DIR}/frame/render_target.cpp
        ${LIBLAVA_DIR}/frame/render_target.hpp
        ${LIBLAVA_DIR}/frame/renderer.cpp
        ${LIBLAVA_DIR}/frame/renderer.hpp
        ${LIBLAVA_DIR}/frame/swapchain.cpp
        ${LIBLAVA_DIR}/frame/swapchain.hpp
        ${LIBLAVA_DIR}/frame/window.cpp
        ${LIBLAVA_DIR}/frame/window.hpp
        )

target_include_directories(lava.frame PUBLIC
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/glfw/include>
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/argh>
        )

message(">>> glfw")

set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LIBLAVA_EXT_DIR}/glfw glfw EXCLUDE_FROM_ALL)

set_target_properties(glfw PROPERTIES FOLDER "lava/ext")

message("<<< glfw")

target_link_libraries(lava.frame
        lava::resource
        glfw
        ${GLFW_LIBRARIES}
        )

set_target_properties(lava.frame PROPERTIES FOLDER "lava")
set_property(TARGET lava.frame PROPERTY EXPORT_NAME frame)
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

set_target_properties(lava.block PROPERTIES FOLDER "lava")
set_property(TARGET lava.block PROPERTY EXPORT_NAME block)
add_library(lava::block ALIAS lava.block)

message(">> lava::app")

set(IMGUI_FILES
        ${LIBLAVA_EXT_DIR}/imgui/imgui.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui.h
        ${LIBLAVA_EXT_DIR}/imgui/imgui_draw.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_tables.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_widgets.cpp
        ${LIBLAVA_EXT_DIR}/imgui/imgui_demo.cpp
        )

source_group("imgui Files" FILES ${IMGUI_FILES})

set(APP_SHADERS
        res/tool/imgui/imgui.frag
        res/tool/imgui/imgui.vert
        )

source_group("Shader Files" FILES ${APP_SHADERS})

add_library(lava.app STATIC
        ${LIBLAVA_DIR}/app/app.cpp
        ${LIBLAVA_DIR}/app/app.hpp
        ${LIBLAVA_DIR}/app/camera.cpp
        ${LIBLAVA_DIR}/app/camera.hpp
        ${LIBLAVA_DIR}/app/config.cpp
        ${LIBLAVA_DIR}/app/config.hpp
        ${LIBLAVA_DIR}/app/def.hpp
        ${LIBLAVA_DIR}/app/forward_shading.cpp
        ${LIBLAVA_DIR}/app/forward_shading.hpp
        ${LIBLAVA_DIR}/app/imgui.cpp
        ${LIBLAVA_DIR}/app/imgui.hpp
        ${IMGUI_FILES}
        ${APP_SHADERS}
        )

target_include_directories(lava.app PUBLIC
        $<BUILD_INTERFACE:${LIBLAVA_EXT_DIR}/imgui>
        )

target_link_libraries(lava.app
        lava::block
        lava::frame
        lava::asset
        )

target_compile_definitions(lava.app PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

set_target_properties(lava.app PROPERTIES FOLDER "lava")
set_property(TARGET lava.app PROPERTY EXPORT_NAME app)
add_library(lava::app ALIAS lava.app)

option(LIBLAVA_TESTS "Enable Tests" TRUE)
if(LIBLAVA_TESTS)
message("========================================================================")

        message("> tests")

        message(">> lava")

        add_executable(lava 
                ${LIBLAVA_TESTS_DIR}/driver.cpp
                ${LIBLAVA_TESTS_DIR}/driver.hpp
                ${LIBLAVA_TESTS_DIR}/tests.cpp
                )

        set_target_properties(lava PROPERTIES FOLDER "lava/tests")

        target_link_libraries(lava lava::app)

        set_property(TARGET lava PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        install(TARGETS
                lava
                RUNTIME DESTINATION bin
                )

        message(">> lava-unit (Catch2)")

        add_subdirectory(${LIBLAVA_EXT_DIR}/Catch2 Catch2 EXCLUDE_FROM_ALL)
        set_target_properties(Catch2 PROPERTIES FOLDER "lava/ext")
        set_target_properties(Catch2WithMain PROPERTIES FOLDER "lava/ext")

        add_executable(lava-unit 
                ${LIBLAVA_TESTS_DIR}/unit.cpp
                )

        set_target_properties(lava-unit PROPERTIES FOLDER "lava/tests")

        target_link_libraries(lava-unit lava::app Catch2::Catch2 Catch2::Catch2WithMain)

        set_property(TARGET lava-unit PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        install(TARGETS
                lava-unit
                RUNTIME DESTINATION bin
                )
endif()

install(TARGETS
        lava.core
        lava.util
        lava.file
        lava.base
        lava.resource
        lava.asset
        lava.frame
        lava.block
        lava.app
        spdlog
        physfs-static
        glfw
        EXPORT LavaTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include include/liblava/ext
        )

install(DIRECTORY
        ${LIBLAVA_DIR}
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp"
        )

install(DIRECTORY
        ${LIBLAVA_EXT_DIR}/glm/glm
        #${LIBLAVA_EXT_DIR}/physfs/src/
        ${LIBLAVA_EXT_DIR}/json/single_include/
        ${LIBLAVA_EXT_DIR}/spdlog/include/
        ${LIBLAVA_EXT_DIR}/Vulkan-Headers/include/
        ${LIBLAVA_EXT_DIR}/VulkanMemoryAllocator/src/
        ${LIBLAVA_EXT_DIR}/volk/
        #${LIBLAVA_EXT_DIR}/stb/
        #${LIBLAVA_EXT_DIR}/gli/gli
        #${LIBLAVA_EXT_DIR}/tinyobjloader/
        #${LIBLAVA_EXT_DIR}/glfw/include/
        #${LIBLAVA_EXT_DIR}/Catch2/
        ${LIBLAVA_EXT_DIR}/argh/
        ${LIBLAVA_EXT_DIR}/imgui/
        DESTINATION include/liblava/ext
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
        )

set(CONFIG_PATH lib/cmake/lava)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config-version.cmake"
        COMPATIBILITY AnyNewerVersion
        )

configure_package_config_file(
        cmake/LavaConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config.cmake"
        INSTALL_DESTINATION ${CONFIG_PATH}
        )

install(EXPORT LavaTargets
        DESTINATION ${CONFIG_PATH}
        NAMESPACE lava::
        FILE LavaTargets.cmake
        EXPORT_LINK_INTERFACE_LIBRARIES
        )

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config-version.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config.cmake"
        DESTINATION ${CONFIG_PATH}
        )

option(LIBLAVA_DEMO "Enable Demo" TRUE)
if(LIBLAVA_DEMO)
message("========================================================================")
        
        message("> module")

        set(LIBLAVA_DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava-demo)

        message(">> lava::demo")

        add_library(lava.demo STATIC
                ${LIBLAVA_DEMO_DIR}/demo.cpp
                ${LIBLAVA_DEMO_DIR}/demo.hpp
                )

        target_include_directories(lava.demo PUBLIC
                ${LIBLAVA_DEMO_DIR}
                ${LIBLAVA_EXT_DIR}/IconFontCppHeaders
                )

        set_target_properties(lava.demo PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava.demo lava::app)

        add_library(lava::demo ALIAS lava.demo)

        message("> lava-triangle")

        set(TRIANGLE_SHADERS
                res/triangle/triangle.frag
                res/triangle/triangle.vert
                )

        add_executable(lava-triangle 
                ${LIBLAVA_DEMO_DIR}/triangle.cpp
                ${TRIANGLE_SHADERS}
                )

        source_group("Shader Files" FILES ${TRIANGLE_SHADERS})

        set_target_properties(lava-triangle PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-triangle lava::demo)

        set_property(TARGET lava-triangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message("> lava-lamp")

        set(LAMP_SHADERS
                res/lamp/lamp.frag
                res/lamp/lamp.vert
                )

        add_executable(lava-lamp 
                ${LIBLAVA_DEMO_DIR}/lamp.cpp
                ${LAMP_SHADERS}
                )

        source_group("Shader Files" FILES ${LAMP_SHADERS})

        set_target_properties(lava-lamp PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-lamp lava::demo)

        set_property(TARGET lava-lamp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message("> lava-spawn")

        set(SPAWN_SHADERS
                res/spawn/spawn.frag
                res/spawn/spawn.vert
                )

        add_executable(lava-spawn 
                ${LIBLAVA_DEMO_DIR}/spawn.cpp
                ${SPAWN_SHADERS}
                )

        source_group("Shader Files" FILES ${SPAWN_SHADERS})

        set_target_properties(lava-spawn PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-spawn lava::demo)

        set_property(TARGET lava-spawn PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

option(LIBLAVA_TEMPLATE "Enable Template" TRUE)
if(LIBLAVA_TEMPLATE)
message("========================================================================")

        set(NAME template)

        message("> ${NAME}")

        set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

        add_executable(${NAME}
                ${SRC_DIR}/main.cpp
                )

        target_include_directories(${NAME} PUBLIC
                ${SRC_DIR}
                )

        target_link_libraries(${NAME} lava::app)

        set_property(TARGET ${NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

message("========================================================================")
