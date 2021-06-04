/**
 * @file liblava/block/pipeline.hpp
 * @brief Pipeline layout
 * @authors Lava Block OÜ and contributors
 * @copyright Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/base.hpp>
#include <liblava/block/descriptor.hpp>
#include <liblava/core/math.hpp>

namespace lava {

/**
 * @brief Pipeline layout
 */
struct pipeline_layout : id_obj {
    /// Shared pointer to pipeline layout
    using ptr = std::shared_ptr<pipeline_layout>;

    /// List of pipeline layouts
    using list = std::vector<ptr>;

    /**
     * @see add_descriptor
     */
    void add(descriptor::ptr const& descriptor) {
        descriptors.push_back(descriptor);
    }

    /**
     * @see add_push_constant_range
     */
    void add(VkPushConstantRange const& range) {
        push_constant_ranges.push_back(range);
    }

    /**
     * @brief Add descriptor
     * 
     * @param descriptor Descriptor
     */
    void add_descriptor(descriptor::ptr const& descriptor) {
        add(descriptor);
    }

    /**
     * @brief Add push contant range
     * 
     * @param range Push contant range
     */
    void add_push_constant_range(VkPushConstantRange const& range) {
        add(range);
    }

    /**
     * @brief Clear descriptors
     */
    void clear_descriptors() {
        descriptors.clear();
    }

    /**
     * @brief Clear push constant ranges
     */
    void clear_ranges() {
        push_constant_ranges.clear();
    }

    /**
     * @brief Clear descriptors and push constant ranges
     */
    void clear() {
        clear_descriptors();
        clear_ranges();
    }

    /**
     * @brief Create a new pipeline layout
     * 
     * @param device Vulkan device
     * @return true Create was successful
     * @return false Create failed
     */
    bool create(device_ptr device);

    /**
     * @brief Destroy the pipeline layout
     */
    void destroy();

    /**
     * @brief Get the Vulkan pipeline layout
     * 
     * @return VkPipelineLayout Pipeline layout
     */
    VkPipelineLayout get() const {
        return layout;
    }

    /**
     * @brief Get the device
     * 
     * @return device_ptr Vulkan device
     */
    device_ptr get_device() {
        return device;
    }

    /**
     * @brief Get the descriptors
     * 
     * @return descriptor::list const& List of descriptors
     */
    descriptor::list const& get_descriptors() const {
        return descriptors;
    }

    /**
     * @brief Get the push constant ranges
     * 
     * @return VkPushConstantRanges const& List of push constant ranges 
     */
    VkPushConstantRanges const& get_push_constant_ranges() const {
        return push_constant_ranges;
    }

    /// List of offsets
    using offset_list = std::vector<index>;

    /**
     * @brief Bind descriptor set
     * 
     * @param cmd_buf Command buffer
     * @param descriptor_set Descriptor set
     * @param first_set Index to first descriptor set
     * @param offsets List of offsets
     * @param bind_point Pipeline bind point
     */
    void bind_descriptor_set(VkCommandBuffer cmd_buf, VkDescriptorSet descriptor_set, index first_set = 0, offset_list offsets = {}, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS);

    /**
     * @see bind_descriptor_set 
     */
    void bind(VkCommandBuffer cmd_buf, VkDescriptorSet descriptor_set, index first_set = 0, offset_list offsets = {}, VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS) {
        bind_descriptor_set(cmd_buf, descriptor_set, first_set, offsets, bind_point);
    }

private:
    /// Vulkan device
    device_ptr device = nullptr;

    /// Pipeline layout
    VkPipelineLayout layout = VK_NULL_HANDLE;

    /// List of descriptors
    descriptor::list descriptors;

    /// List of push constant ranges
    VkPushConstantRanges push_constant_ranges;
};

/**
 * @brief Make a new pipeline layout
 * 
 * @return pipeline_layout::ptr Shared pointer to pipeline layout
 */
inline pipeline_layout::ptr make_pipeline_layout() {
    return std::make_shared<pipeline_layout>();
}

/// Shader main function name
constexpr name _main_ = "main";

/**
 * @brief Pipeline
 */
struct pipeline : id_obj {
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
     * @param device Vulkan device
     * @param pipeline_cache Pipeline cache
     */
    explicit pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0);

    /**
     * @brief Destroy the pipeline
     */
    ~pipeline() override;

    /**
     * @brief Create a new pipeline
     * 
     * @return true Create was successful
     * @return false Create failed
     */
    bool create();

    /**
     * @brief Destroy the pipeline
     */
    void destroy();

    /**
     * @brief Bind the pipeline
     * 
     * @param cmd_buf Command buffer
     */
    virtual void bind(VkCommandBuffer cmd_buf) = 0;

    /**
     * @brief Set pipeline active
     * 
     * @param value Active state
     */
    void set_active(bool value = true) {
        active = value;
    }

    /**
     * @brief Check if pipeline is active
     * 
     * @return true Pipeline is active
     * @return false Pipeline is inactive
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
     * @param value Enable state
     */
    void set_auto_bind(bool value = true) {
        auto_bind_active = value;
    }

    /**
     * @brief Check if auto bind is enabled
     * 
     * @return true Auto bind is enabled
     * @return false Audo bind is disabled
     */
    bool auto_bind() const {
        return auto_bind_active;
    }

    /**
     * @brief Check if pipeline is ready
     * 
     * @return true Pipeline is ready
     * @return false Pipeline is not ready
     */
    bool ready() const {
        return vk_pipeline != VK_NULL_HANDLE;
    }

    /**
     * @brief Get the pipeline
     * 
     * @return VkPipeline Vulkan pipeline
     */
    VkPipeline get() const {
        return vk_pipeline;
    }

    /**
     * @brief Get the device
     * 
     * @return device_ptr Vulkan device
     */
    device_ptr get_device() {
        return device;
    }

    /**
     * @brief Get the layout
     * 
     * @return pipeline_layout::ptr Pipeline layout
     */
    pipeline_layout::ptr get_layout() const {
        return layout;
    }

    /**
     * @brief Set the layout
     * 
     * @param value Pipeline layout
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
         * @param stage Shader stage flag bits
         */
        void set_stage(VkShaderStageFlagBits stage) {
            create_info.stage = stage;
        }

        /**
         * @brief Add specialization entry
         * 
         * @param specialization Specialization map entry
         */
        void add_specialization_entry(VkSpecializationMapEntry const& specialization);

        /**
         * @brief Create a new shader stage
         * 
         * @param device Vulkan device
         * @param shader_data Shader data
         * @param specialization_data Specialization data
         * @return true Create was successful
         * @return false Create failed
         */
        bool create(device_ptr device, cdata const& shader_data, cdata const& specialization_data = data());

        /**
         * @brief Destroy the shader stage
         */
        void destroy();

        /**
         * @brief Get the create info
         * 
         * @return VkPipelineShaderStageCreateInfo const& Pipeline shader stage create information
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
     * @return true Create was successful
     * @return false Create failed
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
 * @param stage Shader stage flag bits
 * @return pipeline::shader_stage::ptr Shared pointer to shader stage
 */
pipeline::shader_stage::ptr make_pipeline_shader_stage(VkShaderStageFlagBits stage);

/**
 * @brief Create a new pipeline shader stage
 * 
 * @param device Vulkan device
 * @param data Shader data
 * @param stage Shader stage flag bits
 * @return pipeline::shader_stage::ptr Shared pointer to pipeline shader stage 
 */
pipeline::shader_stage::ptr create_pipeline_shader_stage(device_ptr device, cdata const& data, VkShaderStageFlagBits stage);

/**
 * @brief Graphics pipeline
 */
struct graphics_pipeline : pipeline {
    /// Shared pointer to graphics pipeline
    using ptr = std::shared_ptr<graphics_pipeline>;

    /// Map of graphics pipelines
    using map = std::map<id, ptr>;

    /// List of graphics pipelines
    using list = std::vector<ptr>;

    /**
     * @brief Sizing mode
     */
    enum class sizing_mode : type {
        input = 0,
        absolute,
        relative
    };

    /**
     * @brief Graphics pipeline create information
     */
    struct create_info {
        /// Vertex input stage
        VkPipelineVertexInputStateCreateInfo vertex_input_state;

        /// Input assembly state
        VkPipelineInputAssemblyStateCreateInfo input_assembly_state;

        /// Viewport state
        VkPipelineViewportStateCreateInfo viewport_state;

        /// Multisample state
        VkPipelineMultisampleStateCreateInfo multisample_state;

        /// Depth stencil state
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state;

        /// Rasterization state
        VkPipelineRasterizationStateCreateInfo rasterization_state;
    };

    /**
     * @brief Construct a new graphics pipeline
     * 
     * @param device Vulkan device
     * @param pipeline_cache Pipeline cache
     */
    explicit graphics_pipeline(device_ptr device, VkPipelineCache pipeline_cache);

    /**
     * @brief Bind the pipeline
     * 
     * @param cmd_buf Command buffer
     */
    void bind(VkCommandBuffer cmd_buf) override;

    /**
     * @brief Set the viewport and scissor
     * 
     * @param cmd_buf Command buffer
     * @param size Viewport and scissor size
     */
    void set_viewport_and_scissor(VkCommandBuffer cmd_buf, uv2 size);

    /**
     * @brief Set the render pass
     * 
     * @param pass Render pass
     */
    void set_render_pass(VkRenderPass pass) {
        render_pass = pass;
    }

    /**
     * @see set_render_pass
     */
    void set(VkRenderPass pass) {
        set_render_pass(pass);
    }

    /**
     * @brief Get the render pass
     * 
     * @return VkRenderPass Render pass
     */
    VkRenderPass get_render_pass() const {
        return render_pass;
    }

    /**
     * @brief Get the subpass
     * 
     * @return index Index of subpass
     */
    index get_subpass() const {
        return subpass;
    }

    /**
     * @brief Set the subpass
     * 
     * @param value Index of subpass
     */
    void set_subpass(index value) {
        subpass = value;
    }

    /**
     * @brief Create a new graphics pipeline
     * 
     * @param pass Vulkan render pass
     * @return true Create was successful
     * @return false Create failed
     */
    bool create(VkRenderPass pass) {
        set(pass);

        return pipeline::create();
    }

    /**
     * @brief Set the vertex input binding
     * 
     * @param description Vertex input binding description
     */
    void set_vertex_input_binding(VkVertexInputBindingDescription const& description);

    /**
     * @brief Set the vertex input bindings
     * 
     * @param descriptions List of vertex input binding descriptions
     */
    void set_vertex_input_bindings(VkVertexInputBindingDescriptions const& descriptions);

    /**
     * @brief Set the vertex input attribute
     * 
     * @param attribute Vertex input attribute description
     */
    void set_vertex_input_attribute(VkVertexInputAttributeDescription const& attribute);

    /**
     * @brief Set the vertex input attributes
     * 
     * @param attributes List of vertex input attributes descriptions
     */
    void set_vertex_input_attributes(VkVertexInputAttributeDescriptions const& attributes);

    /**
     * @brief Set the depth test and write
     * 
     * @param test_enable Enable depth test
     * @param write_enable Enable depth write
     */
    void set_depth_test_and_write(bool test_enable = true, bool write_enable = true);

    /**
     * @brief Set the depth compare operation
     * 
     * @param compare_op Depth compare operation
     */
    void set_depth_compare_op(VkCompareOp compare_op);

    /**
     * @brief Set the rasterization cull mode
     * 
     * @param cull_mode Cull mode flags
     */
    void set_rasterization_cull_mode(VkCullModeFlags cull_mode);

    /**
     * @brief Set the rasterization front face
     * 
     * @param front_face Front face
     */
    void set_rasterization_front_face(VkFrontFace front_face);

    /**
     * @brief Set the rasterization polygon mode
     * 
     * @param polygon_mode Polygon mode
     */
    void set_rasterization_polygon_mode(VkPolygonMode polygon_mode);

    /**
     * @brief Create a color blend attachment
     * 
     * @return VkPipelineColorBlendAttachmentState Pipeline color blend attachment state
     */
    static VkPipelineColorBlendAttachmentState create_color_blend_attachment();

    /**
     * @brief Add color blend attachment
     * 
     * @param attachment Pipeline color blend attachment state
     */
    void add_color_blend_attachment(VkPipelineColorBlendAttachmentState const& attachment = create_color_blend_attachment());

    /**
     * @brief Clear color blend attachment
     */
    void clear_color_blend_attachment();

    /**
     * @brief Set the dynamic states
     * 
     * @param states List of dynamic states
     */
    void set_dynamic_states(VkDynamicStates const& states);

    /**
     * @brief Add a dynamic state
     * 
     * @param state Dynamic state
     */
    void add_dynamic_state(VkDynamicState state);

    /**
     * @brief Clear dynamic states
     */
    void clear_dynamic_states();

    /**
     * @brief Add shader stage
     * 
     * @param data Shader data
     * @param stage Shader stage flag bits
     * @return true Add was successful
     * @return false Add failed
     */
    bool add_shader_stage(cdata const& data, VkShaderStageFlagBits stage);

    /**
     * @brief Add shader
     * 
     * @param data Shader data
     * @param stage Shader stage flag bits
     * @return true Add was successful
     * @return false Add failed
     */
    bool add_shader(cdata const& data, VkShaderStageFlagBits stage) {
        return add_shader_stage(data, stage);
    }

    /**
     * @brief Add shader stage
     * 
     * @param shader_stage Shader stage
     */
    void add(shader_stage::ptr const& shader_stage) {
        shader_stages.push_back(shader_stage);
    }

    /**
     * @brief Get the shader stages
     * 
     * @return shader_stage::list const& List of shader stages
     */
    shader_stage::list const& get_shader_stages() const {
        return shader_stages;
    }

    /**
     * @brief Clear the shader stages
     */
    void clear_shader_stages() {
        shader_stages.clear();
    }

    /**
     * @brief Clear the graphics pipeline
     */
    void clear() {
        clear_color_blend_attachment();
        clear_shader_stages();
    }

    /**
     * @brief Set the auto size
     * 
     * @param value Enable state
     */
    void set_auto_size(bool value = true) {
        auto_size = value;
    }

    /**
     * @brief Get the auto sizing state
     * 
     * @return true Auto sizing is enabled
     * @return false Auto sizing is disabled
     */
    bool auto_sizing() const {
        return auto_size;
    }

    /**
     * @brief Get the viewport
     * 
     * @return VkViewport Vulkan viewport
     */
    VkViewport get_viewport() const {
        return viewport;
    }

    /**
     * @brief Set the viewport
     * 
     * @param value Vulkan viewport
     */
    void set_viewport(VkViewport value) {
        viewport = value;
    }

    /**
     * @brief Get the scissor
     * 
     * @return VkRect2D Scissor rectangle
     */
    VkRect2D get_scissor() const {
        return scissor;
    }

    /**
     * @brief Set the scissor
     * 
     * @param value Scissor rectangle
     */
    void set_scissor(VkRect2D value) {
        scissor = value;
    }

    /**
     * @brief Get the sizing mode
     * 
     * @return sizing_mode Sizing mode
     */
    sizing_mode get_sizing_mode() const {
        return sizing_mode;
    }

    /**
     * @brief Set the sizing mode
     * 
     * @param value Sizing mode
     */
    void set_sizing_mode(sizing_mode value) {
        sizing_mode = value;
    }

    /**
     * @brief Copy pipeline configuration to target
     * 
     * @param target Graphics pipeline
     */
    void copy_to(graphics_pipeline* target) const;

    /**
     * @brief Copy pipeline configuration from source
     * 
     * @param source Graphics pipeline
     */
    void copy_from(ptr const& source) {
        source->copy_to(this);
    }

    /**
     * @brief Set the line width
     * 
     * @param value Line width
     */
    void set_line_width(r32 value) {
        line_width = value;
    }

    /**
     * @brief Get the line width
     * 
     * @return r32 Line width
     */
    r32 get_line_width() const {
        return line_width;
    }

    /**
     * @brief Check if auto line width is active
     * 
     * @return true Auto line width is enabled
     * @return false Auto line width is disabled
     */
    bool auto_line_width() const {
        return auto_line_width_active;
    }

    /**
     * @brief Set the auto line width
     * 
     * @param value Enable state
     */
    void set_auto_line_width(bool value = true) {
        auto_line_width_active = value;
    }

    /**
     * @brief Set the line width
     * 
     * @param cmd_buf Command buffer
     */
    void set_line_width(VkCommandBuffer cmd_buf) {
        vkCmdSetLineWidth(cmd_buf, line_width);
    }

    /// Create function
    using create_func = std::function<bool(create_info&)>;

    /// Called on graphics pipeline create
    create_func on_create;

private:
    /**
     * @brief Internal create a new graphics pipeline
     * 
     * @return true Create was successful
     * @return false Create failed
     */
    bool create_internal() override;

    /**
     * @brief Internal destroy the graphics pipeline
     */
    void destroy_internal() override;

    /// Vulkan render pass
    VkRenderPass render_pass = VK_NULL_HANDLE;

    /// Subpass index
    index subpass = 0;

    /// Create information
    create_info info;

    /// List of vertex input binding descriptions
    VkVertexInputBindingDescriptions vertex_input_bindings;

    /// List of vertex input attribute descriptions
    VkVertexInputAttributeDescriptions vertex_input_attributes;

    /// List of pipeline color blend attachment states
    VkPipelineColorBlendAttachmentStates color_blend_attachment_states;

    /// Pipeline color blend state create information
    VkPipelineColorBlendStateCreateInfo color_blend_state;

    /// Pipeline dynamic state create information
    VkPipelineDynamicStateCreateInfo dynamic_state;

    /// List of dynamic states
    VkDynamicStates dynamic_states;

    /// List of shader stages
    shader_stage::list shader_stages;

    /// Sizing mode
    graphics_pipeline::sizing_mode sizing_mode = sizing_mode::input;

    /// Vulkan viewport
    VkViewport viewport;

    /// Scissor rectangle
    VkRect2D scissor;

    /// Auto size
    bool auto_size = true;

    /// Auto line width
    bool auto_line_width_active = false;

    /// Line width
    r32 line_width = 1.f;
};

/**
 * @brief Make a new graphics pipeline
 * 
 * @param device Vulkan device
 * @param pipeline_cache Pipeline cache
 * @return graphics_pipeline::ptr Shared pointer to graphics pipeline
 */
inline graphics_pipeline::ptr make_graphics_pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0) {
    return std::make_shared<graphics_pipeline>(device, pipeline_cache);
}

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
     * @param cmdBuffer Command buffer
     */
    void bind(VkCommandBuffer cmdBuffer) override;

    /**
     * @brief Set the shader stage
     * 
     * @param data Shader data
     * @param stage Shader stage flag bits
     * @return true Set was successful
     * @return false Set failed
     */
    bool set_shader_stage(cdata const& data, VkShaderStageFlagBits stage);

    /**
     * @brief Set shader stage
     * 
     * @param stage Shader state
     */
    void set(shader_stage::ptr const& stage) {
        shader_stage = stage;
    }

    /**
     * @brief Get the shader stage
     * 
     * @return shader_stage::ptr const& Shader state
     */
    shader_stage::ptr const& get_shader_stage() const {
        return shader_stage;
    }

    /**
     * @brief Copy configuration to target pipeline
     * 
     * @param target Compute pipeline
     */
    void copy_to(compute_pipeline* target) const;

    /**
     * @brief Copy configuration from source
     * 
     * @param source Compute pipeline
     */
    void copy_from(ptr const& source) {
        source->copy_to(this);
    }

private:
    /**
     * @brief Internal create a new compute pipeline
     * 
     * @return true Create was successful
     * @return false Create failed
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
 * @param device Vulkan device
 * @param pipeline_cache Pipeline cache
 * @return compute_pipeline::ptr Shared pointer to compute pipeline
 */
inline compute_pipeline::ptr make_compute_pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0) {
    return std::make_shared<compute_pipeline>(device, pipeline_cache);
}

} // namespace lava
