/**
 * @file         liblava/block/compute_pipeline.hpp
 * @brief        Compute pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/block/pipeline.hpp"

namespace lava {

/**
 * @brief Compute pipeline
 */
struct compute_pipeline : pipeline {
    /// Shared pointer to compute pipeline
    using s_ptr = std::shared_ptr<compute_pipeline>;

    /// Map of compute pipelines
    using map = std::map<id, s_ptr>;

    /// List of compute pipelines
    using list = std::vector<s_ptr>;

    /**
     * @brief Make a new compute pipeline
     * @param device            Vulkan device
     * @param pipeline_cache    Pipeline cache
     * @return s_ptr            Shared pointer to compute pipeline
     */
    static s_ptr make(device::ptr device,
                      VkPipelineCache pipeline_cache = 0) {
        return std::make_shared<compute_pipeline>(device, pipeline_cache);
    }

    /// Pipeline constructors
    using pipeline::pipeline;

    /**
     * @brief Bind the pipeline
     * @param cmdBuffer    Command buffer
     */
    void bind(VkCommandBuffer cmdBuffer) override;

    /**
     * @brief Set shader stage
     * @param data      Shader data
     * @param stage     Shader stage flag bits
     * @return Set was successful or failed
     */
    bool set_shader_stage(c_data::ref data,
                          VkShaderStageFlagBits stage);

    /**
     * @brief Set shader stage
     * @param stage    Shader state
     */
    void set(shader_stage::s_ptr const& stage) {
        shader_stage = stage;
    }

    /**
     * @brief Get the shader stage
     * @return shader_stage::s_ptr const&    Shader state
     */
    shader_stage::s_ptr const& get_shader_stage() const {
        return shader_stage;
    }

    /**
     * @brief Copy configuration to target pipeline
     * @param target    Compute pipeline
     */
    void copy_to(compute_pipeline* target) const;

    /**
     * @brief Copy configuration from source
     * @param source    Compute pipeline
     */
    void copy_from(s_ptr const& source) {
        source->copy_to(this);
    }

private:
    /**
     * @brief Set up the compute pipeline
     * @return Setup was successful or failed
     */
    bool setup() override;

    /**
     * @brief Tear down the compute pipeline
     */
    void teardown() override;

    /// Shader stage
    shader_stage::s_ptr shader_stage;
};

} // namespace lava
