#include "valkyrie/render_context.h"
#include "valkyrie/utility/vulkan_manager.h"
#include "valkyrie/UI/window_manager.h"
#include "valkyrie/UI/window.h"
using namespace Valkyrie;

RenderContext::RenderContext() {
	const auto& device = VulkanManager::getDevice();
	initializeSurface();
	initializeSwapChain();
	initializeDepthBuffer();
	//m_present_command_buffer = m_command_pool_ptr->createCommandBuffer();
	//result = m_setup_command_buffer.begin();
	initializeRenderPass();
	initializeFramebuffers();
	//result = m_setup_command_buffer.end();
	//result = m_setup_command_buffer.submit(m_graphics_queue);
	renderCommands.resize(mp_swapchain->getImageCount());
	for (auto& command : renderCommands) {
		command = VulkanManager::getGlobalVulkanManagerPtr()->createCommandBuffer();
	}

	VkResult result;

	VkFenceCreateInfo present_fence_create = {};
	present_fence_create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	present_fence_create.pNext = nullptr;
	present_fence_create.flags = 0;
	result = vkCreateFence(device, &present_fence_create, nullptr, &m_present_fence);
	assert(result == VK_SUCCESS);

	VkSemaphoreCreateInfo present_semaphore_create = {};
	present_semaphore_create.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	present_semaphore_create.pNext = nullptr;
	present_semaphore_create.flags = 0;
	result = vkCreateSemaphore(device, &present_semaphore_create, nullptr, &m_present_semaphore);
	assert(result == VK_SUCCESS);
}

RenderContext::~RenderContext() {
	const auto& device = VulkanManager::getDevice();
	vkDestroySemaphore(device, m_present_semaphore, nullptr);
	vkDestroyFence(device, m_present_fence, nullptr);
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

VkResult RenderContext::render() {
	VkResult result;
	const auto& queue = VulkanManager::getGraphicsQueue();

	result = mp_swapchain->acquireNextImage(UINT64_MAX, m_present_semaphore, m_present_fence);
	assert(result == VK_SUCCESS);

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &m_present_semaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &renderCommands[mp_swapchain->getCurrent()].handle;

	result = vkQueueSubmit(queue.handle, 1, &submit, m_present_fence);
	assert(result == VK_SUCCESS);

	do {
		result = vkWaitForFences(VulkanManager::getDevice(), 1, &m_present_fence, VK_TRUE, 100000000);
	} while (result == VK_TIMEOUT);
	assert(result == VK_SUCCESS);
	vkResetFences(VulkanManager::getDevice(), 1, &m_present_fence);

	result = mp_swapchain->queuePresent(queue.handle);
	assert(result == VK_SUCCESS);

	return VK_SUCCESS;
}

void RenderContext::initializeSwapChain() {
	VkResult result;
	mp_swapchain = NEW_NT Vulkan::SwapChain(m_surface);
	result = mp_swapchain->initializeImages(m_surface);
	assert(result == VK_SUCCESS);
}

void RenderContext::initializeDepthBuffer() {
	auto& window_manager = *WindowManager::getGlobalWindowManagerPtr();
	auto& window_ptr = window_manager.getMainWindowPtr();
	VkResult result;
	mp_depth_buffer = NEW_NT Vulkan::DepthBuffer(window_ptr->getWidth(), window_ptr->getHeight());
	result = VulkanManager::initializeImage(*mp_depth_buffer);
	assert(result == VK_SUCCESS);
}

void RenderContext::initializeRenderPass() {
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

void RenderContext::initializeFramebuffers() {
	mp_swapchain->initializeFramebuffers(m_render_pass, &(mp_depth_buffer->view), 2);
}

void RenderContext::initializeSurface() {
	VkResult result;
	result = setSurface(m_surface);
	assert(result == VK_SUCCESS);
}

void RenderContext::commandSetViewport(const Vulkan::CommandBuffer& command_buffer) {
	auto& window_manager = *WindowManager::getGlobalWindowManagerPtr();
	auto& window_ptr = window_manager.getMainWindowPtr();
	m_viewport.width = window_ptr->getWidth();
	m_viewport.height = window_ptr->getHeight();
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;
	m_viewport.x = 0;
	m_viewport.y = 0;
	vkCmdSetViewport(command_buffer.handle, 0, 1, &m_viewport);
}

void RenderContext::commandSetScissor(const Vulkan::CommandBuffer& command_buffer) {
	auto& window_manager = *WindowManager::getGlobalWindowManagerPtr();
	auto& window_ptr = window_manager.getMainWindowPtr();
	m_scissor.extent.width = window_ptr->getWidth();
	m_scissor.extent.height = window_ptr->getHeight();
	m_scissor.offset.x = 0;
	m_scissor.offset.y = 0;
	vkCmdSetScissor(command_buffer.handle, 0, 1, &m_scissor);
}