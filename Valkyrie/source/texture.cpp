#include "valkyrie/vulkan/texture.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/physical_device.h"

Vulkan::Texture::Texture() {

}

Vulkan::Texture::~Texture() {

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

VkDescriptorImageInfo* Vulkan::Texture::getInformationPointer() {
	assert(sampler != NULL);
	assert(view != NULL);
	assert(layout != NULL);
	if (mp_information == nullptr) {
		mp_information = NEW_NT VkDescriptorImageInfo;
		mp_information->sampler = sampler;
		mp_information->imageView = view;
		mp_information->imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	}
	return mp_information;
}