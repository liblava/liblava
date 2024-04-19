/**
 * @file         liblava/block/pipeline.hpp
 * @brief        Pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include "liblava/block/pipeline_layout.hpp"

namespace lava {

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
     * @param device            Vulkan device
     * @param pipeline_cache    Pipeline cache
     */
    explicit pipeline(device_p device,
                      VkPipelineCache pipeline_cache = 0);

    /**
     * @brief Destroy the pipeline
     */
    ~pipeline() override;

    /**
     * @brief Create a new pipeline
     * @return Create was successful or failed
     */
    bool create();

    /**
     * @brief Destroy the pipeline
     */
    void destroy();

    /**
     * @brief Bind the pipeline
     * @param cmd_buf    Command buffer
     */
    virtual void bind(VkCommandBuffer cmd_buf) = 0;

    /**
     * @brief Set pipeline active
     * @param value    Active state
     */
    void set_active(bool value = true) {
        active = value;
    }

    /**
     * @brief Check if pipeline is active
     * @return Pipeline is active or note
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
     * @param value    Enable state
     */
    void set_auto_bind(bool value = true) {
        auto_bind_active = value;
    }

    /**
     * @brief Check if auto bind is enabled
     * @return Auto bind is enabled or not
     */
    bool auto_bind() const {
        return auto_bind_active;
    }

    /**
     * @brief Check if pipeline is ready
     * @return Pipeline is ready or not
     */
    bool ready() const {
        return vk_pipeline != VK_NULL_HANDLE;
    }

    /**
     * @brief Get the pipeline
     * @return VkPipeline    Vulkan pipeline
     */
    VkPipeline get() const {
        return vk_pipeline;
    }

    /**
     * @brief Get the device
     * @return device_p    Vulkan device
     */
    device_p get_device() {
        return device;
    }

    /**
     * @brief Get the layout
     * @return pipeline_layout::ptr    Pipeline layout
     */
    pipeline_layout::ptr get_layout() const {
        return layout;
    }

    /**
     * @brief Set the layout
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
         * @brief Make a new pipline shader stage
         * @param stage    Shader stage flag bits
         * @return ptr     Shared pointer to shader stage
         */
        static ptr make(VkShaderStageFlagBits stage) {
            auto result = std::make_shared<shader_stage>();
            result->set_stage(stage);
            return result;
        }

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
         * @param stage    Shader stage flag bits
         */
        void set_stage(VkShaderStageFlagBits stage) {
            create_info.stage = stage;
        }

        /**
         * @brief Add specialization entry
         * @param specialization    Specialization map entry
         */
        void add_specialization_entry(VkSpecializationMapEntry const& specialization);

        /**
         * @brief Create a new shader stage
         * @param device                 Vulkan device
         * @param shader_data            Shader data
         * @param specialization_data    Specialization data
         * @return Create was successful or failed
         */
        bool create(device_p device,
                    c_data::ref shader_data,
                    c_data::ref specialization_data = data());

        /**
         * @brief Destroy the shader stage
         */
        void destroy();

        /**
         * @brief Get the create info
         * @return VkPipelineShaderStageCreateInfo const&    Pipeline shader stage create information
         */
        VkPipelineShaderStageCreateInfo const& get_create_info() const {
            return create_info;
        }

    private:
        /// Vulkan device
        device_p device = nullptr;

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
     * @brief Set up the pipeline
     * @return Setup was successful or failed
     */
    virtual bool setup() = 0;

    /**
     * @brief Tear down the pipeline
     */
    virtual void teardown() = 0;

    /// Vulkan device
    device_p device = nullptr;

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
 * @brief Create a new pipeline shader stage
 * @param device                          Vulkan device
 * @param data                            Shader data
 * @param stage                           Shader stage flag bits
 * @return pipeline::shader_stage::ptr    Shared pointer to pipeline shader stage
 */
pipeline::shader_stage::ptr create_pipeline_shader_stage(device_p device,
                                                         c_data::ref data,
                                                         VkShaderStageFlagBits stage);

} // namespace lava
