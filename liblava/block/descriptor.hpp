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

            explicit binding();

            VkDescriptorSetLayoutBinding get() const {
                return vk_binding;
            }

            void set(index index) {
                vk_binding.binding = index;
            }

            void set_type(VkDescriptorType descriptor_type) {
                vk_binding.descriptorType = descriptor_type;
            }

            void set_count(ui32 descriptor_count) {
                vk_binding.descriptorCount = descriptor_count;
            }

            void set_stage_flags(VkShaderStageFlags stage_flags) {
                vk_binding.stageFlags = stage_flags;
            }

            void set_samplers(VkSampler const* immutable_samplers) {
                vk_binding.pImmutableSamplers = immutable_samplers;
            }

        private:
            VkDescriptorSetLayoutBinding vk_binding;
        };

        using ptr = std::shared_ptr<descriptor>;
        using list = std::vector<ptr>;

        void add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags);

        void add(binding::ptr const& binding) {
            bindings.push_back(binding);
        }

        bool create(device_ptr device);
        void destroy();

        ui32 get_binding_count() const {
            return to_ui32(bindings.size());
        }
        binding::list const& get_bindings() {
            return bindings;
        }

        VkDescriptorSetLayout get() const {
            return layout;
        }

        VkDescriptorSet allocate_set();
        VkDescriptorSet allocate() {
            return allocate_set();
        }

        bool free_set(VkDescriptorSet descriptor_set);
        bool free(VkDescriptorSet descriptor_set) {
            return free_set(descriptor_set);
        }

        VkDescriptorSets allocate_sets(ui32 size);
        VkDescriptorSets allocate(ui32 size) {
            return allocate_sets(size);
        }

        bool free_sets(VkDescriptorSets const& descriptor_sets);
        bool free(VkDescriptorSets const& descriptor_sets) {
            return free_sets(descriptor_sets);
        }

    private:
        device_ptr device = nullptr;

        VkDescriptorSetLayout layout = 0;
        binding::list bindings;
    };

    inline descriptor::ptr make_descriptor() {
        return std::make_shared<descriptor>();
    }

    descriptor::binding::ptr make_descriptor_binding(index index);

} // namespace lava
