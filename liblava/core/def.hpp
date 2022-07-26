/**
 * @file         liblava/core/def.hpp
 * @brief        Defines
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#if defined(NDEBUG)
    #define LAVA_DEBUG 0
#endif

#ifndef LAVA_DEBUG
    #define LAVA_DEBUG 1
#endif

#ifndef LAVA_DEBUG_CONFIG
    #define LAVA_DEBUG_CONFIG LAVA_DEBUG
#endif

#define LAVA_BUILD_DATE __DATE__
#define LAVA_BUILD_TIME __TIME__

#define LAVA_VERSION_MAJOR 0
#define LAVA_VERSION_MINOR 7
#define LAVA_VERSION_PATCH 3
