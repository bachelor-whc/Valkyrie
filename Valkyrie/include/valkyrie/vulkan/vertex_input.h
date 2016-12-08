#ifndef _VULKAN_VERTEX_INPUT_H
#define _VULKAN_VERTEX_INPUT_H
#include "utility.h"

namespace Vulkan {
	struct VertexInput {
		void setBindingDescription(const uint32_t binding, const size_t size);
		void setAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const size_t offset);

		std::vector<VkVertexInputBindingDescription> bindings;
		std::vector<VkVertexInputAttributeDescription> attributes;
	private:
		std::unordered_set<uint32_t> m_binding_set;
	};
}

#endif