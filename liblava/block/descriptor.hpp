// file      : liblava/block/descriptor.hpp
// copyright : Copyright (c) 2018-present, Lava Block OÜ
// license   : MIT; see accompanying LICENSE file

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

struct descriptor : id_obj {

    struct binding : id_obj {

        using ptr = std::shared_ptr<binding>;
        using list = std::vector<ptr>;

        static ptr make(index index);

        explicit binding();

        VkDescriptorSetLayoutBinding get() const { return _binding; }

        void set(index index) { _binding.binding = index; }

        void set_type(VkDescriptorType descriptor_type) { _binding.descriptorType = descriptor_type; }

        void set_count(ui32 descriptor_count) { _binding.descriptorCount = descriptor_count; }

        void set_stage_flags(VkShaderStageFlags stage_flags) { _binding.stageFlags = stage_flags; }

        void set_samplers(VkSampler const* immutable_samplers) { _binding.pImmutableSamplers = immutable_samplers; }

    private:
        VkDescriptorSetLayoutBinding _binding;
    };

    using ptr = std::shared_ptr<descriptor>;
    using list = std::vector<ptr>;

    static ptr make() { return std::make_shared<descriptor>(); }

    void add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags);

    void add(binding::ptr const& binding) { bindings.push_back(binding); }

    bool create(device* device);
    void destroy();

    ui32 get_binding_count() const { return to_ui32(bindings.size()); }
    binding::list const& get_bindings() { return bindings; }

    VkDescriptorSetLayout get() const { return layout; }

    VkDescriptorSet allocate_set();
    bool free_set(VkDescriptorSet descriptor_set);

    VkDescriptorSets create_sets(ui32 size);
    bool free_sets(VkDescriptorSets const& descriptor_sets);

private:
    device* dev = nullptr;

    VkDescriptorSetLayout layout = nullptr;
    binding::list bindings;
};

} // lava
