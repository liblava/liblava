// file      : liblava/file/file_system.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ and contributors
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <filesystem>
#include <liblava/core/version.hpp>

namespace lava {

    namespace fs = std::filesystem;

    struct file_system : no_copy_no_move {
        static file_system& instance() {
            static file_system fs;
            return fs;
        }

        static internal_version get_version();

        static name get_base_dir();
        static string get_base_dir_str();
        static name get_pref_dir();
        static string get_res_dir_str();

        static bool mount(string_ref path);
        static bool mount(name base_dir_path);
        static bool exists(name file);
        static name get_real_dir(name file);
        static string_list enumerate_files(name path);

        bool initialize(name argv_0, name org, name app, name ext);
        void terminate();

        void mount_res();
        bool create_data_folder();
        void clean_pref_dir();

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

} // namespace lava
