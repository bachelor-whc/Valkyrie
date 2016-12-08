#ifndef _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#define _VULKAN_DESCRIPTOR_SET_LAYOUT_H
#include "utility.h"

namespace Vulkan {
	struct Device;

	struct DescriptorSetLayout {
		void setBinding(const uint32_t shader_binding, const VkDescriptorType type, const VkShaderStageFlags flag, const uint32_t count);
		VkResult initialize(const Device& device);

		VkDescriptorSetLayout handle = NULL;
	private:
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
	};

	struct DescriptorPool {
		DescriptorPool() = delete;
		DescriptorPool(uint32_t max_sets);
		~DescriptorPool();

		VkResult initializePool(const Device& device);
		VkResult initializeSet(const Device& device);
		void addPoolSize(VkDescriptorType type, uint32_t count);

		VkDescriptorPool handle = NULL;

		VkDescriptorSet set;
		DescriptorSetLayout setLayout;

	private:
		std::vector<VkDescriptorPoolSize> sizes;
		uint32_t m_max_sets;
	};
}

#endif