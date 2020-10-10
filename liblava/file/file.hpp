// file      : liblava/file/file.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <filesystem>
#include <fstream>
#include <liblava/core/data.hpp>
#include <liblava/util/log.hpp>
#include <nlohmann/json.hpp>

// fwd
struct PHYSFS_File;

namespace lava {

    constexpr name _zip_ = "zip";
    constexpr name _config_file_ = "config.json";

    using json = nlohmann::json;

    namespace fs = std::filesystem;

    bool read_file(std::vector<char>& out, name filename);

    bool write_file(name filename, char const* data, size_t data_size);

    bool extension(name file_name, name extension);

    bool extension(name filename, names extensions);

    string get_filename_from(string_ref path, bool with_extension = false);

    bool remove_existing_path(string& target, string_ref path);

    struct file_guard : no_copy_no_move {
        explicit file_guard(name filename = "")
        : filename(filename) {}
        explicit file_guard(string filename)
        : filename(filename) {}

        ~file_guard() {
            if (remove)
                fs::remove(filename);
        }

        string filename;
        bool remove = true;
    };

    struct file_system : no_copy_no_move {
        static file_system& instance() {
            static file_system fs;
            return fs;
        }

        static internal_version get_version();

        static name get_base_dir();
        static string get_base_dir_str();
        static name get_pref_dir();
        static string get_res_dir();

        static bool mount(string_ref path);
        static bool mount(name base_dir_path);
        static bool exists(name file);
        static name get_real_dir(name file);
        static string_list enumerate_files(name path);

        bool initialize(name argv_0, name org, name app, name ext);
        void terminate();

        void mount_res();
        bool create_data_folder();

        name get_org() const {
            return org;
        }
        name get_app() const {
            return app;
        }
        name get_ext() const {
            return ext;
        }

        bool ready() const {
            return initialized;
        }

    private:
        file_system() = default;

        bool initialized = false;

        name org = nullptr;
        name app = nullptr;
        name ext = nullptr;

        string res_path;
    };

    enum class file_type : type {
        none = 0,
        fs,
        f_stream
    };

    constexpr i64 const file_error_result = -1;

    inline bool file_error(i64 result) {
        return result == file_error_result;
    }

    struct file : no_copy_no_move {
        explicit file(name path = nullptr, bool write = false);
        ~file();

        bool open(name path, bool write = false);
        void close();

        bool opened() const;
        i64 get_size() const;

        i64 read(data_ptr data) {
            return read(data, to_ui64(get_size()));
        }
        i64 read(data_ptr data, ui64 size);

        i64 write(data_cptr data, ui64 size);

        bool writable() const {
            return write_mode;
        }
        file_type get_type() const {
            return type;
        }

        name get_path() const {
            return path;
        }

    private:
        file_type type = file_type::none;
        bool write_mode = false;

        name path = nullptr;

        PHYSFS_File* fs_file = nullptr;
        mutable std::ifstream i_stream;
        mutable std::ofstream o_stream;
    };

    bool load_file_data(string_ref filename, data& target);

    struct file_data : scope_data {
        explicit file_data(string_ref filename) {
            load_file_data(filename, *this);
        }
    };

    struct file_callback {
        using list = std::vector<file_callback*>;

        using func = std::function<void(json&)>;
        func on_load;
        func on_save;
    };

    struct json_file {
        explicit json_file(name path = _config_file_);

        void add(file_callback* callback);
        void remove(file_callback* callback);

        void set(name value) {
            path = value;
        }
        name get() const {
            return str(path);
        }

        bool load();
        bool save();

    private:
        string path;
        file_callback::list callbacks;
    };

} // namespace lava
