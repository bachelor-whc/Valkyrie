#include "valkyrie/vulkan/swapchain.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/tool.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/render_pass.h"
#include "valkyrie/vulkan/queue.h"
using namespace Vulkan;

Framebuffers::Framebuffers(const uint32_t count, const std::vector<SwapChainBuffer>& buffers) :
	m_count(count),
	m_swap_chain_views(buffers.size()) {
	assert(count == buffers.size());
	handles.resize(m_count);
	for (int i = 0; i < m_count; ++i)
		m_swap_chain_views[i] = buffers[i].view;
}

Framebuffers::~Framebuffers() {
	
}

void Framebuffers::initialize(const Device& device, const RenderPass& render_pass, const int width, const int height) {
	int extended_count = extendedAttachments.size();
	std::vector<VkImageView> attachments(1 + extended_count);
	if(extended_count > 0) {
		VkImageView* destination = attachments.data();
		destination += 1;
		memcpy(destination, extendedAttachments.data(), extended_count * sizeof(VkImageView));
	}

	VkFramebufferCreateInfo frame_buffer_info = {};
	frame_buffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frame_buffer_info.renderPass = render_pass.handle;
	frame_buffer_info.attachmentCount = attachments.size();
	frame_buffer_info.pAttachments = attachments.data();
	frame_buffer_info.width = width;
	frame_buffer_info.height = height;
	frame_buffer_info.layers = 1;

	VkResult result;
	for (int i = 0; i < m_count; ++i) {
		attachments[0] = m_swap_chain_views[i];
		result = vkCreateFramebuffer(device.handle, &frame_buffer_info, nullptr, &handles[i]);
		assert(result == VK_SUCCESS);
	}
}

SwapChain::SwapChain(const Device& device, const PhysicalDevice& physical_device, const Surface& surface, const Wendy::Window& window) :
	mp_framebuffers(nullptr),
	m_current_buffer(-1),
	m_width(window.getWidth()),
	m_height(window.getHeight()),
	m_images_initialized(false) {
	VkResult result = VK_SUCCESS;
	VkSurfaceCapabilitiesKHR surface_capabilities = {};

	// Specification:
	// To query the basic capabilities of a surface, needed in order to create a swapchain.
	result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device.handle, surface.handle, &surface_capabilities);

	uint32_t present_mode_count;
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.handle, surface.handle, &present_mode_count, nullptr);

	std::vector<VkPresentModeKHR> present_modes(present_mode_count);
	result = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device.handle, surface.handle, &present_mode_count, present_modes.data());

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
	for (size_t i = 0; i < present_mode_count; ++i) {
		if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchain_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchain_present_mode != VK_PRESENT_MODE_MAILBOX_KHR) && (present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			swapchain_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	uint32_t desired_number_of_swap_chain_images = surface_capabilities.minImageCount > 2 ? surface_capabilities.minImageCount : 2;
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

	result = vkCreateSwapchainKHR(device.handle, &swapchain_create, nullptr, &handle);
	assert(result == VK_SUCCESS);
}

SwapChain::~SwapChain() {
	if (mp_framebuffers != nullptr)
		delete mp_framebuffers;
}

VkResult SwapChain::initializeImages(const Device& device, const Surface& surface, CommandBuffer& buffer) {
	VkResult result;
	uint32_t buffer_count;
	result = vkGetSwapchainImagesKHR(device.handle, handle, &buffer_count, nullptr);
	if (result != VK_SUCCESS)
		return result;

	VkImage *swapchain_images = NEW_NT VkImage[buffer_count];
	assert(swapchain_images);

	result = vkGetSwapchainImagesKHR(device.handle, handle, &buffer_count, swapchain_images);
	if(result != VK_SUCCESS)
		return result;

	m_buffers.resize(buffer_count);

	for (uint32_t i = 0; i < getImageCount(); i++) {
		VkImageViewCreateInfo image_view_create = {};
		image_view_create.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		image_view_create.format = surface.format.format;
		image_view_create.components.r = VK_COMPONENT_SWIZZLE_R;
		image_view_create.components.g = VK_COMPONENT_SWIZZLE_G;
		image_view_create.components.b = VK_COMPONENT_SWIZZLE_B;
		image_view_create.components.a = VK_COMPONENT_SWIZZLE_A;
		image_view_create.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		image_view_create.subresourceRange.baseMipLevel = 0;
		image_view_create.subresourceRange.levelCount = 1;
		image_view_create.subresourceRange.baseArrayLayer = 0;
		image_view_create.subresourceRange.layerCount = 1;
		image_view_create.viewType = VK_IMAGE_VIEW_TYPE_2D;
		image_view_create.flags = 0;

		m_buffers[i].image = swapchain_images[i];

		setImageLayout(
			buffer,
			m_buffers[i].image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		image_view_create.image = m_buffers[i].image;

		result = vkCreateImageView(device.handle, &image_view_create, nullptr, &m_buffers[i].view);
		if(result != VK_SUCCESS)
			return result;
	}
	m_current_buffer = 0;
	m_images_initialized = true;
	return result;
}

void SwapChain::initializeFramebuffers(const Device& device, const RenderPass& render_pass, const VkImageView* extended_attachments, int count) {
	mp_framebuffers = NEW_NT Framebuffers(2, m_buffers);
	assert(mp_framebuffers != nullptr && m_images_initialized);
	mp_framebuffers->extendedAttachments.resize(count);
	memcpy(mp_framebuffers->extendedAttachments.data(), 
		extended_attachments, 
		count * sizeof(VkImageView));
	mp_framebuffers->initialize(device, render_pass, m_width, m_height);
}

VkResult SwapChain::acquireNextImage(const Device& device, uint64_t timeout, const VkSemaphore semaphore, const VkFence fence) {
	return vkAcquireNextImageKHR(device.handle, handle, timeout, semaphore, fence, &m_current_buffer);
}

VkResult SwapChain::queuePresent(const Queue& queue) {
	VkPresentInfoKHR present = {};
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.swapchainCount = 1;
	present.pSwapchains = &handle;
	present.pImageIndices = &m_current_buffer;
	return vkQueuePresentKHR(queue.handle, &present);
}

void Vulkan::DestroySwapChain(const Device& device, SwapChain& swapchain) {
	vkDestroySwapchainKHR(device.handle, swapchain.handle, nullptr);
}

void Vulkan::DestroyFramebuffers(const Device& device, Framebuffers& framebuffers) {
	for (auto& framebuffer : framebuffers.handles) {
		vkDestroyFramebuffer(device.handle, framebuffer, nullptr);
	}
}