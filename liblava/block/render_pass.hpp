// file      : liblava/block/render_pass.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/block/attachment.hpp>
#include <liblava/block/subpass.hpp>
#include <liblava/base/device.hpp>

namespace lava {

struct render_pass : id_obj, target_callback {

    using ptr = std::shared_ptr<render_pass>;
    using list = std::vector<ptr>;

    explicit render_pass(device_ptr device);

    bool create(VkAttachmentsRef target_attachments, rect area);
    void destroy();

    void process(VkCommandBuffer cmd_buf, index frame);

    device_ptr get_device() { return device; }
    VkRenderPass get() const { return vk_render_pass; }

    ui32 get_subpass_count() const { return to_ui32(subpasses.size()); }
    bool exists_subpass(index index = 0) const { return index < subpasses.size(); }

    subpass* get_subpass(index index = 0) { return subpasses.at(index).get(); }
    subpass::list const& get_subpasses() const { return subpasses; }

    void add(attachment::ptr const& attachment) { attachments.push_back(attachment); }
    void add(subpass_dependency::ptr const& dependency) { dependencies.push_back(dependency); }
    void add(subpass::ptr const& subpass) { subpasses.push_back(subpass); }

    void set_clear_values(VkClearValues const& values) { clear_values = values; }
    VkClearValues const& get_clear_values() const { return clear_values; }

    void set_clear_color(v3 value = v3(0.086f, 0.086f, 0.094f));
    v3 get_clear_color() const;

    void add(graphics_pipeline::ptr pipeline, index subpass = 0) { subpasses.at(subpass)->add(pipeline); }
    void add_front(graphics_pipeline::ptr pipeline, index subpass = 0) { subpasses.at(subpass)->add_front(pipeline); }
    void remove(graphics_pipeline::ptr pipeline, index subpass = 0) { subpasses.at(subpass)->remove(pipeline); }

private:
    device_ptr device = nullptr;

    VkRenderPass vk_render_pass = 0;
    VkFramebuffers framebuffers = {};

    attachment::list attachments;
    subpass_dependency::list dependencies;
    subpass::list subpasses;

    VkClearValues clear_values = {};
    rect area;

    void begin(VkCommandBuffer cmd_buf, index frame);
    void end(VkCommandBuffer cmd_buf);

    bool on_target_created(VkAttachmentsRef target_attachments, rect area);
    void on_target_destroyed();
};

inline render_pass::ptr make_render_pass(device_ptr device) { return std::make_shared<render_pass>(device); }

} // lava
