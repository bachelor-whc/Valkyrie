#include "valkyrie/vulkan/texture.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/physical_device.h"

Vulkan::Texture::Texture(const ValkyriePNGPointer& texture_ptr) : 
	mp_png(texture_ptr),
	m_size(0),
	mp_information(nullptr) {

}

Vulkan::Texture::~Texture() {
	
}

VkResult Vulkan::Texture::initializeImage(const Device& device) {
	assert(mp_png->available());
	VkImageCreateInfo image_create = {};
	image_create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create.imageType = VK_IMAGE_TYPE_2D;
	image_create.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_create.extent.depth = 1;
	image_create.extent.width = mp_png->getWidth();
	image_create.extent.height = mp_png->getHeight();
	image_create.mipLevels = 1;
	image_create.arrayLayers = 1;
	image_create.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create.tiling = VK_IMAGE_TILING_LINEAR;
	image_create.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	image_create.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	return vkCreateImage(device.handle, &image_create, nullptr, &image);
}


VkResult Vulkan::Texture::initializeSampler(const Device& device) {
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
	sampler_create.minLod = 0.0f;
	sampler_create.maxLod = 0.0f;
	sampler_create.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler_create.unnormalizedCoordinates = VK_FALSE;
	return vkCreateSampler(device.handle, &sampler_create, nullptr, &sampler);
}

VkResult Vulkan::Texture::initializeView(const Device& device) {
	assert(image != NULL);
	VkImageViewCreateInfo image_view_create = {};
	image_view_create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_create.image = image;
	image_view_create.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_view_create.components.r = VK_COMPONENT_SWIZZLE_R;
	image_view_create.components.g = VK_COMPONENT_SWIZZLE_G;
	image_view_create.components.b = VK_COMPONENT_SWIZZLE_B;
	image_view_create.components.a = VK_COMPONENT_SWIZZLE_A;
	image_view_create.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	image_view_create.subresourceRange.baseMipLevel = 0;
	image_view_create.subresourceRange.baseArrayLayer = 0;
	image_view_create.subresourceRange.layerCount = 1;
	image_view_create.subresourceRange.levelCount = 1;
	return vkCreateImageView(device.handle, &image_view_create, nullptr, &view);
}

VkResult Vulkan::Texture::allocate(const Device& device, PhysicalDevice& physical_device) {
	assert(mp_png->available());
	assert(image != NULL);
	VkMemoryRequirements memory_requirements = {};
	vkGetImageMemoryRequirements(device.handle, image, &memory_requirements);

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate.allocationSize = memory_requirements.size;
	m_size = memory_requirements.size;

	bool found = physical_device.setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memory_allocate.memoryTypeIndex);

	return vkAllocateMemory(device.handle, &memory_allocate, nullptr, &memory);
}

VkResult Vulkan::Texture::write(const Device& device) {
	assert(mp_png->available());
	assert(image != NULL);
	assert(memory != NULL);
	VkResult result;
	VkImageSubresource subresource = {};
	subresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	
	VkSubresourceLayout subresource_layout = {};

	vkGetImageSubresourceLayout(device.handle, image, &subresource, &subresource_layout);
	void* destination;// = mp_png->getDataPointer();
	result = vkMapMemory(device.handle, memory, 0, m_size, 0, &destination);
	assert(result == VK_SUCCESS);
	memcpy(destination, mp_png->getDataPointer(), mp_png->getSize());
	vkUnmapMemory(device.handle, memory);
	return vkBindImageMemory(device.handle, image, memory, 0);
}

VkDescriptorImageInfo* Vulkan::Texture::getInformationPointer() {
	assert(sampler != NULL);
	assert(view != NULL);
	assert(layout != NULL);
	if(mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorImageInfo;
		mp_information->sampler = sampler;
		mp_information->imageView = view;
		mp_information->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	}
	return mp_information;
}