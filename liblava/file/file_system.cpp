/**
 * @file         liblava/file/file_system.cpp
 * @brief        File system
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#include <physfs.h>
#include <liblava/file/file_system.hpp>
#include <liblava/util/log.hpp>

namespace lava {

//-----------------------------------------------------------------------------
internal_version file_system::get_version() {
    PHYSFS_Version result;
    PHYSFS_getLinkedVersion(&result);

    return { result.major, result.minor, result.patch };
}

//-----------------------------------------------------------------------------
string file_system::get_base_dir() {
    return string(PHYSFS_getBaseDir());
}

//-----------------------------------------------------------------------------
string file_system::get_pref_dir() {
    return string(PHYSFS_getPrefDir(org, app));
}

//-----------------------------------------------------------------------------
string file_system::get_res_dir() {
    auto res_dir = get_base_dir();
    res_dir += res_path;
    string_ref const_res_dir = res_dir;

    return std::filesystem::path(const_res_dir).lexically_normal().string();
}

//-----------------------------------------------------------------------------
bool file_system::mount(string_ref path) {
    return PHYSFS_mount(str(path), nullptr, 1) != 0;
}

//-----------------------------------------------------------------------------
bool file_system::mount(name base_dir_path) {
    return mount(get_base_dir() + base_dir_path);
}

//-----------------------------------------------------------------------------
bool file_system::exists(name file) {
    return PHYSFS_exists(file) != 0;
}

//-----------------------------------------------------------------------------
name file_system::get_real_dir(name file) {
    return PHYSFS_getRealDir(file);
}

//-----------------------------------------------------------------------------
string_list file_system::enumerate_files(name path) {
    string_list result;

    auto rc = PHYSFS_enumerateFiles(path);
    for (auto i = rc; *i != nullptr; ++i)
        result.push_back(*i);

    PHYSFS_freeList(rc);

    return result;
}

//-----------------------------------------------------------------------------
bool file_system::initialize(name argv_0,
                             name o,
                             name a,
                             name e) {
#if LIBLAVA_DEBUG_ASSERT
    assert(!initialized); // only once
#endif
    if (initialized)
        return initialized;

    if (!initialized) {
        PHYSFS_init(argv_0);

        PHYSFS_setSaneConfig(o, a, e, 0, 0);
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
void file_system::mount_res(logger log) {
#if LIBLAVA_DEBUG
    #if _WIN32
    res_path = "../../res/";
    #else
    res_path = "../res/";
    #endif
#else
    res_path = "res/";
#endif

    if (std::filesystem::exists(get_res_dir()))
        if (file_system::mount(str(res_path)))
            log->debug("mount {}", str(get_res_dir()));

    auto cwd_res_dir = std::filesystem::current_path()
                           .append("res/")
                           .lexically_normal()
                           .string();

    if (std::filesystem::exists(cwd_res_dir)
        && (cwd_res_dir != get_res_dir()))
        if (file_system::mount(cwd_res_dir))
            log->debug("mount {}", str(cwd_res_dir));

    string archive_file = "res.zip";
    if (std::filesystem::exists({ archive_file }))
        if (file_system::mount(str(archive_file)))
            log->debug("mount {}", str(archive_file));
}

//-----------------------------------------------------------------------------
bool file_system::create_folder(name name) {
    std::filesystem::path path = file_system::get_pref_dir();
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
