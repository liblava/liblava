/**
 * @file         liblava/block/compute_pipeline.hpp
 * @brief        Compute pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/pipeline.hpp>

namespace lava {

/**
 * @brief Compute pipeline
 */
struct compute_pipeline : pipeline {
    /// Shared pointer to compute pipeline
    using ptr = std::shared_ptr<compute_pipeline>;

    /// Map of compute pipelines
    using map = std::map<id, ptr>;

    /// List of compute pipelines
    using list = std::vector<ptr>;

    /// Pipeline constructors
    using pipeline::pipeline;

    /**
     * @brief Bind the pipeline
     * 
     * @param cmdBuffer    Command buffer
     */
    void bind(VkCommandBuffer cmdBuffer) override;

    /**
     * @brief Set the shader stage
     * 
     * @param data      Shader data
     * @param stage     Shader stage flag bits
     * 
     * @return true     Set was successful
     * @return false    Set failed
     */
    bool set_shader_stage(cdata const& data, VkShaderStageFlagBits stage);

    /**
     * @brief Set shader stage
     * 
     * @param stage    Shader state
     */
    void set(shader_stage::ptr const& stage) {
        shader_stage = stage;
    }

    /**
     * @brief Get the shader stage
     * 
     * @return shader_stage::ptr const&    Shader state
     */
    shader_stage::ptr const& get_shader_stage() const {
        return shader_stage;
    }

    /**
     * @brief Copy configuration to target pipeline
     * 
     * @param target    Compute pipeline
     */
    void copy_to(compute_pipeline* target) const;

    /**
     * @brief Copy configuration from source
     * 
     * @param source    Compute pipeline
     */
    void copy_from(ptr const& source) {
        source->copy_to(this);
    }

private:
    /**
     * @brief Internal create a new compute pipeline
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create_internal() override;

    /**
     * @brief Internal destroy the compute pipeline
     */
    void destroy_internal() override;

    /// Shader stage
    shader_stage::ptr shader_stage;
};

/**
 * @brief Make a new compute pipeline
 * 
 * @param device                    Vulkan device
 * @param pipeline_cache            Pipeline cache
 * 
 * @return compute_pipeline::ptr    Shared pointer to compute pipeline
 */
inline compute_pipeline::ptr make_compute_pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0) {
    return std::make_shared<compute_pipeline>(device, pipeline_cache);
}

} // namespace lava
