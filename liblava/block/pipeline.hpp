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
    using s_ptr = std::shared_ptr<pipeline>;

    /// List of pipelines
    using s_list = std::vector<s_ptr>;

    /// Pipeline process function
    using process_func = std::function<void(VkCommandBuffer)>;

    /// Called on pipeline process
    process_func on_process;

    /**
     * @brief Construct a new pipeline
     * @param device            Vulkan device
     * @param pipeline_cache    Pipeline cache
     */
    explicit pipeline(device::ptr device,
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
        m_active = value;
    }

    /**
     * @brief Check if pipeline is active
     * @return Pipeline is active or note
     */
    bool activated() const {
        return m_active;
    }

    /**
     * @brief Toggle activation
     */
    void toggle() {
        m_active = !m_active;
    }

    /**
     * @brief Set auto bind
     * @param value    Enable state
     */
    void set_auto_bind(bool value = true) {
        m_auto_bind_active = value;
    }

    /**
     * @brief Check if auto bind is enabled
     * @return Auto bind is enabled or not
     */
    bool auto_bind() const {
        return m_auto_bind_active;
    }

    /**
     * @brief Check if pipeline is ready
     * @return Pipeline is ready or not
     */
    bool ready() const {
        return m_vk_pipeline != VK_NULL_HANDLE;
    }

    /**
     * @brief Get the pipeline
     * @return VkPipeline    Vulkan pipeline
     */
    VkPipeline get() const {
        return m_vk_pipeline;
    }

    /**
     * @brief Get the device
     * @return device::ptr    Vulkan device
     */
    device::ptr get_device() {
        return m_device;
    }

    /**
     * @brief Get the layout
     * @return pipeline_layout::s_ptr    Pipeline layout
     */
    pipeline_layout::s_ptr get_layout() const {
        return m_layout;
    }

    /**
     * @brief Set the layout
     * @param value    Pipeline layout
     */
    void set_layout(pipeline_layout::s_ptr const& value) {
        m_layout = value;
    }

    /**
     * @brief Shader stage
     */
    struct shader_stage {
        /// Shared pointer to shader stage
        using s_ptr = std::shared_ptr<shader_stage>;

        /// List of shader stages
        using s_list = std::vector<s_ptr>;

        /**
         * @brief Make a new pipline shader stage
         * @param stage     Shader stage flag bits
         * @return s_ptr    Shared pointer to shader stage
         */
        static s_ptr make(VkShaderStageFlagBits stage) {
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
            m_create_info.stage = stage;
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
        bool create(device::ptr device,
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
            return m_create_info;
        }

    private:
        /// Vulkan device
        device::ptr m_device = nullptr;

        /// Pipeline shader stage create information
        VkPipelineShaderStageCreateInfo m_create_info;

        /// Secialization information
        VkSpecializationInfo m_specialization_info;

        /// Map of specialization entries
        VkSpecializationMapEntries m_specialization_entries;

        /// Specialization data copy
        data m_specialization_data_copy;
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
    device::ptr m_device = nullptr;

    /// Vulkan pipeline
    VkPipeline m_vk_pipeline = VK_NULL_HANDLE;

    /// Vulkan pipeline cache
    VkPipelineCache m_pipeline_cache = VK_NULL_HANDLE;

    /// Pipeline layout
    pipeline_layout::s_ptr m_layout;

private:
    /// Active state
    bool m_active = true;

    /// Auto bind state
    bool m_auto_bind_active = true;
};

/**
 * @brief Create a new pipeline shader stage
 * @param device                            Vulkan device
 * @param data                              Shader data
 * @param stage                             Shader stage flag bits
 * @return pipeline::shader_stage::s_ptr    Shared pointer to pipeline shader stage
 */
pipeline::shader_stage::s_ptr create_pipeline_shader_stage(device::ptr device,
                                                           c_data::ref data,
                                                           VkShaderStageFlagBits stage);

} // namespace lava
