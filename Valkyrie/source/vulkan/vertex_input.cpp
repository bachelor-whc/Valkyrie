#include "valkyrie/vulkan/vertex_input.h"
using namespace Vulkan;

void VertexInput::setBindingDescription(const uint32_t binding, const uint32_t size) {
	const auto& binding_exists = m_binding_set.find(binding);
	assert(binding_exists == m_binding_set.end());
	m_binding_set.insert(binding);
	VkVertexInputBindingDescription description = {};
	description.binding = binding;
	description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	description.stride = size;
	bindings.push_back(description);
}

void VertexInput::setAttributeDescription(const uint32_t binding, const uint32_t location, const VkFormat format, const uint32_t offset) {
	const auto& binding_exists = m_binding_set.find(binding);
	assert(binding_exists != m_binding_set.end());
	VkVertexInputAttributeDescription description = {};
	description.binding = binding;
	description.location = location;
	description.format = format;
	description.offset = offset;
	attributes.push_back(description);
}