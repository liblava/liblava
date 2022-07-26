cmake_minimum_required(VERSION 3.22)

project(liblava VERSION 0.7.3 LANGUAGES C CXX)

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
message(" 2022 alpha / 0.7.3                                        MIT licensed ")
message("========================================================================")
message(" https://git.io/liblava                          https://lava-block.com ")
message("========================================================================")
message("> Making lava flow...                                                   ")
message(">> Configuring may take a little longer the first time - so be patient  ")

option(LIBLAVA_TEST "Enable Test" TRUE)
option(LIBLAVA_DEMO "Enable Demo" TRUE)
option(LIBLAVA_PACK "Enable Pack" TRUE)

option(LIBLAVA_TEMPLATE "Enable Template" TRUE)
set(LIBLAVA_TEMPLATE_NAME "template" CACHE STRING "Name of template project")

option(LIBLAVA_WARNING_AS_ERROR "Enable build warnings as errors" FALSE)

option(IMGUI_DOCKING "Dear ImGui with docking" FALSE)
option(LIBLAVA_EXTERNALS "Enable Third-Party modules" TRUE)

set(LIBLAVA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava)
set(LIBLAVA_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext)
set(LIBLAVA_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

if(NOT ${LIBLAVA_RES_DIR} MATCHES ${CMAKE_CURRENT_BINARY_DIR}/res)
  file(CREATE_LINK "${LIBLAVA_RES_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/res"
       RESULT 0 COPY_ON_ERROR SYMBOLIC)
endif()

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
  set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

include(GNUInstallDirs)

if(LIBLAVA_EXTERNALS)
  message("========================================================================")
  message("> CPM.cmake")

  include(cmake/CPM.cmake)
  include(cmake/ext.cmake)

  set(EXT_FOLDER "ext")
  set_target_properties(spdlog PROPERTIES FOLDER ${EXT_FOLDER})
  set_target_properties(Catch2 PROPERTIES FOLDER ${EXT_FOLDER})
  set_target_properties(Catch2WithMain PROPERTIES FOLDER ${EXT_FOLDER})
  set_target_properties(physfs-static PROPERTIES FOLDER ${EXT_FOLDER})

  set(LIBSHADERC_FOLDER "libshaderc")
  set_target_properties(shaderc PROPERTIES FOLDER ${LIBSHADERC_FOLDER})
  set_target_properties(shaderc_combined_genfile PROPERTIES FOLDER ${LIBSHADERC_FOLDER})
  set_target_properties(shaderc_shared PROPERTIES FOLDER ${LIBSHADERC_FOLDER})
  set_target_properties(shaderc_util PROPERTIES FOLDER ${LIBSHADERC_FOLDER})
  set_target_properties(testdata PROPERTIES FOLDER ${LIBSHADERC_FOLDER})

  set(SHADERC_FOLDER "Shaderc")
  set_target_properties(add-copyright PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(build-version PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(glslc PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(glslc_exe PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(shaderc_combined-pkg-config PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(shaderc_static-pkg-config PROPERTIES FOLDER ${SHADERC_FOLDER})
  set_target_properties(shaderc-pkg-config PROPERTIES FOLDER ${SHADERC_FOLDER})

  set(SPIRV_TOOLS_FOLDER "SPIRV-Tools")
  set_target_properties(core_tables PROPERTIES FOLDER ${SPIRV_TOOLS_FOLDER})
  set_target_properties(enum_string_mapping PROPERTIES FOLDER ${SPIRV_TOOLS_FOLDER})
  set_target_properties(extinst_tables PROPERTIES FOLDER ${SPIRV_TOOLS_FOLDER})
  set_target_properties(spirv-tools-pkg-config PROPERTIES FOLDER ${SPIRV_TOOLS_FOLDER})
  set_target_properties(spirv-tools-shared-pkg-config PROPERTIES FOLDER ${SPIRV_TOOLS_FOLDER})
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

target_compile_features(lava.core INTERFACE cxx_std_20)

target_link_libraries(lava.core INTERFACE
  Threads::Threads
  )

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

target_include_directories(lava.util PUBLIC
  $<BUILD_INTERFACE:${PicoSHA2_SOURCE_DIR}>
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
  ${LIBLAVA_DIR}/asset/image_loader.cpp
  ${LIBLAVA_DIR}/asset/image_loader.hpp
  ${LIBLAVA_DIR}/asset/load_texture.cpp
  ${LIBLAVA_DIR}/asset/load_texture.hpp
  ${LIBLAVA_DIR}/asset/load_mesh.cpp
  ${LIBLAVA_DIR}/asset/load_mesh.hpp
  ${LIBLAVA_DIR}/asset/write_image.cpp
  ${LIBLAVA_DIR}/asset/write_image.hpp
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
  ${LIBLAVA_DIR}/frame/driver.cpp
  ${LIBLAVA_DIR}/frame/driver.hpp
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
  ${LIBLAVA_DIR}/engine/engine.cpp
  ${LIBLAVA_DIR}/engine/engine.hpp
  ${LIBLAVA_DIR}/engine/producer.cpp
  ${LIBLAVA_DIR}/engine/producer.hpp
  ${LIBLAVA_DIR}/engine/property.cpp
  ${LIBLAVA_DIR}/engine/property.hpp
  )

target_include_directories(lava.engine PUBLIC
  $<BUILD_INTERFACE:${shaderc_SOURCE_DIR}/libshaderc/include>
  )

target_link_libraries(lava.engine
  lava::app
  )

set(LIBLAVA_ENGINE_LIBRARIES "shaderc")

set_target_properties(lava.engine PROPERTIES FOLDER "liblava")
set_property(TARGET lava.engine PROPERTY EXPORT_NAME engine)

add_library(lava::engine ALIAS lava.engine)

message("========================================================================")
message("> lava")

set(LIBLAVA_STAGE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava-stage)

add_executable(lava
  ${LIBLAVA_STAGE_DIR}/examples.cpp
  ${LIBLAVA_STAGE_DIR}/main.cpp
  ${LIBLAVA_STAGE_DIR}/tutorial.cpp
  )

target_link_libraries(lava
  lava::engine
  ${LIBLAVA_ENGINE_LIBRARIES}
  )

set_target_properties(lava PROPERTIES FOLDER "lava")
set_property(TARGET lava PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

install(TARGETS lava
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT liblava_Runtime
  )

if(LIBLAVA_TEST)
  message("========================================================================")
  message("> lava-test")

  enable_testing()

  set(UNIT_TESTS
    ${LIBLAVA_DIR}/base/test/queue.cpp
    )

  add_executable(lava-test
    ${LIBLAVA_DIR}/test.hpp
    ${UNIT_TESTS}
    )

  target_link_libraries(lava-test
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    Catch2::Catch2
    Catch2::Catch2WithMain
    )

  set_target_properties(lava-test PROPERTIES FOLDER "lava")
  set_property(TARGET lava-test PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

  add_test(NAME lava-test COMMAND lava-test)

  install(TARGETS lava-test
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT liblava_Runtime
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
  lava.engine
  spdlog
  physfs-static
  glfw
  # shaderc
  # shaderc_util
  # shaderc_combined
  EXPORT LavaTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT liblava_Development
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT liblava_Development
  INCLUDES DESTINATION 
    ${CMAKE_INSTALL_INCLUDEDIR}
    ${CMAKE_INSTALL_INCLUDEDIR}/liblava/ext
  COMPONENT liblava_Development
  )

install(
  DIRECTORY ${LIBLAVA_DIR}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT liblava_Development
  FILES_MATCHING PATTERN "*.hpp"
  )

set(EXT_INCLUDE_DIRS
    ${glm_SOURCE_DIR}
    # ${physfs_SOURCE_DIR}/src
    ${json_SOURCE_DIR}/single_include
    ${spdlog_SOURCE_DIR}/include
    ${PicoSHA2_SOURCE_DIR}
    ${Vulkan-Headers_SOURCE_DIR}/include
    ${Vulkan-Profiles_SOURCE_DIR}/library/include
    ${VulkanMemoryAllocator_SOURCE_DIR}/include
    ${volk_SOURCE_DIR}
    # ${stb_SOURCE_DIR}
    # ${gli_SOURCE_DIR}
    # ${tinyobjloader_SOURCE_DIR}
    # ${glfw_SOURCE_DIR}/include
    # ${Catch2_SOURCE_DIR}
    ${argh_SOURCE_DIR}
    ${imgui_SOURCE_DIR}
    ${IconFontCppHeaders_SOURCE_DIR}
    # ${shaderc_SOURCE_DIR}/libshaderc/include
    )

foreach(DIR ${EXT_INCLUDE_DIRS})
  file(GLOB_RECURSE HEADER_FILES "${DIR}/*.h" "${DIR}/*.hpp" "${DIR}/*.inl")

  foreach(HEADER_FILE ${HEADER_FILES})
    cmake_path(SET PATH ${HEADER_FILE})
    cmake_path(RELATIVE_PATH PATH BASE_DIRECTORY ${DIR})
    cmake_path(GET PATH PARENT_PATH OUT_DIR)

    install(
      FILES ${HEADER_FILE}
      DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/liblava/ext/${OUT_DIR}"
      COMPONENT liblava_Development
      )
  endforeach()
endforeach()

set(CONFIG_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/lava)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config-version.cmake"
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  cmake/LavaConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config.cmake"
  INSTALL_DESTINATION ${CONFIG_PATH})

install(
  EXPORT LavaTargets
  DESTINATION ${CONFIG_PATH}
  COMPONENT liblava_Development
  NAMESPACE lava::
  FILE LavaTargets.cmake
  EXPORT_LINK_INTERFACE_LIBRARIES
  )

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config-version.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/lava-config.cmake"
  DESTINATION ${CONFIG_PATH}
  COMPONENT liblava_Development
  )

if(LIBLAVA_DEMO)
  message("========================================================================")
  message("> lava-demo")

  set(LIBLAVA_DEMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liblava-demo)

  add_executable(lava-demo
    ${LIBLAVA_DEMO_DIR}/main.cpp
    ${LIBLAVA_DEMO_DIR}/generics.cpp
    ${LIBLAVA_DEMO_DIR}/lamp.cpp
    ${LIBLAVA_DEMO_DIR}/light.cpp
    ${LIBLAVA_DEMO_DIR}/shapes.cpp
    ${LIBLAVA_DEMO_DIR}/spawn.cpp
    ${LIBLAVA_DEMO_DIR}/triangle.cpp
    )

  target_compile_definitions(lava-demo PRIVATE LAVA_DEMO)

  target_link_libraries(lava-demo
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-demo PROPERTIES FOLDER "lava-demo")
  set_property(TARGET lava-demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

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

  target_link_libraries(lava-triangle
    lava::app
    )

  set_target_properties(lava-triangle PROPERTIES FOLDER "lava-demo")
  set_property(TARGET lava-triangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

  message(">> lava-generics")

  set(GENERICS_SHADERS
    res/generics/triangle.frag
    res/generics/float_triangle.vert
    res/generics/int_triangle.vert
    res/generics/double_triangle.vert
    )

  add_executable(lava-generics
    ${LIBLAVA_DEMO_DIR}/generics.cpp
    ${GENERICS_SHADERS}
    )

  source_group("Shader Files" FILES ${GENERICS_SHADERS})

  target_link_libraries(lava-generics
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-generics PROPERTIES FOLDER "lava-demo")
  set_property(TARGET lava-generics PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

  message(">> lava-shapes")

  set(SHAPES_SHADERS
    res/shapes/shapes.frag
    res/shapes/shapes.vert
    )

  add_executable(lava-shapes
    ${LIBLAVA_DEMO_DIR}/shapes.cpp
    ${SHAPES_SHADERS}
    )

  source_group("Shader Files" FILES ${SHAPES_SHADERS})

  target_link_libraries(lava-shapes
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-shapes PROPERTIES FOLDER "lava-demo")
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

  target_link_libraries(lava-lamp
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-lamp PROPERTIES FOLDER "lava-demo")
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

  target_link_libraries(lava-spawn
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-spawn PROPERTIES FOLDER "lava-demo")
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

  target_link_libraries(lava-light
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(lava-light PROPERTIES FOLDER "lava-demo")
  set_property(TARGET lava-light PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")

  # install demo targets

  install(TARGETS
    lava-demo
    lava-triangle
    lava-generics
    lava-shapes
    lava-lamp
    lava-spawn
    lava-light
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT liblava_Runtime
    )

  install(
    DIRECTORY ${LIBLAVA_RES_DIR}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT liblava_Runtime
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

  target_link_libraries(${LIBLAVA_TEMPLATE_NAME}
    lava::engine
    ${LIBLAVA_ENGINE_LIBRARIES}
    )

  set_target_properties(${LIBLAVA_TEMPLATE_NAME} PROPERTIES FOLDER "lava")
  set_property(TARGET ${LIBLAVA_TEMPLATE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_BINARY_DIR}")
endif()

message("========================================================================")

if(LIBLAVA_PACK)
  set(CPACK_PACKAGE_NAME "liblava")
  set(CPACK_PACKAGE_VENDOR "Lava Block OÜ and contributors")
  set(CPACK_PACKAGE_CONTACT "lib@lava-block.com")
  set(CPACK_PACKAGE_HOMEPAGE_URL "https://git.io/liblava")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern C++ and easy-to-use library for the Vulkan® API")

  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})

  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include(InstallRequiredSystemLibraries)

    set(CPACK_GENERATOR "ZIP")
    set(CPACK_SOURCE_GENERATOR "SOURCE_ZIP")
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SOURCE_GENERATOR "SOURCE_TGZ")
  endif()

  set(CPACK_PACKAGE_CHECKSUM SHA256)

  set(CPACK_OUTPUT_FILE_PREFIX packages)

  set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

  include(CPack)
endif()
