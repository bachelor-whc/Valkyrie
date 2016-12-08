#include "valkyrie/vulkan/descriptor.h"
#include "valkyrie/vulkan/device.h"
using namespace Vulkan;

DescriptorPool::DescriptorPool(uint32_t max_sets) : m_max_sets(max_sets) {

}

DescriptorPool::~DescriptorPool() {

}


VkResult DescriptorPool::initializePool(const Device& device) {
	VkDescriptorPoolCreateInfo descriptor_pool_create = {};
	descriptor_pool_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_create.maxSets = m_max_sets;
	descriptor_pool_create.poolSizeCount = sizes.size();
	descriptor_pool_create.pPoolSizes = sizes.data();
	return vkCreateDescriptorPool(device.handle, &descriptor_pool_create, nullptr, &handle);
	
}

VkResult DescriptorPool::initializeSet(const Device& device) {
	VkDescriptorSetAllocateInfo descriptor_set_allocate = {};
	descriptor_set_allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptor_set_allocate.descriptorPool = handle;
	descriptor_set_allocate.descriptorSetCount = 1;
	descriptor_set_allocate.pSetLayouts = &setLayout.handle;
	VkResult result = vkAllocateDescriptorSets(device.handle, &descriptor_set_allocate, &set);
	return result;
}

void DescriptorPool::addPoolSize(VkDescriptorType type, uint32_t count) {
	VkDescriptorPoolSize pool_size = {};
	pool_size.type = type;
	pool_size.descriptorCount = count;
	sizes.push_back(pool_size);
}


void DescriptorSetLayout::setBinding(const uint32_t shader_binding, const VkDescriptorType type, const VkShaderStageFlags flag, const uint32_t count) {
	VkDescriptorSetLayoutBinding binding = {};
	binding.binding = shader_binding;
	binding.descriptorType = type;
	binding.stageFlags = flag;
	binding.descriptorCount = count;
	binding.pImmutableSamplers = nullptr;
	m_bindings.push_back(binding);
}

VkResult DescriptorSetLayout::initialize(const Device& device) {
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create = {};
	descriptor_set_layout_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create.bindingCount = m_bindings.size();
	descriptor_set_layout_create.pBindings = m_bindings.data();
	return vkCreateDescriptorSetLayout(device.handle, &descriptor_set_layout_create, nullptr, &handle);
}