/**
 * @file         liblava/core/def.hpp
 * @brief        Defines
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#if defined(NDEBUG)
    #define LIBLAVA_DEBUG 0
#endif

#ifndef LIBLAVA_DEBUG
    #define LIBLAVA_DEBUG 1
#endif

#ifndef LIBLAVA_DEBUG_ASSERT
    #define LIBLAVA_DEBUG_ASSERT 0
#endif

#define LIBLAVA_BUILD_DATE __DATE__
#define LIBLAVA_BUILD_TIME __TIME__

#define LIBLAVA_VERSION_MAJOR 0
#define LIBLAVA_VERSION_MINOR 7
#define LIBLAVA_VERSION_PATCH 1
