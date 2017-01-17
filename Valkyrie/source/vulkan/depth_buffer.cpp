#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/depth_buffer.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/vulkan/default_create_info.h"
using namespace Vulkan;

DepthBuffer::DepthBuffer(uint32_t width, uint32_t height) :
	m_width(width),
	m_height(height) {
	Valkyrie::VulkanManager::getSupportFormat(format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

DepthBuffer::~DepthBuffer() {

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

VkImageCreateInfo DepthBuffer::getImageCreate() const {
	auto create = VK_DEFAULT_DEPTH_IMAGE_CREATE_INFO;
	create.format = format;
	create.extent.width = m_width;
	create.extent.height = m_height;
	return create;
}

VkImageViewCreateInfo DepthBuffer::getImageViewCreate() const {
	auto create = VK_DEFAULT_DEPTH_IMAGE_VIEW_CREATE_INFO;
	create.format = format;
	create.image = handle;
	return create;
}
