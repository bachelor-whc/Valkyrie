#include <SDL.h>
#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/vulkan/queue.h"
#include "valkyrie/UI/window.h"
#include "valkyrie/UI/window_manager.h"
#include "valkyrie/utility/vulkan_manager.h"
using namespace Vulkan;

Framebuffers::Framebuffers(const uint32_t count, const std::vector<SwapChainBuffer>& buffers) :
	m_count(count),
	m_swap_chain_views(buffers.size()) {
	assert(count == buffers.size());
	handles.resize(m_count);
	for (uint32_t i = 0; i < m_count; ++i)
		m_swap_chain_views[i] = buffers[i].view;
}

Framebuffers::~Framebuffers() {
	
}

void Framebuffers::initialize(const RenderPass& render_pass, const int width, const int height) {
	const auto& device = Valkyrie::VulkanManager::getDevice();

	VkFramebufferCreateInfo frame_buffer_info = {};
	frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frame_buffer_info.renderPass = render_pass.handle;
	frame_buffer_info.attachmentCount = extendedAttachments.size();
	frame_buffer_info.pAttachments = extendedAttachments.data();
	frame_buffer_info.width = width;
	frame_buffer_info.height = height;
	frame_buffer_info.layers = 1;

	VkResult result;
	for (int i = 0; i < m_count; ++i) {
		extendedAttachments[0] = m_swap_chain_views[i];
		result = vkCreateFramebuffer(device, &frame_buffer_info, nullptr, &handles[i]);
		assert(result == VK_SUCCESS);
	}
}

SwapChain::SwapChain(const Surface& surface, const Valkyrie::WindowPtr& window_ptr) :
	mp_framebuffers(nullptr),
	m_current_buffer(-1),
	m_width(window_ptr->getWidth()),
	m_height(window_ptr->getHeight()),
	m_images_initialized(false) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	const auto& physical_device = Valkyrie::VulkanManager::getPhysicalDevice();
	VkResult result = VK_SUCCESS;
	VkSurfaceCapabilitiesKHR surface_capabilities = {};

	// Specification:
	// To query the basic capabilities of a surface, needed in order to create a swapchain.
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface.handle, &surface_capabilities);

	uint32_t present_mode_count;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface.handle, &present_mode_count, nullptr);

	std::vector<VkPresentModeKHR> present_modes(present_mode_count);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface.handle, &present_mode_count, present_modes.data());

	VkExtent2D swap_chain_extent;
	bool surface_size_undefined = surface_capabilities.currentExtent.width == 0xFFFFFFFF && surface_capabilities.currentExtent.height == 0xFFFFFFFF;
	if (surface_size_undefined) {
		swap_chain_extent.width = m_width;
		swap_chain_extent.height = m_height;
	}
	else {
		swap_chain_extent = surface_capabilities.currentExtent;
	}

	VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
	for (uint32_t i = 0; i < present_mode_count; ++i) {
		if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchain_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchain_present_mode != VK_PRESENT_MODE_MAILBOX_KHR) && (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			swapchain_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	uint32_t desired_number_of_swap_chain_images = surface_capabilities.minImageCount > VALKYRIE_FRAME_BUFFER_COUNT ? surface_capabilities.minImageCount : VALKYRIE_FRAME_BUFFER_COUNT;
	if (surface_capabilities.maxImageCount > 0 && desired_number_of_swap_chain_images > surface_capabilities.maxImageCount) {
		desired_number_of_swap_chain_images = surface_capabilities.maxImageCount;
	}

	VkSurfaceTransformFlagBitsKHR transform;
	if (surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		transform = surface_capabilities.currentTransform;
	}

	VkBool32 is_support;
	vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, Valkyrie::VulkanManager::getGraphicsQueue().index, surface.handle, &is_support);
	assert(is_support);
	if (is_support) {
		VkSwapchainCreateInfoKHR swapchain_create = {};
		swapchain_create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create.surface = surface.handle;
		swapchain_create.minImageCount = desired_number_of_swap_chain_images;
		swapchain_create.imageFormat = surface.format.format;
		swapchain_create.imageColorSpace = surface.format.colorSpace;
		swapchain_create.imageExtent.width = swap_chain_extent.width;
		swapchain_create.imageExtent.height = swap_chain_extent.height;
		swapchain_create.preTransform = transform;
		swapchain_create.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create.imageArrayLayers = 1;
		swapchain_create.presentMode = swapchain_present_mode;
		swapchain_create.oldSwapchain = VK_NULL_HANDLE;
		swapchain_create.clipped = VK_TRUE;
		swapchain_create.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchain_create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchain_create.queueFamilyIndexCount = 0;
		swapchain_create.pQueueFamilyIndices = nullptr;

		result = vkCreateSwapchainKHR(device, &swapchain_create, nullptr, &handle);
	}
	assert(result == VK_SUCCESS);
}

SwapChain::~SwapChain() {
	if (mp_framebuffers != nullptr)
		delete mp_framebuffers;
}

VkResult SwapChain::initializeImages(const Surface& surface) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	VkResult result;
	uint32_t buffer_count;
	result = vkGetSwapchainImagesKHR(device, handle, &buffer_count, nullptr);
	if (result != VK_SUCCESS)
		return result;

	VkImage *swapchain_images = NEW_NT VkImage[buffer_count]();
	assert(swapchain_images);

	result = vkGetSwapchainImagesKHR(device, handle, &buffer_count, swapchain_images);
	if(result != VK_SUCCESS)
		return result;

	m_buffers.resize(buffer_count);

	for (uint32_t i = 0; i < getImageCount(); i++) {
		m_buffers[i].handle = swapchain_images[i];
		
		VkImageViewCreateInfo image_view_create = m_buffers[i].getImageViewCreate();
		image_view_create.format = surface.format.format;
		image_view_create.image = m_buffers[i].handle;

		m_buffers[i].mask = VK_IMAGE_ASPECT_COLOR_BIT;
		m_buffers[i].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		m_buffers[i].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		//Valkyrie::VulkanManager::setImageLayout(m_buffers[i]);

		result = vkCreateImageView(device, &image_view_create, nullptr, &m_buffers[i].view);
		if(result != VK_SUCCESS)
			return result;
	}
	m_current_buffer = 0;
	m_images_initialized = true;
	delete[] swapchain_images;
	return result;
}

void SwapChain::initializeFramebuffers(const RenderPass& render_pass, const VkImageView* extended_attachments, int count) {
	mp_framebuffers = NEW_NT Framebuffers(VALKYRIE_FRAME_BUFFER_COUNT, m_buffers);
	assert(mp_framebuffers != nullptr && m_images_initialized);
	mp_framebuffers->extendedAttachments.resize(count);
	memcpy(
		mp_framebuffers->extendedAttachments.data() + 1, 
		extended_attachments, 
		sizeof(VkImageView)
	);
	mp_framebuffers->initialize(render_pass, m_width, m_height);
}

VkResult SwapChain::acquireNextImage(uint64_t timeout, const VkSemaphore semaphore, const VkFence fence) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	return vkAcquireNextImageKHR(device, handle, timeout, semaphore, fence, &m_current_buffer);
}

VkResult SwapChain::queuePresent(const VkQueue& queue, VkSemaphore semaphore) {
	VkPresentInfoKHR present = {};
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.swapchainCount = 1;
	present.pSwapchains = &handle;
	present.pImageIndices = &m_current_buffer;
	if (semaphore != VK_NULL_HANDLE) {
		present.waitSemaphoreCount = 1;
		present.pWaitSemaphores = &semaphore;
	}
	return vkQueuePresentKHR(queue, &present);
}

void Vulkan::DestroySwapChain(SwapChain& swapchain) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	vkDestroySwapchainKHR(device, swapchain.handle, nullptr);
}

void Vulkan::DestroyFramebuffers(Framebuffers& framebuffers) {
	const auto& device = Valkyrie::VulkanManager::getDevice();
	for (auto& framebuffer : framebuffers.handles) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}

VkImageCreateInfo SwapChainBuffer::getImageCreate() const {
	VkImageCreateInfo create = {};
	return create;
}

VkImageViewCreateInfo SwapChainBuffer::getImageViewCreate() const {
	VkImageViewCreateInfo create = VK_DEFAULT_SWAPCHAIN_IMAGE_VIEW_CREATE_INFO;
	return create;
}

VkFlags SwapChainBuffer::getMemoryType() const {
	return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
}
