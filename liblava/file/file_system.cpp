/**
 * @file         liblava/file/file_system.cpp
 * @brief        File system
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include "liblava/file/file_system.hpp"
#include "physfs.h"

namespace lava {

//-----------------------------------------------------------------------------
int_version file_system::get_version() {
    PHYSFS_Version result;
    PHYSFS_getLinkedVersion(&result);

    return { result.major, result.minor, result.patch };
}

//-----------------------------------------------------------------------------
string file_system::get_base_dir() {
    return string(PHYSFS_getBaseDir());
}

//-----------------------------------------------------------------------------
string file_system::get_full_base_dir(string_ref path) {
    return std::filesystem::path(get_base_dir() + path)
        .lexically_normal()
        .string();
}

//-----------------------------------------------------------------------------
string file_system::get_pref_dir() {
    return string(PHYSFS_getPrefDir(str(org), str(app)));
}

//-----------------------------------------------------------------------------
string file_system::get_res_dir() {
    return get_full_base_dir(res_path);
}

//-----------------------------------------------------------------------------
bool file_system::mount(string_ref path) {
    return PHYSFS_mount(str(path), nullptr, 1) != 0;
}

//-----------------------------------------------------------------------------
bool file_system::mount_base(string_ref base_dir_path) {
    return mount(get_base_dir() + base_dir_path);
}

//-----------------------------------------------------------------------------
bool file_system::exists(string_ref file) {
    return PHYSFS_exists(str(file)) != 0;
}

//-----------------------------------------------------------------------------
string file_system::get_real_dir(string_ref file) {
    return PHYSFS_getRealDir(str(file));
}

//-----------------------------------------------------------------------------
string_list file_system::enumerate_files(string_ref path) {
    string_list result;

    auto files = PHYSFS_enumerateFiles(str(path));
    for (auto i = files; *i != nullptr; ++i)
        result.push_back(*i);

    PHYSFS_freeList(files);

    return result;
}

//-----------------------------------------------------------------------------
bool file_system::initialize(string_ref argv_0,
                             string_ref o,
                             string_ref a,
                             string_ref e) {
    LAVA_ASSERT(!initialized); // only once
    if (initialized)
        return initialized;

    if (!initialized) {
        PHYSFS_init(str(argv_0));

        PHYSFS_setSaneConfig(str(o), str(a), str(e), 0, 0);
        initialized = true;

        org = o;
        app = a;
        ext = e;
    }

    return initialized;
}

//-----------------------------------------------------------------------------
void file_system::terminate() {
    if (!initialized)
        return;

    PHYSFS_deinit();
}

//-----------------------------------------------------------------------------
string_list file_system::mount_res() {
#if LAVA_DEBUG
    #if _WIN32
    res_path = "../../res/";
    #else
    res_path = "../res/";
    #endif
#else
    res_path = "res/";
#endif

    string_list result;

    auto res_dir = get_res_dir();

    if (std::filesystem::exists(res_dir))
        if (mount(res_dir))
            result.push_back(res_dir);

    auto cwd_res_dir = get_full_base_dir("res/");

    if (std::filesystem::exists(cwd_res_dir)
        && (cwd_res_dir != get_res_dir()))
        if (mount(cwd_res_dir))
            result.push_back(cwd_res_dir);

    string archive_file = get_full_base_dir("res.zip");
    if (std::filesystem::exists(archive_file))
        if (mount(archive_file))
            result.push_back(archive_file);

    return result;
}

//-----------------------------------------------------------------------------
bool file_system::create_folder(string_ref name) {
    std::filesystem::path path = get_pref_dir();
    path += std::filesystem::path::preferred_separator;
    path += name;

    if (!std::filesystem::exists(path))
        std::filesystem::create_directories(path);

    return std::filesystem::exists(path);
}

//-----------------------------------------------------------------------------
void file_system::clean_pref_dir() {
    std::filesystem::remove_all(get_pref_dir());
}

} // namespace lava
