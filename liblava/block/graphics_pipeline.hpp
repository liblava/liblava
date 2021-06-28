/**
 * @file         liblava/block/graphics_pipeline.hpp
 * @brief        Graphics pipeline
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/block/pipeline.hpp>

namespace lava {

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
     * @brief Sizing modes
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
     * @param device            Vulkan device
     * @param pipeline_cache    Pipeline cache
     */
    explicit graphics_pipeline(device_ptr device, VkPipelineCache pipeline_cache);

    /**
     * @brief Bind the pipeline
     * 
     * @param cmd_buf    Command buffer
     */
    void bind(VkCommandBuffer cmd_buf) override;

    /**
     * @brief Set the viewport and scissor
     * 
     * @param cmd_buf    Command buffer
     * @param size       Viewport and scissor size
     */
    void set_viewport_and_scissor(VkCommandBuffer cmd_buf, uv2 size);

    /**
     * @brief Set the render pass
     * 
     * @param pass    Render pass
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
     * @return VkRenderPass    Render pass
     */
    VkRenderPass get_render_pass() const {
        return render_pass;
    }

    /**
     * @brief Get the subpass
     * 
     * @return index    Index of subpass
     */
    index get_subpass() const {
        return subpass;
    }

    /**
     * @brief Set the subpass
     * 
     * @param value    Index of subpass
     */
    void set_subpass(index value) {
        subpass = value;
    }

    /**
     * @brief Create a new graphics pipeline
     * 
     * @param pass      Vulkan render pass
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create(VkRenderPass pass) {
        set(pass);

        return pipeline::create();
    }

    /**
     * @brief Set the vertex input binding
     * 
     * @param description    Vertex input binding description
     */
    void set_vertex_input_binding(VkVertexInputBindingDescription const& description);

    /**
     * @brief Set the vertex input bindings
     * 
     * @param descriptions    List of vertex input binding descriptions
     */
    void set_vertex_input_bindings(VkVertexInputBindingDescriptions const& descriptions);

    /**
     * @brief Set the vertex input attribute
     * 
     * @param attribute    Vertex input attribute description
     */
    void set_vertex_input_attribute(VkVertexInputAttributeDescription const& attribute);

    /**
     * @brief Set the vertex input attributes
     * 
     * @param attributes    List of vertex input attributes descriptions
     */
    void set_vertex_input_attributes(VkVertexInputAttributeDescriptions const& attributes);

    /**
     * @brief Set the input assembler's topology.
     *
     * @param topology Enum describing polygon primitives
     */
    void set_input_topology(VkPrimitiveTopology const& topology);

    /**
     * @brief Set the depth test and write
     * 
     * @param test_enable     Enable depth test
     * @param write_enable    Enable depth write
     */
    void set_depth_test_and_write(bool test_enable = true, bool write_enable = true);

    /**
     * @brief Set the depth compare operation
     * 
     * @param compare_op    Depth compare operation
     */
    void set_depth_compare_op(VkCompareOp compare_op);

    /**
     * @brief Set the rasterization cull mode
     * 
     * @param cull_mode    Cull mode flags
     */
    void set_rasterization_cull_mode(VkCullModeFlags cull_mode);

    /**
     * @brief Set the rasterization front face
     * 
     * @param front_face    Front face
     */
    void set_rasterization_front_face(VkFrontFace front_face);

    /**
     * @brief Set the rasterization polygon mode
     * 
     * @param polygon_mode    Polygon mode
     */
    void set_rasterization_polygon_mode(VkPolygonMode polygon_mode);

    /**
     * @brief Create a color blend attachment
     * 
     * @return VkPipelineColorBlendAttachmentState    Pipeline color blend attachment state
     */
    static VkPipelineColorBlendAttachmentState create_color_blend_attachment();

    /**
     * @brief Add color blend attachment
     * 
     * @param attachment    Pipeline color blend attachment state
     */
    void add_color_blend_attachment(VkPipelineColorBlendAttachmentState const& attachment = create_color_blend_attachment());

    /**
     * @brief Clear color blend attachment
     */
    void clear_color_blend_attachment();

    /**
     * @brief Set the dynamic states
     * 
     * @param states    List of dynamic states
     */
    void set_dynamic_states(VkDynamicStates const& states);

    /**
     * @brief Add a dynamic state
     * 
     * @param state    Dynamic state
     */
    void add_dynamic_state(VkDynamicState state);

    /**
     * @brief Clear dynamic states
     */
    void clear_dynamic_states();

    /**
     * @brief Add shader stage
     * 
     * @param data      Shader data
     * @param stage     Shader stage flag bits
     * 
     * @return true     Add was successful
     * @return false    Add failed
     */
    bool add_shader_stage(cdata const& data, VkShaderStageFlagBits stage);

    /**
     * @brief Add shader
     * 
     * @param data      Shader data
     * @param stage     Shader stage flag bits
     * 
     * @return true     Add was successful
     * @return false    Add failed
     */
    bool add_shader(cdata const& data, VkShaderStageFlagBits stage) {
        return add_shader_stage(data, stage);
    }

    /**
     * @brief Add shader stage
     * 
     * @param shader_stage    Shader stage
     */
    void add(shader_stage::ptr const& shader_stage) {
        shader_stages.push_back(shader_stage);
    }

    /**
     * @brief Get the shader stages
     * 
     * @return shader_stage::list const&    List of shader stages
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
     * @param value    Enable state
     */
    void set_auto_size(bool value = true) {
        auto_size = value;
    }

    /**
     * @brief Get the auto sizing state
     * 
     * @return true     Auto sizing is enabled
     * @return false    Auto sizing is disabled
     */
    bool auto_sizing() const {
        return auto_size;
    }

    /**
     * @brief Get the viewport
     * 
     * @return VkViewport    Vulkan viewport
     */
    VkViewport get_viewport() const {
        return viewport;
    }

    /**
     * @brief Set the viewport
     * 
     * @param value    Vulkan viewport
     */
    void set_viewport(VkViewport value) {
        viewport = value;
    }

    /**
     * @brief Get the scissor
     * 
     * @return VkRect2D    Scissor rectangle
     */
    VkRect2D get_scissor() const {
        return scissor;
    }

    /**
     * @brief Set the scissor
     * 
     * @param value    Scissor rectangle
     */
    void set_scissor(VkRect2D value) {
        scissor = value;
    }

    /**
     * @brief Get the sizing mode
     * 
     * @return sizing_mode    Sizing mode
     */
    sizing_mode get_sizing_mode() const {
        return sizing_mode;
    }

    /**
     * @brief Set the sizing mode
     * 
     * @param value    Sizing mode
     */
    void set_sizing_mode(sizing_mode value) {
        sizing_mode = value;
    }

    /**
     * @brief Copy pipeline configuration to target
     * 
     * @param target    Graphics pipeline
     */
    void copy_to(graphics_pipeline* target) const;

    /**
     * @brief Copy pipeline configuration from source
     * 
     * @param source    Graphics pipeline
     */
    void copy_from(ptr const& source) {
        source->copy_to(this);
    }

    /**
     * @brief Set the line width
     * 
     * @param value    Line width
     */
    void set_line_width(r32 value) {
        line_width = value;
    }

    /**
     * @brief Get the line width
     * 
     * @return r32    Line width
     */
    r32 get_line_width() const {
        return line_width;
    }

    /**
     * @brief Check if auto line width is active
     * 
     * @return true     Auto line width is enabled
     * @return false    Auto line width is disabled
     */
    bool auto_line_width() const {
        return auto_line_width_active;
    }

    /**
     * @brief Set the auto line width
     * 
     * @param value    Enable state
     */
    void set_auto_line_width(bool value = true) {
        auto_line_width_active = value;
    }

    /**
     * @brief Set the line width
     * 
     * @param cmd_buf    Command buffer
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
     * @return true     Create was successful
     * @return false    Create failed
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
 * @param device                     Vulkan device
 * @param pipeline_cache             Pipeline cache
 * 
 * @return graphics_pipeline::ptr    Shared pointer to graphics pipeline
 */
inline graphics_pipeline::ptr make_graphics_pipeline(device_ptr device, VkPipelineCache pipeline_cache = 0) {
    return std::make_shared<graphics_pipeline>(device, pipeline_cache);
}

} // namespace lava
