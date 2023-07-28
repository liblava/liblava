/**
 * @file         liblava/app/benchmark.hpp
 * @brief        Benchmark
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/app/def.hpp"
#include "liblava/frame/frame.hpp"

namespace lava {

/**
 * @brief Benchmark data
 */
struct benchmark_data {
    /// Benchmark duration
    ms time = ms{ 10000 };

    /// Warm up time
    ms offset = ms{ 5000 };

    /// Output file
    string file = _benchmark_json_;

    /// Output path (empty: pref_dir)
    string path;

    /// Close app after benchmark
    bool exit = true;

    /// Pre-allocated buffer size for results
    ui32 buffer_size = 100000;

    /// List of frame times
    using list = std::vector<ui32>;

    /// Benchmark results
    list values;

    /// Current frame index
    index current = 0;

    /// Benchmark start timestamp
    ms start_timestamp = ms{ 0 };
};

/**
 * @brief Parse command line arguments and set benchmark data
 * @param cmd_line    Command line arguments
 * @param data        Benchmark data
 * @return Benchmark data is parsed or not ready
 */
bool parse_benchmark(cmd_line cmd_line, benchmark_data& data);

/**
 * @brief Start a benchmark run
 * @param app     App to benchmark
 * @param data    Benchmark data setting
 */
void benchmark(frame& app, benchmark_data& data);

/**
 * @brief Write frames to json file
 * @param data     Benchmark data setting
 * @return Write was successful or failed
 */
bool write_frames_json(benchmark_data& data);

} // namespace lava
