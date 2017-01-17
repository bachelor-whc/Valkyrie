#include <vulkan/vulkan.h>
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/vulkan/pipeline_module.h"
#include "common.h"
using namespace Valkyrie;

VulkanManager* VulkanManager::gp_vulkan_manager = nullptr;

int VulkanManager::initialize() {
	if (gp_vulkan_manager != nullptr)
		return 0;
	gp_vulkan_manager = NEW_NT VulkanManager;
	gp_vulkan_manager->initializeVulkan();
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

VkInstance VulkanManager::getInstance() {
	return gp_vulkan_manager->m_instatnce.handle;
}

VkPhysicalDevice VulkanManager::getPhysicalDevice() {
	return gp_vulkan_manager->m_physical_device.handle;
}

VkDevice VulkanManager::getDevice() {
	return gp_vulkan_manager->m_device.handle;
}

VkResult VulkanManager::initializeImage(Vulkan::Image& image) {
	const auto& device = VulkanManager::getDevice();
	VkResult result;
	VkImageCreateInfo image_create = image.getImageCreate();
	result = vkCreateImage(device, &image_create, nullptr, &image.handle);
	assert(result == VK_SUCCESS);
	allocateMemory(image);
	VkImageViewCreateInfo image_view_create = image.getImageViewCreate();
	result = vkCreateImageView(device, &image_view_create, nullptr, &image.view);
	return result;
}

VkResult VulkanManager::initializeTexture(Vulkan::Texture& texture) {
	VkResult result = initializeImage(texture);
	assert(result == VK_SUCCESS);
	result = texture.write();
	assert(result == VK_SUCCESS);
	result = texture.initializeSampler();
	assert(result == VK_SUCCESS);
	texture.mask = VK_IMAGE_ASPECT_COLOR_BIT;
	texture.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
	texture.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	result = setImageLayout(texture);
	assert(result == VK_SUCCESS);
	return result;
}

VkResult VulkanManager::setImageLayout(const Vulkan::Image& image) {
	VkImageMemoryBarrier image_memory_barrier = {};
	image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	image_memory_barrier.oldLayout = image.oldLayout;
	image_memory_barrier.newLayout = image.newLayout;
	image_memory_barrier.image = image.handle;
	image_memory_barrier.subresourceRange.aspectMask = image.mask;
	image_memory_barrier.subresourceRange.baseMipLevel = 0;
	image_memory_barrier.subresourceRange.levelCount = 1;
	image_memory_barrier.subresourceRange.layerCount = 1;

	if (image.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (image.oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (image.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (image.oldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	if (image.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	if (image.newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
		image_memory_barrier.srcAccessMask = image_memory_barrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (image.newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (image.newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	if (image.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	VkPipelineStageFlags source_flag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destination_flag = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	auto command_buffer = getSetupCommandBuffer();
	command_buffer.begin();
	vkCmdPipelineBarrier(command_buffer.handle, source_flag, destination_flag, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
	command_buffer.end();
	return command_buffer.submit(getGraphicsQueue());
}

bool VulkanManager::getSupportFormat(VkFormat& format, const VkFlags flag) {
	std::vector<VkFormat> formats;
	if(flag == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
		formats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};
	}
	if (formats.size() == 0) {
		return true;
	}
	for (auto& desired_format : formats) {
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(getPhysicalDevice(), desired_format, &format_properties);
		if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			format = desired_format;
			return true;
		}
	}
	return false;
}

VulkanManager::~VulkanManager() {
	Vulkan::DestroyPipelineCache();
	Vulkan::DestroyCommandPool(*m_command_pool_ptr);
	Vulkan::DestroyDevice(m_device);
	Vulkan::DestroyInstance(m_instatnce);
}

Vulkan::CommandBuffer VulkanManager::createCommandBuffer() {
	return gp_vulkan_manager->m_command_pool_ptr->createCommandBuffer();
}

VulkanManager::VulkanManager() {
	
}

Vulkan::CommandBuffer VulkanManager::getSetupCommandBuffer() {
	return gp_vulkan_manager->m_setup_command_buffer;
}

VkResult VulkanManager::allocateMemory(Vulkan::Image & image) {
	const auto& device = VulkanManager::getDevice();
	VkResult result;

	VkMemoryAllocateInfo memory_allocate = {};
	memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, image.handle, &memory_requirements);

	memory_allocate.allocationSize = memory_requirements.size;
	Vulkan::PhysicalDevice::setMemoryType(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, memory_allocate.memoryTypeIndex);
	result = vkAllocateMemory(device, &memory_allocate, nullptr, &image.memory);
	if (result != VK_SUCCESS)
		return result;
	result = vkBindImageMemory(device, image.handle, image.memory, 0);
	return result;
}

void VulkanManager::initializeVulkan() {
	initializeInstance();
	initializePhysicalDevice();
	initializeDevice();
	initializeCommandPool();
	m_setup_command_buffer = m_command_pool_ptr->createCommandBuffer();
	initializePipelineCache();
}

void VulkanManager::initializeInstance() {
	VkResult result;
	result = Vulkan::CreateInstance("Valkyrie", m_instatnce);
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializePhysicalDevice() {
	VkResult result;
	result = Vulkan::CreatePhysicalDevice(m_physical_device);
	assert(result == VK_SUCCESS);
}

void VulkanManager::initializeDevice() {
	VkResult result;
	result = Vulkan::CreateDevice(m_device);
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
