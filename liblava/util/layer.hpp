/**
 * @file         liblava/util/layer.hpp
 * @brief        Layering
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/core/id.hpp"
#include "liblava/core/misc.hpp"

namespace lava {

/**
 * @brief Layer
 */
struct layer : entity {
    /// Shared pointer to layer
    using s_ptr = std::shared_ptr<layer>;

    /// Map of layers
    using map = std::map<id, s_ptr>;

    /// List of layers
    using list = std::vector<s_ptr>;

    /// Layer function
    using func = std::function<void()>;

    /**
     * @brief Make a new layer
     * @param name      Name of layer
     * @return s_ptr    Shared pointer to layer
     */
    static s_ptr make(string_ref name) {
        return std::make_shared<layer>(name);
    }

    /**
     * @brief Construct a new layer
     * @param name    Name of layer
     */
    layer(string_ref name)
    : name(name) {}

    /// Called by layering
    func on_func;

    /// Active state
    bool active = true;

    /// Name of layer
    string name;
};

/**
 * @brief Layer list
 */
struct layer_list {
    /// Pointer to layer list
    using ptr = layer_list*;

    /**
     * @brief Add a new layer
     * @param name      Name of layer
     * @param func      Layer function
     * @param active    Layer active state
     * @return id       Id of added layer
     */
    id add(string_ref name,
           layer::func func,
           bool active = true) {
        auto layer = layer::make(name);

        layer->on_func = func;
        layer->active = active;

        layers.push_back(layer);

        return layer->get_id();
    }

    /**
     * @brief Add a layer
     * @param layer    Layer to add
     */
    void add(layer::s_ptr layer) {
        layers.push_back(layer);
    }

    /**
     * @brief Add a new inactive layer
     * @param name    Name of layer
     * @param func    Layer function
     * @return id     Id of added layer
     */
    id add_inactive(string_ref name,
                    layer::func func) {
        return add(name, func, false);
    }

    /**
     * @brief Get layer in list by id
     * @param layer_id       Id of layer
     * @return layer::ptr    Shared pointer to layer
     */
    layer::s_ptr get(id::ref layer_id) {
        for (auto const& layer : layers)
            if (layer->get_id() == layer_id)
                return layer;

        return nullptr;
    }

    /**
     * @brief Remove layer from list
     * @param layer_id    Id of layer to remove
     * @return Remove was successful or failed
     */
    bool remove(id::ref layer_id) {
        for (auto const& layer : layers) {
            if (layer->get_id() == layer_id) {
                lava::remove(layers, layer);
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Get all layers
     * @return layer::list const&    List of layers
     */
    layer::list const& get_all() const {
        return layers;
    }

    /**
     * @brief Clear layer list
     */
    void clear() {
        layers.clear();
    }

private:
    /// List of layers
    layer::list layers;
};

} // namespace lava
