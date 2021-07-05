/**
 * @file         liblava/block/pipeline.hpp
 * @brief        Pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/pipeline_layout.hpp>

namespace lava {

/// Shader main function name
constexpr name _main_ = "main";

/**
 * @brief Pipeline
 */
struct pipeline : entity {
    /// Shared pointer to pipeline
    using ptr = std::shared_ptr<pipeline>;

    /// List of pipelines
    using list = std::vector<ptr>;

    /// Pipeline process function
    using process_func = std::function<void(VkCommandBuffer)>;

    /// Called on pipeline process
    process_func on_process;

    /**
     * @brief Construct a new pipeline
     * 
     * @param device            Vulkan device
     * @param pipeline_cache    Pipeline cache
     */
    explicit pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0);

    /**
     * @brief Destroy the pipeline
     */
    ~pipeline() override;

    /**
     * @brief Create a new pipeline
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create();

    /**
     * @brief Destroy the pipeline
     */
    void destroy();

    /**
     * @brief Bind the pipeline
     * 
     * @param cmd_buf    Command buffer
     */
    virtual void bind(VkCommandBuffer cmd_buf) = 0;

    /**
     * @brief Set pipeline active
     * 
     * @param value    Active state
     */
    void set_active(bool value = true) {
        active = value;
    }

    /**
     * @brief Check if pipeline is active
     * 
     * @return true     Pipeline is active
     * @return false    Pipeline is inactive
     */
    bool activated() const {
        return active;
    }

    /**
     * @brief Toggle activation
     */
    void toggle() {
        active = !active;
    }

    /**
     * @brief Set auto bind
     * 
     * @param value    Enable state
     */
    void set_auto_bind(bool value = true) {
        auto_bind_active = value;
    }

    /**
     * @brief Check if auto bind is enabled
     * 
     * @return true     Auto bind is enabled
     * @return false    Audo bind is disabled
     */
    bool auto_bind() const {
        return auto_bind_active;
    }

    /**
     * @brief Check if pipeline is ready
     * 
     * @return true     Pipeline is ready
     * @return false    Pipeline is not ready
     */
    bool ready() const {
        return vk_pipeline != VK_NULL_HANDLE;
    }

    /**
     * @brief Get the pipeline
     * 
     * @return VkPipeline    Vulkan pipeline
     */
    VkPipeline get() const {
        return vk_pipeline;
    }

    /**
     * @brief Get the device
     * 
     * @return device_ptr    Vulkan device
     */
    device_ptr get_device() {
        return device;
    }

    /**
     * @brief Get the layout
     * 
     * @return pipeline_layout::ptr    Pipeline layout
     */
    pipeline_layout::ptr get_layout() const {
        return layout;
    }

    /**
     * @brief Set the layout
     * 
     * @param value    Pipeline layout
     */
    void set_layout(pipeline_layout::ptr const& value) {
        layout = value;
    }

    /**
     * @brief Shader stage
     */
    struct shader_stage {
        /// Shared pointer to shader stage
        using ptr = std::shared_ptr<shader_stage>;

        /// List of shader stages
        using list = std::vector<ptr>;

        /**
         * @brief Construct a new shader stage
         */
        explicit shader_stage();

        /**
         * @brief Destroy the shader stage
         */
        ~shader_stage();

        /**
         * @brief Set the stage
         * 
         * @param stage    Shader stage flag bits
         */
        void set_stage(VkShaderStageFlagBits stage) {
            create_info.stage = stage;
        }

        /**
         * @brief Add specialization entry
         * 
         * @param specialization    Specialization map entry
         */
        void add_specialization_entry(VkSpecializationMapEntry const& specialization);

        /**
         * @brief Create a new shader stage
         * 
         * @param device                 Vulkan device
         * @param shader_data            Shader data
         * @param specialization_data    Specialization data
         * 
         * @return true                  Create was successful
         * @return false                 Create failed
         */
        bool create(device_ptr device, cdata const& shader_data, cdata const& specialization_data = data());

        /**
         * @brief Destroy the shader stage
         */
        void destroy();

        /**
         * @brief Get the create info
         * 
         * @return VkPipelineShaderStageCreateInfo const&    Pipeline shader stage create information
         */
        VkPipelineShaderStageCreateInfo const& get_create_info() const {
            return create_info;
        }

    private:
        /// Vulkan device
        device_ptr device = nullptr;

        /// Pipeline shader stage create information
        VkPipelineShaderStageCreateInfo create_info;

        /// Secialization information
        VkSpecializationInfo specialization_info;

        /// Map of specialization entries
        VkSpecializationMapEntries specialization_entries;

        /// Specialization data copy
        data specialization_data_copy;
    };

protected:
    /**
     * @brief Internal create a new pipeline
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    virtual bool create_internal() = 0;

    /**
     * @brief Internal destroy the pipeline
     */
    virtual void destroy_internal() = 0;

    /// Vulkan device
    device_ptr device = nullptr;

    /// Vulkan pipeline
    VkPipeline vk_pipeline = VK_NULL_HANDLE;

    /// Vulkan pipeline cache
    VkPipelineCache pipeline_cache = VK_NULL_HANDLE;

    /// Pipeline layout
    pipeline_layout::ptr layout;

private:
    /// Active state
    bool active = true;

    /// Auto bind state
    bool auto_bind_active = true;
};

/**
 * @brief Make a new pipline shader stage
 * 
 * @param stage                           Shader stage flag bits
 * @return pipeline::shader_stage::ptr    Shared pointer to shader stage
 */
pipeline::shader_stage::ptr make_pipeline_shader_stage(VkShaderStageFlagBits stage);

/**
 * @brief Create a new pipeline shader stage
 * 
 * @param device                          Vulkan device
 * @param data                            Shader data
 * @param stage                           Shader stage flag bits
 * @return pipeline::shader_stage::ptr    Shared pointer to pipeline shader stage 
 */
pipeline::shader_stage::ptr create_pipeline_shader_stage(device_ptr device, cdata const& data, VkShaderStageFlagBits stage);

} // namespace lava
