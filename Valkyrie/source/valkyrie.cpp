#include <imgui.h>
#include "valkyrie.h"
#include "valkyrie/vulkan/tool.h"
#include "valkyrie/vulkan/debug.h"
#include "valkyrie/UI/user_input.h"
using namespace Vulkan;

ValkyrieEngine* ValkyrieEngine::gp_valkyrie = nullptr;
VkDevice g_device_handle = VK_NULL_HANDLE;
VkPhysicalDevice g_physical_device_handle = VK_NULL_HANDLE;

void glfwRefreshCallback(GLFWwindow * window) {
	ValkyrieEngine::getGlobalValkyriePtr()->render();
}

ValkyrieEngine::ValkyrieEngine(std::string application_name) :
	m_application_name(application_name),
	mp_window(nullptr),
	mp_swapchain(nullptr),
	mp_depth_buffer(nullptr),
	descriptorPool(8),
	m_render_pfns() {

}

ValkyrieEngine::~ValkyrieEngine() {
	DestroyPipelineCache();
	DestroyFramebuffers(*mp_swapchain->getFramebuffers());
	DestroySwapChain(*mp_swapchain);
	for (auto& ph : m_thread_ptrs)
		delete ph;
	DestroySurface(m_instatnce, m_surface);
	DestroyDevice(m_device);
	DestroyInstance(m_instatnce);
	delete mp_swapchain;
	delete mp_depth_buffer;
	glfwDestroyWindow(mp_window);
}

void ValkyrieEngine::initializeInstance() {
	VkResult result;
	result = CreateInstance(m_application_name.c_str(), m_instatnce);
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializePhysicalDevice() {
	VkResult result;
	result = CreatePhysicalDevice(m_instatnce, m_physical_device);
	g_physical_device_handle = m_physical_device.handle;
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeDevice() {
	VkResult result;
	result = CreateDevice(m_device);
	g_device_handle = m_device.handle;
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeSurface() {
	VkResult result;
	result = setSurface(m_surface, mp_window, m_instatnce);
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeThreads() {
	bool queue_got = GetQueue(VK_QUEUE_GRAPHICS_BIT, m_graphics_queue);
	assert(queue_got == true);

	ThreadPointer p_thread = NEW_NT Valkyrie::Thread(m_graphics_queue);
	m_thread_ptrs.push_back(p_thread);
}

void ValkyrieEngine::initializeSwapChain(CommandBuffer& command_bufer) {
	VkResult result;
	mp_swapchain = NEW_NT SwapChain(m_surface, mp_window);
	result = mp_swapchain->initializeImages(m_surface, command_bufer);
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeDepthBuffer(CommandBuffer& command_bufer) {
	VkResult result;
	mp_depth_buffer = NEW_NT DepthBuffer();
	result = mp_depth_buffer->initializeImages(command_bufer, mp_window);
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeRenderPass() {
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
	bool initialized = m_render_pass.initialize(dependencies);
	assert(initialized);
}

void ValkyrieEngine::initializeFramebuffers() {
	mp_swapchain->initializeFramebuffers(m_render_pass, &(mp_depth_buffer->view), 1);
}

void ValkyrieEngine::initializePipelineCache() {
	VkResult result;
	result = PipelineModule::initializeCache();
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeImGuiInput() {
	auto& imgui_io = ImGui::GetIO();
	imgui_io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	imgui_io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	imgui_io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	imgui_io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	imgui_io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	imgui_io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	imgui_io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	imgui_io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	imgui_io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	imgui_io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	imgui_io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	imgui_io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	imgui_io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	imgui_io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	imgui_io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	imgui_io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	imgui_io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	imgui_io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	imgui_io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	glfwSetKeyCallback(mp_window, GLFWKeyBoardCallback);
	glfwSetMouseButtonCallback(mp_window, GLFWMouseButtonCallback);
	glfwSetCharCallback(mp_window, GLFWCharCallback);
	glfwSetScrollCallback(mp_window, GLFWScrollCallback);
}

void ValkyrieEngine::updateUserInput() {
	auto& imgui_io = ImGui::GetIO();
	if(glfwGetWindowAttrib(mp_window, GLFW_FOCUSED)) {
		double mouse_x, mouse_y;
		glfwGetCursorPos(mp_window, &mouse_x, &mouse_y);
		imgui_io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
	}
	else
		imgui_io.MousePos = ImVec2(-1, -1);
	for (int i = 0; i < 3; ++i) {
		imgui_io.MouseDown[i] = userInput.mousePressed[i];
	}
	imgui_io.MouseWheel = userInput.mouseWheel;
	userInput.mouseWheel = 0.0f;
}

void ValkyrieEngine::updateTime() {
	m_current_timestamp = glfwGetTime();
	m_deltatime = m_current_timestamp - m_previous_timestamp;
	m_previous_timestamp = m_current_timestamp;
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DeltaTime = (float)m_deltatime;
}

bool ValkyrieEngine::execute() {
	if(!glfwWindowShouldClose(mp_window)) {
		glfwPollEvents();
		updateUserInput();
		updateTime();
		render();
		return true;
	}
	return false;
}

VkResult ValkyrieEngine::initialize() {
	VkResult result;
	gp_valkyrie = this;

	assert(mp_window != nullptr);
	initializeInstance();

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

	initializeImGuiInput();

	return VK_SUCCESS;
}

VkResult ValkyrieEngine::render() {
	VkResult result;

	VkSemaphore present_semaphore;
	VkSemaphoreCreateInfo present_semaphore_create = {};
	present_semaphore_create.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	present_semaphore_create.pNext = NULL;
	present_semaphore_create.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	result = vkCreateSemaphore(m_device.handle, &present_semaphore_create, nullptr, &present_semaphore);
	assert(result == VK_SUCCESS);

	result = mp_swapchain->acquireNextImage(UINT64_MAX, present_semaphore, NULL);
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

void ValkyrieEngine::initializeWindow(int width, int height, const std::string & title) {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	mp_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
}

void ValkyrieEngine::initializePipelineLayout(const std::string& pipeline_name) {
	VkResult result;
	assert(pipelines.count(pipeline_name) > 0);
	std::vector<VkDescriptorSetLayout>& set_layouts = descriptorPool.getSetLayoutHandles();
	result = pipelines[pipeline_name]->initializeLayout(set_layouts);
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeDescriptorSetLayouts() {
	VkResult result;
	result = descriptorPool.initializeSetLayouts();
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::createPipelineModule(const std::string & pipename_name) {
	pipelines[pipename_name] = std::make_shared<Vulkan::PipelineModule>();
	vertexInputs[pipename_name] = std::make_shared<Vulkan::VertexInput>();
}

void ValkyrieEngine::initializeShaderModules() {
	VkResult result;
	for (auto& key_value : shaders) {
		auto& shader_ptr = key_value.second;
		result = shader_ptr->initializeModule();
		assert(result == VK_SUCCESS);
	}
}

void ValkyrieEngine::initializePipeline(const std::string& pipename_name) {
	VkResult result;
	if (pipelines.find(pipename_name) != pipelines.end() && vertexInputs.find(pipename_name) != vertexInputs.end()) {
		auto& pipeline_ptr = pipelines[pipename_name];
		auto& vertex_input_ptr = vertexInputs[pipename_name];
		pipeline_ptr->setVertexInput(*vertex_input_ptr);
		result = pipeline_ptr->initialize();
		assert(result == VK_SUCCESS);
	}
}

void ValkyrieEngine::initializeDescriptorPool() {
	VkResult result = descriptorPool.initializePool();
	assert(result == VK_SUCCESS);
}

void ValkyrieEngine::initializeDescriptorSets() {
	VkResult result;
	result = descriptorPool.initializeSets();
	assert(result == VK_SUCCESS);
}
void ValkyrieEngine::writeSets(const std::vector<VkWriteDescriptorSet>& writes) {
	vkUpdateDescriptorSets(m_device.handle, writes.size(), writes.data(), 0, NULL);
}

void ValkyrieEngine::commandSetViewport(const Vulkan::CommandBuffer& command_buffer) {
	int width;
	int height;
	glfwGetWindowSize(mp_window, &width, &height);
	m_viewport.width = (float)width;
	m_viewport.height = (float)height;
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;
	m_viewport.x = 0;
	m_viewport.y = 0;
	vkCmdSetViewport(command_buffer.handle, 0, 1, &m_viewport);
}

void ValkyrieEngine::commandSetScissor(const Vulkan::CommandBuffer& command_buffer) {
	int width;
	int height;
	glfwGetWindowSize(mp_window, &width, &height);
	m_scissor.extent.width = (uint32_t)width;
	m_scissor.extent.height = (uint32_t)height;
	m_scissor.offset.x = 0;
	m_scissor.offset.y = 0;
	vkCmdSetScissor(command_buffer.handle, 0, 1, &m_scissor);
}

void ValkyrieEngine::initailizeTexture(Vulkan::Texture& texture) {
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
	setImageLayout(m_setup_command_buffer, texture.image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_PREINITIALIZED, texture.layout);
	m_setup_command_buffer.end();
	result = m_setup_command_buffer.submit(m_graphics_queue);
	assert(result == VK_SUCCESS);
}

bool ValkyrieEngine::registerRenderFunction(std::string name, Valkyrie::RenderPFN pfn) {
	if(m_render_pfns.find(name) != m_render_pfns.end())
		return false;
	else {
		m_render_pfns[name] = pfn;
	}
	return true;
}

void ValkyrieEngine::executeRenderFunction(std::string name, const std::vector<void*>& data) {
	m_render_pfns[name]->render(data, mp_swapchain->getCurrent());
}

Vulkan::CommandBuffer ValkyrieEngine::createCommandBuffer() {
	return m_thread_ptrs[0]->createCommandBuffer();
}

Vulkan::SecondaryCommandBuffers ValkyrieEngine::createSecondaryCommandBuffers(uint32_t count) {
	return m_thread_ptrs[0]->createSecondaryCommandBuffers(count);
}
