#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/depth_buffer.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/tool.h"
using namespace Vulkan;

DepthBuffer::DepthBuffer(const PhysicalDevice& physical_device) {
	std::vector<VkFormat> depth_formats = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};

	bool found = false;
	for (auto& device_format : depth_formats) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(physical_device.handle, device_format, &format_properties);
		if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			format = device_format;
			found = true;
			break;
		}
	}
	assert(found);
}

DepthBuffer::~DepthBuffer() {

}

VkResult DepthBuffer::initializeImages(const Device& device, const PhysicalDevice& physical_device, CommandBuffer& buffer, const Wendy::Window& window) {
	VkResult result;
	
	VkImageCreateInfo image_create = {};
	image_create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create.imageType = VK_IMAGE_TYPE_2D;
	image_create.format = format;
	image_create.extent.width = window.getWidth();
	image_create.extent.height = window.getHeight();
	image_create.extent.depth = 1;
	image_create.mipLevels = 1;
	image_create.arrayLayers = 1;
	image_create.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkImageViewCreateInfo image_view_create = {};
	image_view_create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_create.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create.format = format;
	image_view_create.components.r = VK_COMPONENT_SWIZZLE_R;
	image_view_create.components.g = VK_COMPONENT_SWIZZLE_G;
	image_view_create.components.b = VK_COMPONENT_SWIZZLE_B;
	image_view_create.components.a = VK_COMPONENT_SWIZZLE_A;
	image_view_create.flags = 0;
	image_view_create.subresourceRange = {};
	image_view_create.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	image_view_create.subresourceRange.baseMipLevel = 0;
	image_view_create.subresourceRange.levelCount = 1;
	image_view_create.subresourceRange.baseArrayLayer = 0;
	image_view_create.subresourceRange.layerCount = 1;

	VkMemoryRequirements memory_requirements;

	result = vkCreateImage(device.handle, &image_create, nullptr, &image);
	vkGetImageMemoryRequirements(device.handle, image, &memory_requirements);
	memory_allocate.allocationSize = memory_requirements.size;
	physical_device.setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_allocate.memoryTypeIndex);
	result = vkAllocateMemory(device.handle, &memory_allocate, nullptr, &memory);

	result = vkBindImageMemory(device.handle, image, memory, 0);
	setImageLayout(buffer, image, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	image_view_create.image = image;
	result = vkCreateImageView(device.handle, &image_view_create, nullptr, &view);
	return result;
}

VkAttachmentDescription DepthBuffer::getAttachmentDescription() const {
	VkAttachmentDescription attachment_description = {};
	attachment_description.format = format;
	attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
	attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment_description.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachment_description.flags = 0;
	return attachment_description;
}