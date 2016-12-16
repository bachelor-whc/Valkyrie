#include "valkyrie.h"
#include "valkyrie/vulkan/tool.h"
#include "valkyrie/vulkan/debug.h"
using namespace Vulkan;

Valkyrie* Valkyrie::gp_valkyrie = nullptr;

Valkyrie::Valkyrie(std::string application_name) :
	m_application_name(application_name),
	mp_window(nullptr),
	mp_swapchain(nullptr),
	mp_depth_buffer(nullptr),
	descriptorPool(8) {
#ifdef _WIN32
	
#endif
}

Valkyrie::~Valkyrie() {
	DestroyPipelineLayout(m_device);
	DestroyPipelineCache(m_device);
	DestroyFramebuffers(m_device, *mp_swapchain->getFramebuffers());
	DestroySwapChain(m_device, *mp_swapchain);
	for (auto& ph : m_thread_ptrs)
		delete ph;
	DestroySurface(m_instatnce, m_surface);
	DestroyDevice(m_device);
	DestroyInstance(m_instatnce);
}

void Valkyrie::initializeInstance() {
	VkResult result;
	result = CreateInstance(m_application_name.c_str(), m_instatnce);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializePhysicalDevice() {
	VkResult result;
	result = CreatePhysicalDevice(m_instatnce, m_physical_device);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeDevice() {
	VkResult result;
	result = CreateDevice(m_physical_device, m_device);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeSurface() {
	VkResult result;
	result = setSurface(m_surface, *mp_window, m_instatnce, m_physical_device);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeThreads() {
	bool queue_got = GetQueue(m_device, m_physical_device, VK_QUEUE_GRAPHICS_BIT, m_graphics_queue);
	assert(queue_got == true);

	ThreadPointer p_thread = NEW_NT ValkyrieThread(m_device, m_graphics_queue);
	m_thread_ptrs.push_back(p_thread);
}

void Valkyrie::initializeSwapChain(CommandBuffer& command_bufer) {
	VkResult result;
	mp_swapchain = new SwapChain(m_device, m_physical_device, m_surface, *mp_window);
	result = mp_swapchain->initializeImages(m_device, m_surface, command_bufer);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeDepthBuffer(CommandBuffer& command_bufer) {
	VkResult result;
	mp_depth_buffer = new DepthBuffer(m_physical_device);
	result = mp_depth_buffer->initializeImages(m_device, m_physical_device, command_bufer, *mp_window);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeRenderPass() {
	m_render_pass.attachments.push_back(m_surface.getAttachmentDescription());
	m_render_pass.attachments.push_back(mp_depth_buffer->getAttachmentDescription());

	Subpass subpass;

	VkAttachmentReference color_reference = {};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_reference = {};
	depth_reference.attachment = 1;
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	subpass.colorAttachmentReferences.push_back(color_reference);
	subpass.setDepthAttachmentReferences(depth_reference);

	m_render_pass.subpasses.push_back(subpass.createSubpassDescription());

	SubpassDependencies dependencies;
	bool initialized = m_render_pass.initialize(m_device, dependencies);
	assert(initialized);
}

void Valkyrie::initializeFramebuffers() {
	mp_swapchain->initializeFramebuffers(m_device, m_render_pass, &(mp_depth_buffer->view), 1);
}

void Valkyrie::initializePipelineCache() {
	VkResult result;
	result = Pipeline::initializeCache(m_device);
	assert(result == VK_SUCCESS);
}

VkResult Valkyrie::initialize() {
	VkResult result;
	gp_valkyrie = this;

	assert(mp_window != nullptr);
	initializeInstance();

	//Vulkan::SetupDebug(m_instatnce, VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT, NULL);

	initializePhysicalDevice();
	initializeDevice();
	initializeSurface();
	initializeThreads();
	
	m_setup_command_buffer = m_thread_ptrs[0]->createCommandBuffer();
	m_present_command_buffer = m_thread_ptrs[0]->createCommandBuffer();

	result = m_setup_command_buffer.begin();
	assert(result == VK_SUCCESS);

	initializeSwapChain(m_setup_command_buffer);
	initializeDepthBuffer(m_setup_command_buffer);
	
	result = m_setup_command_buffer.end();
	assert(result == VK_SUCCESS);

	result = m_setup_command_buffer.submit(m_graphics_queue);
	assert(result == VK_SUCCESS);

	initializeRenderPass();
	initializeFramebuffers();
	initializePipelineCache();
	renderCommands.resize(mp_swapchain->getImageCount());
	for (auto& command : renderCommands) {
		command = m_thread_ptrs[0]->createCommandBuffer();
	}

	m_state = INITIALIZED;
	return VK_SUCCESS;
}

VkResult Valkyrie::render() {
	VkResult result;

	VkSemaphore present_semaphore;
	VkSemaphoreCreateInfo present_semaphore_create = {};
	present_semaphore_create.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	present_semaphore_create.pNext = NULL;
	present_semaphore_create.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	result = vkCreateSemaphore(m_device.handle, &present_semaphore_create, nullptr, &present_semaphore);
	assert(result == VK_SUCCESS);

	result = mp_swapchain->acquireNextImage(m_device, UINT64_MAX, present_semaphore, NULL);
	assert(result == VK_SUCCESS);

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &present_semaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &renderCommands[mp_swapchain->getCurrent()].handle;

	result = vkQueueSubmit(m_graphics_queue.handle, 1, &submit, VK_NULL_HANDLE);
	assert(result == VK_SUCCESS);

	result = mp_swapchain->queuePresent(m_graphics_queue);
	assert(result == VK_SUCCESS);

	VkImageMemoryBarrier present_barrier = {};
	present_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	present_barrier.pNext = NULL;
	present_barrier.srcAccessMask = 0;
	present_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	present_barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	present_barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	present_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	present_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	present_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	present_barrier.image = mp_swapchain->getCurrentImage();

	result = m_present_command_buffer.begin();

	vkCmdPipelineBarrier(
		m_present_command_buffer.handle,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &present_barrier);

	result = m_present_command_buffer.end();

	assert(result == VK_SUCCESS);
	m_present_command_buffer.submit(m_graphics_queue);

	vkDestroySemaphore(m_device.handle, present_semaphore, nullptr);

	return VK_SUCCESS;
}

bool Valkyrie::execute() {
	if (m_state == INITIALIZED)
		m_state = EXECUTE;

	static MSG message;
	PeekMessage(&message, NULL, 0, 0, PM_REMOVE);
	if (message.message == WM_QUIT) {
		return false;
	}
	else {
		TranslateMessage(&message);
		DispatchMessage(&message);
		VkResult result = render();
		assert(result == VK_SUCCESS);
	}
	return m_state == EXECUTE;
}

#ifdef WIN32
void Valkyrie::setWindowPointer(Wendy::Win32Window* window_ptr) {
#endif
	if(window_ptr != nullptr)
		mp_window = window_ptr;
}

void Valkyrie::initializePipelineLayout() {
	VkResult result;
	std::vector<DescriptorSetLayout> layouts = { descriptorPool.setLayout };
	result = Pipeline::initializeLayout(m_device, layouts);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeDescriptorSetLayout() {
	VkResult result;
	result = descriptorPool.setLayout.initialize(m_device);
	assert(result == VK_SUCCESS);
}

void Valkyrie::allocateMemoryBuffer(Vulkan::MemoryBuffer& buffer, const VkBufferUsageFlags usage, uint32_t size) {
	VkResult result = buffer.allocate(m_device, m_physical_device, usage, size);
	assert(result == VK_SUCCESS);
}

void Valkyrie::destroyMemoryBuffer(Vulkan::MemoryBuffer& buffer) {
	vkDestroyBuffer(m_device.handle, buffer.handle, nullptr);
}

void Valkyrie::writeMemoryBuffer(Vulkan::MemoryBuffer& buffer, const void *data, uint32_t offset) {
	VkResult result = buffer.write(m_device, data, offset);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeShaderModules() {
	VkResult result;
	for (auto& key_value : shaders) {
		auto& shader_ptr = key_value.second;
		result = shader_ptr->initializeModule(m_device);
		assert(result == VK_SUCCESS);
	}
}

void Valkyrie::initializePipelines() {
	VkResult result;
	for (auto& key_value : pipelines) {
		auto& pipeline_ptr = key_value.second;
		pipeline_ptr->setRenderPass(m_render_pass, 0);
		pipeline_ptr->setVertexInput(vertexInput);
		result = pipeline_ptr->initialize(m_device);
		assert(result == VK_SUCCESS);
	}
}

void Valkyrie::initializeDescriptorPool() {
	VkResult result = descriptorPool.initializePool(m_device);
	assert(result == VK_SUCCESS);
}

void Valkyrie::initializeDescriptorSets() {
	VkResult result;
	result = descriptorPool.initializeSet(m_device);
	assert(result == VK_SUCCESS);
}
void Valkyrie::writeSets(const std::vector<VkWriteDescriptorSet>& writes) {
	vkUpdateDescriptorSets(m_device.handle, writes.size(), writes.data(), 0, NULL);
}

void Valkyrie::commandSetViewport(const Vulkan::CommandBuffer& command_buffer) {
	m_viewport.width = (float)mp_window->getWidth();
	m_viewport.height = (float)mp_window->getHeight();
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;
	m_viewport.x = 0;
	m_viewport.y = 0;
	vkCmdSetViewport(command_buffer.handle, 0, 1, &m_viewport);
}

void Valkyrie::commandSetScissor(const Vulkan::CommandBuffer& command_buffer) {
	m_scissor.extent.width = (float)mp_window->getWidth();
	m_scissor.extent.height = (float)mp_window->getHeight();
	m_scissor.offset.x = 0;
	m_scissor.offset.y = 0;
	vkCmdSetScissor(command_buffer.handle, 0, 1, &m_scissor);
}

void Valkyrie::initailizeTexture(Vulkan::Texture& texture) {
	VkResult result;
	result = texture.initializeImage(m_device);
	assert(result == VK_SUCCESS);
	result = texture.allocate(m_device, m_physical_device);
	assert(result == VK_SUCCESS);
	result = texture.write(m_device);
	assert(result == VK_SUCCESS);
	result = texture.initializeSampler(m_device);
	assert(result == VK_SUCCESS);
	result = texture.initializeView(m_device);
	assert(result == VK_SUCCESS);
	result = m_setup_command_buffer.begin();
	assert(result == VK_SUCCESS);
	setImageLayout(m_setup_command_buffer, texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, texture.layout);
	m_setup_command_buffer.end();
	result = m_setup_command_buffer.submit(m_graphics_queue);
	assert(result == VK_SUCCESS);
}