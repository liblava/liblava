// file      : liblava/def.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#if defined(NDEBUG)
#define LIBLAVA_DEBUG 0
#endif

#ifndef LIBLAVA_DEBUG
#define LIBLAVA_DEBUG 1
#endif

#define LIBLAVA_BUILD_DATE __DATE__
#define LIBLAVA_BUILD_TIME __TIME__

#define LIBLAVA_VERSION_MAJOR 0
#define LIBLAVA_VERSION_MINOR 5
#define LIBLAVA_VERSION_PATCH 2
