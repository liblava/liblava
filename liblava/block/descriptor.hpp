/**
 * @file         liblava/block/descriptor.hpp
 * @brief        Descriptor definition
 * @authors      Lava Block OÜ and contributors
 * @copyright    Copyright (c) 2018-present, MIT License
 */

#pragma once

#include <liblava/base/device.hpp>

namespace lava {

/**
 * @brief Descriptor
 */
struct descriptor : entity {
    /**
     * @brief Descriptor binding
     */
    struct binding {
        /// Shared pointer to binding
        using ptr = std::shared_ptr<binding>;

        /// List of bindings
        using list = std::vector<ptr>;

        /**
         * @brief Construct a new binding
         */
        explicit binding();

        /**
         * @brief Get the Vulkan descriptor set layout binding
         * 
         * @return VkDescriptorSetLayoutBinding const&    Vulkan binding
         */
        VkDescriptorSetLayoutBinding const& get() const {
            return vk_binding;
        }

        /**
         * @brief Det the binding index
         * 
         * @param index    Binding index
         */
        void set(index index) {
            vk_binding.binding = index;
        }

        /**
         * @brief Set the type
         * 
         * @param descriptor_type    Descriptor type
         */
        void set_type(VkDescriptorType descriptor_type) {
            vk_binding.descriptorType = descriptor_type;
        }

        /**
         * @brief Set the count
         * 
         * @param descriptor_count    Descriptor count
         */
        void set_count(ui32 descriptor_count) {
            vk_binding.descriptorCount = descriptor_count;
        }

        /**
         * @brief Set the stage flags
         * 
         * @param stage_flags    Shader stage flags
         */
        void set_stage_flags(VkShaderStageFlags stage_flags) {
            vk_binding.stageFlags = stage_flags;
        }

        /**
         * @brief Set the samplers
         * 
         * @param immutable_samplers    Pointer to immutable samplers
         */
        void set_samplers(VkSampler const* immutable_samplers) {
            vk_binding.pImmutableSamplers = immutable_samplers;
        }

    private:
        /// Vulkan binding
        VkDescriptorSetLayoutBinding vk_binding;
    };

    /**
     * @brief Descriptor pool
     */
    struct pool : entity {
        /// Shared pointer to pool
        using ptr = std::shared_ptr<pool>;

        /// List of pools
        using list = std::vector<ptr>;

        /**
         * @brief Create a new pool
         * 
         * @param device    Vulkan device
         * @param sizes     Descriptor pool sizes
         * @param max       Number of pools
         * @param flags     Create flags
         * 
         * @return true     Create was successful
         * @return false    Create failed
         */
        bool create(device_ptr device, VkDescriptorPoolSizesRef sizes, ui32 max = 0,
                    VkDescriptorPoolCreateFlags flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

        /**
         * @brief Destroy the pool
         */
        void destroy();

        /**
         * @brief Get the descriptor pool
         * 
         * @return VkDescriptorPool    Vulkan descriptor pool
         */
        VkDescriptorPool get() const {
            return vk_pool;
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
         * @brief Get the sizes
         * 
         * @return VkDescriptorPoolSizes const&    Descriptor pool sizes
         */
        VkDescriptorPoolSizes const& get_sizes() const {
            return sizes;
        }

        /**
         * @brief Get the max
         * 
         * @return ui32    Number of pools
         */
        ui32 get_max() const {
            return max;
        }

    private:
        /// Vulkan device
        device_ptr device = nullptr;

        /// Vulkan descriptor pool
        VkDescriptorPool vk_pool = VK_NULL_HANDLE;

        /// Descritpr pool sizes
        VkDescriptorPoolSizes sizes;

        /// Number of pools
        ui32 max = 0;
    };

    /// Shared pointer to descriptor
    using ptr = std::shared_ptr<descriptor>;

    /// List of descriptors
    using list = std::vector<ptr>;

    /**
     * @brief Add binding
     * 
     * @param binding            Index of binding
     * @param descriptor_type    Descriptor type
     * @param stage_flags        Shader stage flags
     */
    void add_binding(index binding, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags);

    /**
     * @brief Clear bindings
     */
    void clear_bindings() {
        bindings.clear();
    }

    /**
     * @brief Add binding
     * 
     * @param binding    Descriptor binding
     */
    void add(binding::ptr const& binding) {
        bindings.push_back(binding);
    }

    /**
     * @brief Create a new descriptor
     * 
     * @param device    Vulkan device
     * 
     * @return true     Create was successful
     * @return false    Create failed
     */
    bool create(device_ptr device);

    /**
     * @brief Destroy the descriptor
     */
    void destroy();

    /**
     * @brief Get the binding count
     * 
     * @return ui32    Number of bindings
     */
    ui32 get_binding_count() const {
        return to_ui32(bindings.size());
    }

    /**
     * @brief Get the bindings
     * 
     * @return binding::list const&    List of bindings
     */
    binding::list const& get_bindings() {
        return bindings;
    }

    /**
     * @brief Get descriptor set layout
     * 
     * @return VkDescriptorSetLayout    Vulkan descriptor set layout
     */
    VkDescriptorSetLayout get() const {
        return layout;
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
     * @brief Allocate descriptor set
     * 
     * @param pool                Descriptor pool
     * @return VkDescriptorSet    Descriptor set
     */
    VkDescriptorSet allocate_set(VkDescriptorPool pool);

    /**
     * @see allocate_set
     */
    VkDescriptorSet allocate(VkDescriptorPool pool) {
        return allocate_set(pool);
    }

    /**
     * @brief Free descriptor set
     * 
     * @param descriptor_set    Descriptor set
     * @param pool              Descriptor pool
     * 
     * @return true             Free was successful
     * @return false            Free failed
     */
    bool free_set(VkDescriptorSet& descriptor_set, VkDescriptorPool pool);

    /**
     * @see free_set
     */
    bool free(VkDescriptorSet& descriptor_set, VkDescriptorPool pool) {
        return free_set(descriptor_set, pool);
    }

    /**
     * @brief Allocate descriptor sets
     * 
     * @param size                 Number of sets
     * @param pool                 Descriptor pool
     * 
     * @return VkDescriptorSets    List of descriptor sets
     */
    VkDescriptorSets allocate_sets(ui32 size, VkDescriptorPool pool);

    /**
     * @see allocate_sets
     */
    VkDescriptorSets allocate(ui32 size, VkDescriptorPool pool) {
        return allocate_sets(size, pool);
    }

    /**
     * @brief Free descriptor sets
     * 
     * @param descriptor_sets    List of descriptor sets
     * @param pool               Descriptor pool
     * 
     * @return true              Free was successful
     * @return false             Free failed
     */
    bool free_sets(VkDescriptorSets& descriptor_sets, VkDescriptorPool pool);

    /**
     * @see free_sets
     */
    bool free(VkDescriptorSets& descriptor_sets, VkDescriptorPool pool) {
        return free_sets(descriptor_sets, pool);
    }

private:
    /// Vulkan device
    device_ptr device = nullptr;

    /// Vulkan descriptor set layout
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    /// List of descriptor bindings
    binding::list bindings;
};

/**
 * @brief Make a new descriptor
 * 
 * @return descriptor::ptr    Shared pointer to descriptor
 */
inline descriptor::ptr make_descriptor() {
    return std::make_shared<descriptor>();
}

/**
 * @brief Make a new descriptor binding
 * 
 * @param index                        Binding index
 * 
 * @return descriptor::binding::ptr    Shared pointer to descriptor binding
 */
descriptor::binding::ptr make_descriptor_binding(index index);

/**
 * @brief Make a new descriptor pool
 * 
 * @return descriptor::pool::ptr    Shared pointer to descriptor pool
 */
inline descriptor::pool::ptr make_descriptor_pool() {
    return std::make_shared<descriptor::pool>();
}

} // namespace lava
