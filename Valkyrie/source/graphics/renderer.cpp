#include "valkyrie/graphics/renderer.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/UI/window_manager.h"
#include "valkyrie/UI/window.h"
#include "valkyrie/vulkan/default_create_info.h"
using namespace Valkyrie;

Renderer::Renderer(const WindowPtr& window_ptr) : 
	m_window_ptr(window_ptr) {
	const auto& device = VulkanManager::getDevice();
	initializeSurface();
	initializeSwapChain();
	initializeDepthBuffer();
	initializeRenderPass();
	initializeFramebuffers();
	renderCommands.resize(mp_swapchain->getImageCount());
	for (auto& command : renderCommands) {
		command = VulkanManager::instance().createCommandBuffer();
	}

	VkResult result;

	VkSemaphoreCreateInfo semaphore_create = {};
	semaphore_create.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create.pNext = nullptr;
	semaphore_create.flags = 0;
	result = vkCreateSemaphore(device, &semaphore_create, nullptr, &m_render_semaphore);
	result = vkCreateSemaphore(device, &semaphore_create, nullptr, &m_present_semaphore);
	assert(result == VK_SUCCESS);

	VkFenceCreateInfo present_fence_create = {};
	present_fence_create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	present_fence_create.pNext = nullptr;
	present_fence_create.flags = 0;
	result = vkCreateFence(device, &present_fence_create, nullptr, &m_fence);
	assert(result == VK_SUCCESS);

	m_submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	m_submit.pWaitDstStageMask = &m_submit_pipeline_stages;
	m_submit.waitSemaphoreCount = 1;
	m_submit.pWaitSemaphores = &m_present_semaphore;
	m_submit.signalSemaphoreCount = 1;
	m_submit.pSignalSemaphores = &m_render_semaphore;
}

Renderer::~Renderer() {
	const auto& device = VulkanManager::getDevice();
	vkDestroyFence(device, m_fence, nullptr);
	vkDestroySemaphore(device, m_render_semaphore, nullptr);
	vkDestroySemaphore(device, m_present_semaphore, nullptr);
	DestroyFramebuffers(*mp_swapchain->getFramebuffers());
	DestroySwapChain(*mp_swapchain);
	DestroySurface(m_surface);
	if (mp_swapchain != nullptr) {
		delete mp_swapchain;
	}
	if(mp_depth_buffer != nullptr) {
		delete mp_depth_buffer;
	}
}

void Renderer::prepareFrame() {
	VkResult result;
	result = mp_swapchain->acquireNextImage(UINT64_MAX, m_present_semaphore, VK_NULL_HANDLE);
	assert(result == VK_SUCCESS);
}

VkResult Renderer::render() {
	VkResult result;
	const auto& device = VulkanManager::getDevice();
	const auto& queue = VulkanManager::getGraphicsQueue();

	m_submit.commandBufferCount = 1;
	m_submit.pCommandBuffers = &renderCommands[mp_swapchain->getCurrent()].handle;

	result = vkQueueSubmit(queue.handle, 1, &m_submit, m_fence);
	assert(result == VK_SUCCESS);

	do {
		result = vkWaitForFences(VulkanManager::getDevice(), 1, &m_fence, VK_TRUE, 100000000);
	} while (result == VK_TIMEOUT);
	assert(result == VK_SUCCESS);
	vkResetFences(device, 1, &m_fence);

	result = mp_swapchain->queuePresent(queue.handle, m_render_semaphore);
	assert(result == VK_SUCCESS);
	result = vkQueueWaitIdle(queue.handle);
	assert(result == VK_SUCCESS);
	return VK_SUCCESS;
}

VkRenderPassBeginInfo Renderer::getRenderPassBegin() const {
	auto rpb = VK_DEFAULT_RENDER_PASS_BEGIN;
	rpb.renderArea.extent.width = m_window_ptr->getWidth();
	rpb.renderArea.extent.height = m_window_ptr->getHeight();
	rpb.renderPass = m_render_pass.handle;
	return rpb;
}

void Renderer::initializeSwapChain() {
	VkResult result;
	mp_swapchain = NEW_NT Vulkan::SwapChain(m_surface, m_window_ptr);
	result = mp_swapchain->initializeImages(m_surface);
	assert(result == VK_SUCCESS);
}

void Renderer::initializeDepthBuffer() {
	VkResult result;
	mp_depth_buffer = NEW_NT Vulkan::DepthBuffer(m_window_ptr->getWidth(), m_window_ptr->getHeight());
	result = VulkanManager::initializeImage(*mp_depth_buffer);
	assert(result == VK_SUCCESS);
}

void Renderer::initializeRenderPass() {
	m_render_pass.attachments.push_back(m_surface.getAttachmentDescription());
	m_render_pass.attachments.push_back(mp_depth_buffer->getAttachmentDescription());

	Vulkan::Subpass subpass;

	VkAttachmentReference color_reference = {};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_reference = {};
	depth_reference.attachment = 1;
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.colorAttachmentReferences.push_back(color_reference);
	subpass.setDepthAttachmentReferences(depth_reference);

	m_render_pass.subpasses.push_back(subpass.createSubpassDescription());

	Vulkan::SubpassDependencies dependencies;
	bool initialized = m_render_pass.initialize(dependencies);
	assert(initialized);
}

void Renderer::initializeFramebuffers() {
	mp_swapchain->initializeFramebuffers(m_render_pass, &(mp_depth_buffer->view), 2);
}

void Renderer::initializeSurface() {
	VkResult result;
	result = setSurface(m_surface, m_window_ptr);
	assert(result == VK_SUCCESS);
}

void Renderer::commandSetViewport(const Vulkan::CommandBuffer& command_buffer) {
	m_viewport.width = m_window_ptr->getWidth();
	m_viewport.height = m_window_ptr->getHeight();
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;
	m_viewport.x = 0;
	m_viewport.y = 0;
	vkCmdSetViewport(command_buffer.handle, 0, 1, &m_viewport);
}

void Renderer::commandSetScissor(const Vulkan::CommandBuffer& command_buffer) {
	m_scissor.extent.width = m_window_ptr->getWidth();
	m_scissor.extent.height = m_window_ptr->getHeight();
	m_scissor.offset.x = 0;
	m_scissor.offset.y = 0;
	vkCmdSetScissor(command_buffer.handle, 0, 1, &m_scissor);
}