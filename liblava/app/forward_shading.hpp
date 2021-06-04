/**
 * @file liblava/app/forward_shading.hpp
 * @brief Forward shading
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/render_pass.hpp>
#include <liblava/frame/render_target.hpp>

namespace lava {

/**
 * @brief Forward shading
 */
struct forward_shading : id_obj {
    /**
     * @brief Construct a new forward shading
     */
    explicit forward_shading() = default;

    /**
     * @brief Destroy the forward shading
     */
    ~forward_shading() {
        destroy();
    }

    /**
     * @brief Create a forward shading for a render target
     * 
     * @param target Render target
     * @return true Create was successful
     * @return false Create failed
     */
    bool create(render_target::ptr target);

    /**
     * @brief Destroy the forward shading
     */
    void destroy();

    /**
     * @brief Get the render pass
     * 
     * @return render_pass::ptr Render pass
     */
    render_pass::ptr get_pass() const {
        return pass;
    }

    /**
     * @brief Get the Vulkan render pass
     * 
     * @return VkRenderPass Vulkan Render pass
     */
    VkRenderPass get_vk_pass() const {
        return pass->get();
    }

    /**
     * @brief Get the depth stencil image
     * 
     * @return image::ptr Depth stencil Image
     */
    image::ptr get_depth_stencil() const {
        return depth_stencil;
    }

private:
    /// Render target
    render_target::ptr target;

    /// Render pass
    render_pass::ptr pass;

    /// Depth stencil image
    image::ptr depth_stencil;
};

} // namespace lava
