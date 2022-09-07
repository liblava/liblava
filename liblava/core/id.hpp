/**
 * @file         liblava/core/id.hpp
 * @brief        Object Identification
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
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
    ui32 value = undef;

    /**
     * @brief Check if the id is valid
     * @return Id is valid or not
     */
    bool valid() const {
        return value != undef;
    }

    /**
     * @brief Convert the id to string
     * @return string    String representation of id
     */
    string to_string() const {
        return std::to_string(value);
    }

    /**
     * @brief Invalidate id
     */
    void invalidate() {
        *this = {};
    }

    /**
     * @brief Compare operator
     */
    auto operator<=>(id const&) const = default;
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
     * @brief Get id factory instance
     * @return ids&    Id factory
     */
    static ids& instance() {
        static ids ids;
        return ids;
    }

    /**
     * @brief Get next id from factory
     * @return id    Next id
     */
    id next() {
        return { ++next_id };
    }

private:
    /// Next id
    std::atomic<ui32> next_id = { undef };
};

/**
 * @brief Add object to id map
 * @tparam T        Type of object
 * @param object    Object to add
 * @param map       Target map
 * @return id       Id of object in map
 */
template<typename T>
inline id add_id_map(T const& object,
                     std::map<id, T>& map) {
    auto next = ids::instance().next();
    map.emplace(next, std::move(object));
    return next;
}

/**
 * @brief Remove object from id map
 * @tparam T        Type of object
 * @param object    Object to remove
 * @param map       Target map
 * @return Removed object from map or object not found
 */
template<typename T>
inline bool remove_id_map(id::ref object,
                          std::map<id, T>& map) {
    if (!map.count(object))
        return false;

    map.erase(object);

    return true;
}

/**
 * @brief Id listeners
 * @tparam T    Listener
 */
template<typename T>
struct id_listeners {
    /**
     * @brief Add listener to map
     * @param listener    Target listener
     * @return id         Id of listener
     */
    id add(typename T::func const& listener) {
        return add_id_map(listener, list);
    }

    /**
     * @brief Remove listener from map by id
     * @param id    Id of listener
     */
    void remove(id& id) {
        if (remove_id_map(id, list))
            id.invalidate();
    }

    /**
     * @brief Get the list
     * @return T::listeners const&    List of listeners
     */
    typename T::listeners const& get_list() const {
        return list;
    }

private:
    /// List of listeners
    typename T::listeners list = {};
};

/**
 * @brief Entity
 */
struct entity : no_copy_no_move, interface {
    /**
     * @brief Construct a new entity
     */
    entity()
    : entity_id(ids::instance().next()) {}

    /**
     * @brief Get the id of entity
     * @return id::ref    Entity id
     */
    id::ref get_id() const {
        return entity_id;
    }

private:
    /// Entity id
    id entity_id;
};

/**
 * @brief Id registry
 * @tparam T       Type of objects hold in registry
 * @tparam Meta    Meta type for object
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
     * @param info    Meta information
     * @return id     Object id
     */
    id create(Meta info = {}) {
        auto object = std::make_shared<T>();
        add(object, info);

        return object->get_id();
    }

    /**
     * @brief Add a object with meta to registry
     * @param object    Object to add
     * @param info      Meta of object
     */
    void add(ptr object,
             Meta info = {}) {
        objects.emplace(object->get_id(), object);
        meta.emplace(object->get_id(), info);
    }

    /**
     * @brief Check if object exists in registry
     * @param object    Object to check
     * @return Object exists or not
     */
    bool exists(id::ref object) const {
        return objects.count(object);
    }

    /**
     * @brief Get the object by id
     * @param object    Object id
     * @return ptr      Shared pointer to object
     */
    ptr get(id::ref object) const {
        return objects.at(object);
    }

    /**
     * @brief Get the meta by id
     * @param object    Object id
     * @return Meta     Meta object
     */
    Meta const& get_meta(id::ref object) const {
        return meta.at(object);
    }

    /**
     * @brief Get all objects
     * @return map const&    Map with objects
     */
    map const& get_all() const {
        return objects;
    }

    /**
     * @brief Get all meta objects
     * @return meta_map const&    Map with metas
     */
    meta_map const& get_all_meta() const {
        return meta;
    }

    /**
     * @brief Update meta of object
     * @param object    Object id
     * @param meta      Meta to update
     * @return Meta updated or not
     */
    bool update(id::ref object,
                Meta const& meta) {
        if (!exists(object))
            return false;

        meta.at(object) = meta;
        return true;
    }

    /**
     * @brief Remove object from registry
     * @param object    Object id
     */
    void remove(id::ref object) {
        objects.erase(object);
        meta.erase(object);
    }

    /**
     * @brief Clear the registry
     */
    void clear() {
        objects.clear();
        meta.clear();
    }

private:
    /// Map of objects
    map objects;

    /// Map of metas
    meta_map meta;
};

} // namespace lava
