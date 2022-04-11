# file      : CMakeLists.txt
# copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
# license   : MIT; see accompanying LICENSE file

cmake_minimum_required(VERSION 3.22)

project(liblava VERSION 0.7.1 LANGUAGES C CXX)

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
message(" 2022 preview / 0.7.1                                      MIT licensed ")
message("========================================================================")
message(" https://git.io/liblava                          https://lava-block.com ")
message("========================================================================")
message("> Making lava flow...                                                   ")
message("-- (may take a little longer the first time - so be patient)            ")

option(IMGUI_DOCKING "Dear ImGui with docking" FALSE)

option(LIBLAVA_TEST "Enable Test" TRUE)
option(LIBLAVA_DEMO "Enable Demo" TRUE)

option(LIBLAVA_TEMPLATE "Enable Template" TRUE)
set(LIBLAVA_TEMPLATE_NAME "template" CACHE STRING "Name of template project")

option(LIBLAVA_WARNING_AS_ERROR "Enable build warnings as errors" FALSE)

set(LIBLAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava)
set(LIBLAVA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(LIBLAVA_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

if(NOT ${LIBLAVA_RES_DIR} MATCHES ${CMAKE_CURRENT_BINARY_DIR}/res)
        file(CREATE_LINK "${LIBLAVA_RES_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/res" RESULT 0 COPY_ON_ERROR SYMBOLIC)
endif()

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
        set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

include(cmake/version.cmake)

message("========================================================================")
message("> CPM.cmake")

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME argh
        GITHUB_REPOSITORY ${argh_GITHUB}
        GIT_TAG ${argh_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME glfw
        GITHUB_REPOSITORY ${glfw_GITHUB}
        GIT_TAG ${glfw_TAG}
        OPTIONS
                "GLFW_INSTALL OFF"
                "GLFW_BUILD_DOCS OFF"
                "GLFW_BUILD_TESTS OFF"
                "GLFW_BUILD_EXAMPLES OFF"
        )

CPMAddPackage(
        NAME gli
        GITHUB_REPOSITORY ${gli_GITHUB}
        GIT_TAG ${gli_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME glm
        GITHUB_REPOSITORY ${glm_GITHUB}
        GIT_TAG ${glm_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME physfs
        GITHUB_REPOSITORY ${physfs_GITHUB}
        GIT_TAG ${physfs_TAG}
        OPTIONS
                "PHYSFS_ARCHIVE_ZIP ON"
                "PHYSFS_ARCHIVE_7Z OFF"
                "PHYSFS_ARCHIVE_GRP OFF"
                "PHYSFS_ARCHIVE_WAD OFF"
                "PHYSFS_ARCHIVE_HOG OFF"
                "PHYSFS_ARCHIVE_MVL OFF"
                "PHYSFS_ARCHIVE_QPAK OFF"
                "PHYSFS_ARCHIVE_SLB OFF"
                "PHYSFS_ARCHIVE_ISO9660 OFF"
                "PHYSFS_ARCHIVE_VDF OFF"
                "PHYSFS_BUILD_SHARED OFF"
                "PHYSFS_BUILD_TEST OFF"
        )

CPMAddPackage(
        NAME json
        GITHUB_REPOSITORY ${json_GITHUB}
        GIT_TAG ${json_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME spdlog
        GITHUB_REPOSITORY ${spdlog_GITHUB}
        GIT_TAG ${spdlog_TAG}
        )

CPMAddPackage(
        NAME stb
        GITHUB_REPOSITORY ${stb_GITHUB}
        GIT_TAG ${stb_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME tinyobjloader
        GITHUB_REPOSITORY ${tinyobjloader_GITHUB}
        GIT_TAG ${tinyobjloader_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME Vulkan-Headers
        GITHUB_REPOSITORY ${Vulkan-Headers_GITHUB}
        GIT_TAG ${Vulkan-Headers_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME Vulkan-Profiles
        GITHUB_REPOSITORY ${Vulkan-Profiles_GITHUB}
        GIT_TAG ${Vulkan-Profiles_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME volk
        GITHUB_REPOSITORY ${volk_GITHUB}
        GIT_TAG ${volk_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME VulkanMemoryAllocator
        GITHUB_REPOSITORY ${VulkanMemoryAllocator_GITHUB}
        GIT_TAG ${VulkanMemoryAllocator_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME IconFontCppHeaders
        GITHUB_REPOSITORY ${IconFontCppHeaders_GITHUB}
        GIT_TAG ${IconFontCppHeaders_TAG}
        DOWNLOAD_ONLY YES
        )

if(IMGUI_DOCKING)
        CPMAddPackage(
                NAME imgui
                GITHUB_REPOSITORY ${imgui_docking_GITHUB}
                GIT_TAG ${imgui_docking_TAG}
                DOWNLOAD_ONLY YES
                )
else()
        CPMAddPackage(
                NAME imgui
                GITHUB_REPOSITORY ${imgui_GITHUB}
                GIT_TAG ${imgui_TAG}
                DOWNLOAD_ONLY YES
                )
endif()

CPMAddPackage(
        NAME SPIRV-Headers
        GITHUB_REPOSITORY ${SPIRV-Headers_GITHUB}
        GIT_TAG ${SPIRV-Headers_TAG}
        DOWNLOAD_ONLY YES
        )

CPMAddPackage(
        NAME SPIRV-Tools
        GITHUB_REPOSITORY ${SPIRV-Tools_GITHUB}
        GIT_TAG ${SPIRV-Tools_TAG}
        OPTIONS
                "SPIRV_SKIP_TESTS ON"
                "SPIRV_SKIP_EXECUTABLES ON"
        )

CPMAddPackage(
        NAME glslang
        GITHUB_REPOSITORY ${glslang_GITHUB}
        GIT_TAG ${glslang_TAG}
        OPTIONS
                "ENABLE_CTEST OFF"
        )

CPMAddPackage(
        NAME shaderc
        GITHUB_REPOSITORY ${shaderc_GITHUB}
        GIT_TAG ${shaderc_TAG}
        OPTIONS
                "SHADERC_SKIP_TESTS ON"
                "SHADERC_SKIP_EXAMPLES ON"
                "SHADERC_SKIP_COPYRIGHT_CHECK ON"
                #"SHADERC_ENABLE_INSTALL ON"
        )

if(LIBLAVA_TEST)
        CPMAddPackage(
                NAME Catch2
                GITHUB_REPOSITORY ${Catch2_GITHUB}
                GIT_TAG ${Catch2_TAG}
                )
endif()

message("========================================================================")
message("> module")

message(">> lava::core")

find_package(Threads REQUIRED)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp "")

add_library(lava.core STATIC
        ${CMAKE_CURRENT_BINARY_DIR}/empty.cpp
        ${LIBLAVA_DIR}/core/data.hpp
        ${LIBLAVA_DIR}/core/def.hpp
        ${LIBLAVA_DIR}/core/hex.hpp
        ${LIBLAVA_DIR}/core/id.hpp
        ${LIBLAVA_DIR}/core/math.hpp
        ${LIBLAVA_DIR}/core/time.hpp
        ${LIBLAVA_DIR}/core/types.hpp
        ${LIBLAVA_DIR}/core/version.hpp
        )

target_include_directories(lava.core INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${glm_SOURCE_DIR}>
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

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(lava.core INTERFACE "-Wno-psabi")
        target_link_options(lava.core INTERFACE "-latomic")
endif()

if(LIBLAVA_WARNING_AS_ERROR)
        target_compile_options(lava.core INTERFACE
                $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
                $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
                )
endif()

set_target_properties(lava.core PROPERTIES FOLDER "liblava")
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

target_link_libraries(lava.util INTERFACE
        lava::core
        spdlog
        )

set_target_properties(lava.util PROPERTIES FOLDER "liblava")
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
        ${LIBLAVA_DIR}/file/json.hpp
        )

target_include_directories(lava.file PUBLIC
        $<BUILD_INTERFACE:${physfs_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${json_SOURCE_DIR}/single_include>
        )

target_link_libraries(lava.file
        lava::util
        physfs-static
        )

set_target_properties(lava.file PROPERTIES FOLDER "liblava")
set_property(TARGET lava.file PROPERTY EXPORT_NAME file)
add_library(lava::file ALIAS lava.file)

message(">> lava::base")

add_library(lava.base STATIC
        ${LIBLAVA_DIR}/base/base.cpp
        ${LIBLAVA_DIR}/base/base.hpp
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
        ${LIBLAVA_DIR}/base/platform.cpp
        ${LIBLAVA_DIR}/base/platform.hpp
        ${LIBLAVA_DIR}/base/profile.hpp
        ${LIBLAVA_DIR}/base/queue.cpp
        ${LIBLAVA_DIR}/base/queue.hpp
        ${volk_SOURCE_DIR}/volk.c
        )

target_include_directories(lava.base PUBLIC
        $<BUILD_INTERFACE:${Vulkan-Headers_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${Vulkan-Profiles_SOURCE_DIR}/library/include>
        $<BUILD_INTERFACE:${VulkanMemoryAllocator_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${volk_SOURCE_DIR}>
        )

target_link_libraries(lava.base
        lava::util
        ${CMAKE_DL_LIBS}
        )

set_target_properties(lava.base PROPERTIES FOLDER "liblava")
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
        ${LIBLAVA_DIR}/resource/primitive.hpp
        ${LIBLAVA_DIR}/resource/mesh.hpp
        ${LIBLAVA_DIR}/resource/texture.cpp
        ${LIBLAVA_DIR}/resource/texture.hpp
        )

target_link_libraries(lava.resource
        lava::base
        )

set_target_properties(lava.resource PROPERTIES FOLDER "liblava")
set_property(TARGET lava.resource PROPERTY EXPORT_NAME resource)
add_library(lava::resource ALIAS lava.resource)

message(">> lava::asset")

add_library(lava.asset STATIC
        ${LIBLAVA_DIR}/asset/image_data.cpp
        ${LIBLAVA_DIR}/asset/image_data.hpp
        ${LIBLAVA_DIR}/asset/mesh_loader.cpp
        ${LIBLAVA_DIR}/asset/mesh_loader.hpp
        ${LIBLAVA_DIR}/asset/texture_loader.cpp
        ${LIBLAVA_DIR}/asset/texture_loader.hpp
        )

target_include_directories(lava.asset PUBLIC
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
        $<BUILD_INTERFACE:${gli_SOURCE_DIR}>
        $<BUILD_INTERFACE:${tinyobjloader_SOURCE_DIR}>
        )

target_link_libraries(lava.asset
        lava::resource
        lava::file
        )

set_target_properties(lava.asset PROPERTIES FOLDER "liblava")
set_property(TARGET lava.asset PROPERTY EXPORT_NAME asset)
add_library(lava::asset ALIAS lava.asset)

message(">> lava::frame")

add_library(lava.frame STATIC
        ${LIBLAVA_DIR}/frame/argh.hpp
        ${LIBLAVA_DIR}/frame/frame.cpp
        ${LIBLAVA_DIR}/frame/frame.hpp
        ${LIBLAVA_DIR}/frame/gamepad.cpp
        ${LIBLAVA_DIR}/frame/gamepad.hpp
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
        $<BUILD_INTERFACE:${glfw_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${argh_SOURCE_DIR}>
        )

target_link_libraries(lava.frame
        lava::resource
        glfw
        ${GLFW_LIBRARIES}
        )

set_target_properties(lava.frame PROPERTIES FOLDER "liblava")
set_property(TARGET lava.frame PROPERTY EXPORT_NAME frame)
add_library(lava::frame ALIAS lava.frame)

message(">> lava::block")

add_library(lava.block STATIC
        ${LIBLAVA_DIR}/block/attachment.hpp
        ${LIBLAVA_DIR}/block/block.cpp
        ${LIBLAVA_DIR}/block/block.hpp
        ${LIBLAVA_DIR}/block/compute_pipeline.cpp
        ${LIBLAVA_DIR}/block/compute_pipeline.hpp
        ${LIBLAVA_DIR}/block/descriptor.cpp
        ${LIBLAVA_DIR}/block/descriptor.hpp
        ${LIBLAVA_DIR}/block/graphics_pipeline.cpp
        ${LIBLAVA_DIR}/block/graphics_pipeline.hpp
        ${LIBLAVA_DIR}/block/pipeline.cpp
        ${LIBLAVA_DIR}/block/pipeline.hpp
        ${LIBLAVA_DIR}/block/pipeline_layout.cpp
        ${LIBLAVA_DIR}/block/pipeline_layout.hpp
        ${LIBLAVA_DIR}/block/render_pass.cpp
        ${LIBLAVA_DIR}/block/render_pass.hpp
        ${LIBLAVA_DIR}/block/subpass.cpp
        ${LIBLAVA_DIR}/block/subpass.hpp
        )

target_link_libraries(lava.block
        lava::base
        )

set_target_properties(lava.block PROPERTIES FOLDER "liblava")
set_property(TARGET lava.block PROPERTY EXPORT_NAME block)
add_library(lava::block ALIAS lava.block)

message(">> lava::app")

set(IMGUI_FILES
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        )

source_group("imgui Files" FILES ${IMGUI_FILES})

set(APP_SHADERS
        res/imgui/imgui.frag
        res/imgui/imgui.vert
        )

source_group("Shader Files" FILES ${APP_SHADERS})

add_library(lava.app STATIC
        ${LIBLAVA_DIR}/app/app.cpp
        ${LIBLAVA_DIR}/app/app.hpp
        ${LIBLAVA_DIR}/app/benchmark.cpp
        ${LIBLAVA_DIR}/app/benchmark.hpp
        ${LIBLAVA_DIR}/app/camera.cpp
        ${LIBLAVA_DIR}/app/camera.hpp
        ${LIBLAVA_DIR}/app/config.cpp
        ${LIBLAVA_DIR}/app/config.hpp
        ${LIBLAVA_DIR}/app/def.hpp
        ${LIBLAVA_DIR}/app/forward_shading.cpp
        ${LIBLAVA_DIR}/app/forward_shading.hpp
        ${LIBLAVA_DIR}/app/icon.hpp
        ${LIBLAVA_DIR}/app/imgui.cpp
        ${LIBLAVA_DIR}/app/imgui.hpp
        ${IMGUI_FILES}
        ${APP_SHADERS}
        )

target_include_directories(lava.app PUBLIC
        $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
        $<BUILD_INTERFACE:${IconFontCppHeaders_SOURCE_DIR}>
        )

target_link_libraries(lava.app
        lava::block
        lava::frame
        lava::asset
        )

target_compile_definitions(lava.app PRIVATE IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

set_target_properties(lava.app PROPERTIES FOLDER "liblava")
set_property(TARGET lava.app PROPERTY EXPORT_NAME app)
add_library(lava::app ALIAS lava.app)

message(">> lava::engine")

add_library(lava.engine STATIC
        ${LIBLAVA_DIR}/engine/driver.cpp
        ${LIBLAVA_DIR}/engine/driver.hpp
        ${LIBLAVA_DIR}/engine/engine.cpp
        ${LIBLAVA_DIR}/engine/engine.hpp
        ${LIBLAVA_DIR}/engine/producer.cpp
        ${LIBLAVA_DIR}/engine/producer.hpp
        ${LIBLAVA_DIR}/engine/property.cpp
        ${LIBLAVA_DIR}/engine/property.hpp
        )

target_link_libraries(lava.engine
        lava::app
        shaderc
        )

set_target_properties(lava.engine PROPERTIES FOLDER "liblava")
set_property(TARGET lava.engine PROPERTY EXPORT_NAME engine)
add_library(lava::engine ALIAS lava.engine)

message("========================================================================")
message("> lava")

add_executable(lava
        ${LIBLAVA_DIR}/engine/stage/examples.cpp
        ${LIBLAVA_DIR}/engine/stage/tutorial.cpp
        ${LIBLAVA_DIR}/engine/main.cpp
        )

target_link_libraries(lava lava::engine)

set_property(TARGET lava PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

install(TARGETS
        lava
        RUNTIME DESTINATION bin
        )


if(LIBLAVA_TEST)
message("========================================================================")
        message("> lava-test")

        set(UNIT_TESTS
                ${LIBLAVA_DIR}/base/test/queue.cpp
                )

        add_executable(lava-test
                ${LIBLAVA_DIR}/test.hpp
                ${UNIT_TESTS}
                )

        target_link_libraries(lava-test lava::engine Catch2::Catch2 Catch2::Catch2WithMain)

        set_property(TARGET lava-test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        install(TARGETS
                lava-test
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
        #lava.engine
        spdlog
        physfs-static
        glfw
        #shaderc
        #shaderc_util
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

set(EXT_INCLUDE_DIRS
        ${glm_SOURCE_DIR}
        #${physfs_SOURCE_DIR}/src
        ${json_SOURCE_DIR}/single_include
        ${spdlog_SOURCE_DIR}/include
        ${Vulkan-Headers_SOURCE_DIR}/include
        ${Vulkan-Profiles_SOURCE_DIR}/library/include
        ${VulkanMemoryAllocator_SOURCE_DIR}/include
        ${volk_SOURCE_DIR}
        #${stb_SOURCE_DIR}
        #${gli_SOURCE_DIR}
        #${tinyobjloader_SOURCE_DIR}
        #${glfw_SOURCE_DIR}/include
        #${Catch2_SOURCE_DIR}
        ${argh_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${IconFontCppHeaders_SOURCE_DIR}
        #${shaderc_SOURCE_DIR}/libshaderc/include
        )

foreach(DIR ${EXT_INCLUDE_DIRS})
        file(GLOB_RECURSE
                HEADER_FILES
                "${DIR}/*.h"
                "${DIR}/*.hpp"
                "${DIR}/*.inl"
                )

        foreach(HEADER_FILE ${HEADER_FILES})
                cmake_path(SET PATH ${HEADER_FILE})
                cmake_path(RELATIVE_PATH
                        PATH
                        BASE_DIRECTORY ${DIR}
                        )
                cmake_path(GET PATH PARENT_PATH OUT_DIR)

                install(FILES
                        ${HEADER_FILE}
                        DESTINATION "include/liblava/ext/${OUT_DIR}"
                        )
        endforeach()
endforeach()

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

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config-version.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config.cmake"
        DESTINATION ${CONFIG_PATH}
        )

if(LIBLAVA_DEMO)
message("========================================================================")
        message("> lava-demo")

        set(LIBLAVA_DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava-demo)

        message(">> lava-triangle")

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
        target_link_libraries(lava-triangle lava::app)

        set_property(TARGET lava-triangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message(">> lava-generics")

        set(GENERICS_SHADERS
                res/generics/triangle.frag
                res/generics/lava_triangle.vert
                res/generics/int_triangle.vert
                res/generics/double_triangle.vert
                )

        add_executable(lava-generics
                ${LIBLAVA_DEMO_DIR}/generics.cpp
                ${GENERICS_SHADERS}
                )

        source_group("Shader Files" FILES ${GENERICS_SHADERS})

        set_target_properties(lava-generics PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-generics lava::engine)

        set_property(TARGET lava-generics PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message(">> lava-shapes")

        set(SHAPES_SHADERS
                res/shapes/shapes.frag
                res/shapes/shapes.vert)

        add_executable(lava-shapes
                ${LIBLAVA_DEMO_DIR}/shapes.cpp
                ${SHAPES_SHADERS})

        source_group("Shader Files" FILES ${SHAPES_SHADERS})

        set_target_properties(lava-shapes PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-shapes lava::engine)

        set_property(TARGET lava-shapes PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message(">> lava-lamp")

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
        target_link_libraries(lava-lamp lava::engine)

        set_property(TARGET lava-lamp PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message(">> lava-spawn")

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
        target_link_libraries(lava-spawn lava::engine)

        set_property(TARGET lava-spawn PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        message(">> lava-light")

        set(LIGHT_SHADERS
                res/light/gbuffer.frag
                res/light/gbuffer.vert
                res/light/lighting.frag
                res/light/lighting.vert
                res/light/data.inc
                )

        add_executable(lava-light 
                ${LIBLAVA_DEMO_DIR}/light.cpp
                ${LIGHT_SHADERS}
                )

        source_group("Shader Files" FILES ${LIGHT_SHADERS})

        set_target_properties(lava-light PROPERTIES FOLDER "lava-demo")
        target_link_libraries(lava-light lava::engine)

        set_property(TARGET lava-light PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

        install(TARGETS
                lava-triangle
                lava-generics
                lava-shapes
                lava-lamp
                lava-spawn
                lava-light
                RUNTIME DESTINATION bin
                )

        install(DIRECTORY ${LIBLAVA_RES_DIR}
                DESTINATION bin
        )
endif()

if(LIBLAVA_TEMPLATE)
message("========================================================================")
        message("> ${LIBLAVA_TEMPLATE_NAME}")

        set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

        add_executable(${LIBLAVA_TEMPLATE_NAME}
                ${SRC_DIR}/main.cpp
                )

        target_include_directories(${LIBLAVA_TEMPLATE_NAME} PUBLIC
                ${SRC_DIR}
                )

        target_link_libraries(${LIBLAVA_TEMPLATE_NAME} lava::engine)

        set_property(TARGET ${LIBLAVA_TEMPLATE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

message("========================================================================")
