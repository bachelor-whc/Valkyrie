#include "valkyrie/vulkan/texture.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/vulkan/default_create_info.h"
using namespace Vulkan;

Texture::Texture(const Valkyrie::ImageMemoryPointer& image_ptr) : m_image_ptr(image_ptr) {
	m_size = m_image_ptr->getSize();
}

Texture::~Texture() {
	if (mp_information != nullptr)
		delete mp_information;
}

VkImageCreateInfo Texture::getImageCreate() const {
	VkImageCreateInfo create = VK_DEFAULT_TEXTURE_IMAGE_CREATE_INFO;
	create.extent.width = m_image_ptr->getWidth();
	create.extent.height = m_image_ptr->getHeight();
	return create;
}

VkImageViewCreateInfo Texture::getImageViewCreate() const {
	VkImageViewCreateInfo create = VK_DEFAULT_TEXTURE_IMAGE_VIEW_CREATE_INFO;
	create.image = handle;
	return create;
}

VkFlags Vulkan::Texture::getMemoryType() const {
	return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
}

VkResult Texture::initializeSampler() {
	VkSamplerCreateInfo sampler_create = {};
	sampler_create.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create.magFilter = VK_FILTER_NEAREST;
	sampler_create.minFilter = VK_FILTER_NEAREST;
	sampler_create.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
	sampler_create.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create.mipLodBias = 0.0f;
	sampler_create.anisotropyEnable = VK_FALSE;
	sampler_create.maxAnisotropy = 1;
	sampler_create.compareOp = VK_COMPARE_OP_NEVER;
	sampler_create.minLod = -1000.0f;
	sampler_create.maxLod = 1000.0f;
	sampler_create.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler_create.unnormalizedCoordinates = VK_FALSE;
	return vkCreateSampler(Valkyrie::VulkanManager::getDevice(), &sampler_create, nullptr, &sampler);
}

VkDescriptorImageInfo* Texture::getInformationPointer() {
	assert(sampler != VK_NULL_HANDLE);
	assert(view != VK_NULL_HANDLE);
	assert(layout != VK_NULL_HANDLE);
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorImageInfo;
		mp_information->sampler = sampler;
		mp_information->imageView = view;
		mp_information->imageLayout = layout;
	}
	return mp_information;
}

VkResult Texture::write() {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	assert(m_image_ptr->ready());
	assert(handle != VK_NULL_HANDLE);
	assert(memory != VK_NULL_HANDLE);
	VkResult result;
	VkImageSubresource subresource = {};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkSubresourceLayout subresource_layout = {};

	vkGetImageSubresourceLayout(device, handle, &subresource, &subresource_layout);
	void* destination;
	result = vkMapMemory(device, memory, 0, m_size, 0, &destination);
	assert(result == VK_SUCCESS);
	memcpy(destination, m_image_ptr->getData(), m_image_ptr->getSize());
	vkUnmapMemory(device, memory);
	return result;
}

VkWriteDescriptorSet Vulkan::Texture::getWriteSet() {
	VkWriteDescriptorSet write = {};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.pImageInfo = getInformationPointer();
	return write;
}
