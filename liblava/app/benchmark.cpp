/**
 * @file         liblava/app/benchmark.cpp
 * @brief        Benchmark
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <liblava/app/benchmark.hpp>
#include <liblava/file.hpp>

namespace lava {

//-----------------------------------------------------------------------------
bool parse(cmd_line cmd_line, benchmark_data& data) {
    if (!(cmd_line[{ "-fs", "--frames" }]))
        return false;

    data = {};

    if (auto time = -1; cmd_line({ "-fst", "--frames_time" }) >> time)
        data.time = ms{ time };

    if (auto offset = -1; cmd_line({ "-fso", "--frames_offset" }) >> offset)
        data.offset = ms{ offset };

    if (auto file = cmd_line({ "-fsf", "--frames_file" })) {
        data.file = file.str();
        remove_chars(data.file, _punctuation_marks_);
    }

    if (auto path = cmd_line({ "-fsp", "--frames_path" })) {
        data.path = path.str();
        remove_chars(data.path, _punctuation_marks_);
    }

    cmd_line({ "-fsx", "--frames_exit" }) >> data.exit;

    cmd_line({ "-fsb", "--frames_buffer" }) >> data.buffer_size;

    return true;
}

//-----------------------------------------------------------------------------
void benchmark(frame& frame, benchmark_data& data) {
    auto& cmd_line = frame.get_cmd_line();

    data.values.resize(data.buffer_size);

    frame.add_run([&](id::ref run) {
        auto current = get_current_timestamp_ms();

        auto pre = data.start_timestamp + data.offset;
        if (pre > current)
            return run_continue;

        auto bench = data.start_timestamp + data.offset;
        auto end = bench + data.time;
        if (current <= end) {
            if (data.current >= data.buffer_size) {
                log()->error("benchmark buffer overflow: {}", data.buffer_size);
                return run_abort;
            }

            data.values.at(data.current) = (current - bench).count();

            data.current++;

            return run_continue;
        }

        frame.remove(run);

        frame.add_run_once([&]() {
            if (!write_frames_json(data))
                return run_abort;

            if (data.exit)
                frame.shut_down();

            return run_continue;
        });

        return run_continue;
    });

    data.current = 0;
    data.start_timestamp = get_current_timestamp_ms();
}

//-----------------------------------------------------------------------------
bool write_frames_json(benchmark_data& data) {
    json j;

    j[_benchmark_][_time_] = data.time.count();
    j[_benchmark_][_offset_] = data.offset.count();

    auto timestamp_count = data.current;

    auto timestamps = benchmark_data::list{ data.values.begin(),
                                            data.values.begin() + timestamp_count };

    j[_timestamps_] = timestamps;

    auto frame_count = timestamp_count - 1; // ignore first timestamp

    benchmark_data::list frame_durations;
    frame_durations.resize(frame_count);

    {
        auto last = 0;
        for (auto i = 0u; i < timestamp_count; ++i) {
            if (i == 0) {
                last = timestamps.at(i);
                continue;
            }

            auto timestamp = timestamps.at(i);
            frame_durations.at(i - 1) = timestamp - last;
            last = timestamp;
        }
    }

    j[_benchmark_][_count_] = frame_count;
    j[_frames_] = frame_durations;

    j[_benchmark_][_min_] = *std::min_element(frame_durations.begin(), frame_durations.end());
    j[_benchmark_][_max_] = *std::max_element(frame_durations.begin(), frame_durations.end());
    j[_benchmark_][_avg_] = std::accumulate(frame_durations.begin(), frame_durations.end(), 0) / (r32) frame_count;

    auto file_path = std::filesystem::path(data.file);

    if (!data.path.empty()) {
        file_path = data.path;
        file_path /= data.file;
    }

    file file(str(file_path.string()), file_mode::write);
    if (!file.opened()) {
        log()->error("save benchmark ({}) - {}", str(file_path.string()), str(j.dump()));
        return false;
    }

    auto jString = j.dump(4);

    file.write(jString.data(), jString.size());

    log()->info("save benchmark ({}) - {}", str(file_path.string()), str(j.dump()));
    return true;
}

} // namespace lava
