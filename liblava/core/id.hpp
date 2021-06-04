/**
 * @file liblava/core/id.hpp
 * @brief Object Identification
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <atomic>
#include <deque>
#include <liblava/core/types.hpp>
#include <memory>
#include <mutex>
#include <set>

namespace lava {

/**
 * @brief Identification
 */
struct id {
    /// Reference to id
    using ref = id const&;

    /// Set of ids
    using set = std::set<id>;

    /// Reference to set of ids
    using set_ref = set const&;

    /// List if ids
    using list = std::vector<id>;

    /// Map of ids
    using map = std::map<id, id>;

    /// Index map by ids
    using index_map = std::map<id, index>;

    /// String map by ids
    using string_map = std::map<id, string>;

    /// Value
    type value = undef;

    /// Version
    ui32 version = 0;

    /**
     * @brief Check if the id is valid
     * 
     * @return true Id is valid
     * @return false Id is invalid
     */
    bool valid() const {
        return value != undef;
    }

    /**
     * @brief Convert the id to string
     * 
     * @param show_version Show version in string
     * @return string String representation of id
     */
    string to_string(bool show_version = false) const {
        char result[32];
        if (show_version)
            snprintf(result, sizeof(result), "%u.%u", value, version);
        else
            snprintf(result, sizeof(result), "%u", value);
        return string(result);
    }

    /**
     * @brief Invalidate id
     */
    void invalidate() {
        *this = {};
    }

    /**
     * @brief Equal operator
     * 
     * @param rhs Id to compare
     * @return true Id is equal
     * @return false Id is not equal
     */
    bool operator==(ref rhs) const {
        return (value == rhs.value) && (version == rhs.version);
    }

    /**
     * @brief Not equal operator
     * 
     * @param rhs Id to compare
     * @return true Id is not equal
     * @return false Id is equal
     */
    bool operator!=(ref rhs) const {
        return !(*this == rhs);
    }

    /**
     * @brief Order operator
     * 
     * @param rhs Id to order
     * @return true Id is smaller
     * @return false Id is bigger
     */
    bool operator<(ref rhs) const {
        return std::tie(value, version) < std::tie(rhs.value, rhs.version);
    }

    /**
     * @brief Check if id exists in map, if so reassign it from map
     * 
     * @param map Map to check and reassign
     * @return true Found id in map and reassigned
     * @return false Id ignored
     */
    bool check(id::map& map) {
        if (!valid())
            return false;

        if (!map.count(*this))
            return false;

        *this = map.at(*this);
        return true;
    }
};

/// Map of string ids
using string_id_map = std::map<string, id>;

/// Undefined id
constexpr id const undef_id = id();

/**
 * @brief Id factory
 */
struct ids {
    /**
     * @brief Global id factory
     * 
     * @return ids& Singelton factory
     */
    static ids& global() {
        static ids instance;
        return instance;
    }

    /**
     * @brief Get next id from factory (singleton)
     * 
     * @return id Next id
     */
    static id next() {
        return ids::global().get_next();
    }

    /**
     * @brief Free id in factory (singleton)
     * 
     * @param id Id to free
     */
    static void free(id::ref id) {
        ids::global().reuse(id);
    }

    /**
     * @brief Get the next id
     * 
     * @return id Next id
     */
    id get_next() {
        if (!reuse_ids)
            return { ++next_id };

        return get_next_locked();
    }

    /**
     * @brief Reuse the id
     * 
     * @param id Id to reuse
     */
    void reuse(id::ref id) {
        if (reuse_ids)
            reuse_locked(id);
    }

    /**
     * @brief Set the reuse handling
     * 
     * @param state Enable reuse
     */
    void set_reuse(bool state) {
        reuse_ids = state;
    }

    /**
     * @brief Check if the reuse handling is enabled
     * 
     * @return true Reuse handling is active
     * @return false Reuse handling is inactive
     */
    bool reusing() const {
        return reuse_ids;
    }

    /**
     * @brief Get the max id
     * 
     * @return type Max id
     */
    type get_max() const {
        return next_id;
    }

    /**
     * @brief Set the max id
     * 
     * @param max Max id
     */
    void set_max(type max) {
        if (max > next_id)
            next_id = max;
    }

private:
    /**
     * @brief Get the next locked id
     * 
     * @return id Next id
     */
    id get_next_locked() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (free_ids.empty())
            return { ++next_id };

        auto next_id = free_ids.front();
        free_ids.pop_front();
        return { next_id.value, next_id.version + 1 };
    }

    /**
     * @brief Reuse the id
     * 
     * @param id Id to reuse
     */
    void reuse_locked(id::ref id) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        free_ids.push_back(id);
    }

    /// Next id
    std::atomic<type> next_id = { undef };

    /// Queue mutex
    std::mutex queue_mutex;

    /// Reuse id handling
    bool reuse_ids = true;

    /// List of freed ids
    std::deque<id> free_ids;
};

/**
 * @brief Add object to id map
 * 
 * @tparam T Type of object
 * @param object Object to add
 * @param map Target map
 * @return id Id of object in map
 */
template<typename T>
inline id add_id_map(T const& object, std::map<id, T>& map) {
    auto next = ids::next();
    map.emplace(next, std::move(object));
    return next;
}

/**
 * @brief Remove object from id map
 * 
 * @tparam T Type of object
 * @param object Object to remove
 * @param map Target map
 * @return true Found object in map and removed it
 * @return false Object in map not found
 */
template<typename T>
inline bool remove_id_map(id::ref object, std::map<id, T>& map) {
    if (!map.count(object))
        return false;

    map.erase(object);
    ids::free(object);

    return true;
}

/**
 * @brief Id listeners
 * 
 * @tparam T Listener
 */
template<typename T>
struct id_listeners {
    /**
     * @brief Add listener to map
     * 
     * @param listener Target listener
     * @return id Id of listener
     */
    id add(typename T::func const& listener) {
        return add_id_map(listener, list);
    }

    /**
     * @brief Remove listener from map by id
     * 
     * @param id Id of listener
     */
    void remove(id& id) {
        if (remove_id_map(id, list))
            id.invalidate();
    }

    /**
     * @brief Get the list
     * 
     * @return T::listeners const& List of listeners
     */
    typename T::listeners const& get_list() const {
        return list;
    }

private:
    /// List of listeners
    typename T::listeners list = {};
};

/**
 * @brief Id object
 */
struct id_obj : interface {
    /**
     * @brief Construct a new id object
     */
    id_obj()
    : obj_id(ids::next()) {}

    /**
     * @brief Destroy the id object
     */
    ~id_obj() {
        ids::free(obj_id);
    }

    /**
     * @brief Get the id of object
     * 
     * @return id::ref Object id
     */
    id::ref get_id() const {
        return obj_id;
    }

private:
    /// Object id
    id obj_id;
};

/**
 * @brief Id registry
 * 
 * @tparam T Type of objects hold in registry
 * @tparam Meta Meta type for object
 */
template<typename T, typename Meta>
struct id_registry {
    /// Shared pointer to id registry
    using ptr = std::shared_ptr<T>;

    /// Map of id registries
    using map = std::map<id, ptr>;

    /// Map of ids with meta
    using meta_map = std::map<id, Meta>;

    /**
     * @brief Create a new object in registry
     * 
     * @param info Meta information
     * @return id Object id
     */
    id create(Meta info = {}) {
        auto object = std::make_shared<T>();
        add(object, info);

        return object->get_id();
    }

    /**
     * @brief Add a object with meta to registry
     * 
     * @param object Object to add
     * @param info Meta of object
     */
    void add(ptr object, Meta info = {}) {
        objects.emplace(object->get_id(), object);
        meta.emplace(object->get_id(), info);
    }

    /**
     * @brief Check if object exists in registry
     * 
     * @param object Object to check
     * @return true Object exists
     * @return false Object does not exist
     */
    bool has(id::ref object) const {
        return objects.count(object);
    }

    /**
     * @brief Get the object by id
     * 
     * @param object Object id
     * @return ptr Shared pointer to object
     */
    ptr get(id::ref object) const {
        return objects.at(object).get();
    }

    /**
     * @brief Get the meta by id
     * 
     * @param object Object id
     * @return Meta Copy of meta
     */
    Meta const& get_meta(id::ref object) const {
        return meta.at(object).get();
    }

    /**
     * @brief Get all objects
     * 
     * @return map const& Map with objects
     */
    map const& get_all() const {
        return objects;
    }

    /**
     * @brief Get all meta objects
     * 
     * @return meta_map const& Map with metas
     */
    meta_map const& get_all_meta() const {
        return meta;
    }

    /**
     * @brief Update meta of object
     * 
     * @param object Object id
     * @param meta Meta to update
     * @return true Meta updated
     * @return false Meta not updated
     */
    bool update(id::ref object, Meta const& meta) {
        if (!has(object))
            return false;

        meta.at(object) = meta;
        return true;
    }

    /**
     * @brief Remove object from registry
     * 
     * @param object Object id
     */
    void remove(id::ref object) {
        objects.erase(object);
        meta.erase(object);
    }

private:
    /// Map of objects
    map objects;

    /// Map of metas
    meta_map meta;
};

} // namespace lava
