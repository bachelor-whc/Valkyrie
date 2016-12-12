#include "valkyrie/vulkan/image_texture.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/physical_device.h"

Vulkan::ImageTexture::ImageTexture(const ValkyrieImageFilePointer& texture_ptr) : Texture(), mp_image(texture_ptr) {

}

Vulkan::ImageTexture::~ImageTexture() {

}

VkResult Vulkan::ImageTexture::initializeImage(const Device& device) {
	VkImageCreateInfo image_create = {};
	image_create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create.imageType = VK_IMAGE_TYPE_2D;
	image_create.format = VK_FORMAT_R8G8B8A8_UNORM;
	image_create.extent.depth = 1;
	image_create.extent.width = mp_image->getWidth();
	image_create.extent.height = mp_image->getHeight();
	image_create.mipLevels = 1;
	image_create.arrayLayers = 1;
	image_create.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create.tiling = VK_IMAGE_TILING_LINEAR;
	image_create.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	image_create.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	return vkCreateImage(device.handle, &image_create, nullptr, &image);
}

VkResult Vulkan::ImageTexture::allocate(const Device& device, PhysicalDevice& physical_device) {
	assert(mp_image->available());
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

VkResult Vulkan::ImageTexture::write(const Device& device) {
	assert(mp_image->available());
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
	memcpy(destination, mp_image->getDataPointer(), mp_image->getSize());
	vkUnmapMemory(device.handle, memory);
	return vkBindImageMemory(device.handle, image, memory, 0);
}