#include <vulkan/vulkan.h>
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/vulkan/pipeline.h"
#include "common.h"
using namespace Valkyrie;

VulkanManager* VulkanManager::gp_vulkan_manager = nullptr;

int VulkanManager::initialize() {
	if (gp_vulkan_manager != nullptr)
		return 0;
	gp_vulkan_manager = NEW_NT VulkanManager;
	if (gp_vulkan_manager == nullptr) {
		return 1;
	}
	return 0;
}

void VulkanManager::close() {
	if (gp_vulkan_manager != nullptr) {
		delete gp_vulkan_manager;
		gp_vulkan_manager = nullptr;
	}
}

bool VulkanManager::initialized() {
	return gp_vulkan_manager != nullptr;
}

VkInstance Valkyrie::VulkanManager::getInstance() {
	return gp_vulkan_manager->m_instatnce.handle;
}

VkPhysicalDevice Valkyrie::VulkanManager::getPhysicalDevice() {
	return gp_vulkan_manager->m_physical_device.handle;
}

VkDevice Valkyrie::VulkanManager::getDevice() {
	return gp_vulkan_manager->m_device.handle;
}

void Valkyrie::VulkanManager::setImageLayout(VkImage image, VkImageAspectFlags aspect_mask, VkImageLayout old_image_layout, VkImageLayout new_image_layout) {
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.oldLayout = old_image_layout;
	image_memory_barrier.newLayout = new_image_layout;
	image_memory_barrier.image = image;
	image_memory_barrier.subresourceRange.aspectMask = aspect_mask;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.layerCount = 1;

	if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (old_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		image_memory_barrier.srcAccessMask = image_memory_barrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	VkPipelineStageFlags source_flag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destination_flag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(getSetupCommandBuffer(), source_flag, destination_flag, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}

VulkanManager::~VulkanManager() {
	Vulkan::DestroyPipelineCache();
	Vulkan::DestroyCommandPool(*m_command_pool_ptr);
	Vulkan::DestroyDevice(m_device);
	Vulkan::DestroyInstance(m_instatnce);
}

void VulkanManager::initailizeTexture(Vulkan::Texture& texture) {
	VkResult result;
	result = texture.initializeImage();
	assert(result == VK_SUCCESS);
	result = texture.allocate();
	assert(result == VK_SUCCESS);
	result = texture.write();
	assert(result == VK_SUCCESS);
	result = texture.initializeSampler();
	assert(result == VK_SUCCESS);
	result = texture.initializeView();
	assert(result == VK_SUCCESS);
	result = m_setup_command_buffer.begin();
	assert(result == VK_SUCCESS);
	setImageLayout(texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, texture.layout);
	m_setup_command_buffer.end();
	result = m_setup_command_buffer.submit(m_graphics_queue);
	assert(result == VK_SUCCESS);
}

VulkanManager::VulkanManager() {
	initializeInstance();
	initializePhysicalDevice();
	initializeDevice();
	initializeCommandPool();
	m_setup_command_buffer = m_command_pool_ptr->createCommandBuffer();
	initializePipelineCache();
}

VkCommandBuffer VulkanManager::getSetupCommandBuffer() {
	return gp_vulkan_manager->m_setup_command_buffer.handle;
}

void VulkanManager::initializeInstance() {
	VkResult result;
	result = Vulkan::CreateInstance("Valkyrie", m_instatnce);
	g_instance_handle = m_instatnce.handle;
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializePhysicalDevice() {
	VkResult result;
	result = Vulkan::CreatePhysicalDevice(m_physical_device);
	g_physical_device_handle = m_physical_device.handle;
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializeDevice() {
	VkResult result;
	result = Vulkan::CreateDevice(m_device);
	g_device_handle = m_device.handle;
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializePipelineCache() {
	VkResult result;
	result = Vulkan::PipelineModule::initializeCache();
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializeCommandPool() {
	bool queue_got = GetQueue(VK_QUEUE_GRAPHICS_BIT, m_graphics_queue);
	assert(queue_got == true);

	m_command_pool_ptr = MAKE_SHARED(Vulkan::CommandPool)(m_graphics_queue);
	assert(m_command_pool_ptr->handle != VK_NULL_HANDLE);
}
