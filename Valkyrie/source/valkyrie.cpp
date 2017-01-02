#include <imgui.h>
#include "valkyrie.h"
#include "valkyrie/vulkan/tool.h"
#include "valkyrie/vulkan/debug.h"
#include "valkyrie/UI/user_input.h"
using namespace Vulkan;

ValkyrieEngine* ValkyrieEngine::gp_valkyrie = nullptr;
bool ValkyrieEngine::SDLInitialized = false;
VkDevice g_device_handle = VK_NULL_HANDLE;
VkPhysicalDevice g_physical_device_handle = VK_NULL_HANDLE;

ValkyrieEngine::ValkyrieEngine(std::string application_name) :
	m_application_name(application_name),
	mp_window(nullptr),
	mp_swapchain(nullptr),
	mp_depth_buffer(nullptr),
	descriptorPool(8),
	m_render_pfns() {
	if(!SDLInitialized) {
		int sdl_result = SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		assert(sdl_result == 0);
		SDLInitialized = true;
	}
}

ValkyrieEngine::~ValkyrieEngine() {
	vkDestroySemaphore(m_device.handle, m_present_semaphore, nullptr);
	vkDestroyFence(m_device.handle, m_present_fence, nullptr);
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
	SDL_Quit();
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
	imgui_io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	imgui_io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	imgui_io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	imgui_io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	imgui_io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	imgui_io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	imgui_io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	imgui_io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	imgui_io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	imgui_io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	imgui_io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	imgui_io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	imgui_io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	imgui_io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	imgui_io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	imgui_io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	imgui_io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	imgui_io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	imgui_io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void ValkyrieEngine::updateUserInput(const SDL_Event& s_event) {
	auto& imgui_io = ImGui::GetIO();
	if (s_event.type == SDL_MOUSEMOTION) {
		imgui_io.MousePos.x = s_event.motion.x;
		imgui_io.MousePos.y = s_event.motion.y;
	}
	memcpy(imgui_io.MouseDown, userInput.mousePressed, 3);
	imgui_io.MouseWheel = userInput.mouseWheel;
	userInput.mouseWheel = 0.0f;
}

void ValkyrieEngine::updateTime() {
	double time = (double)SDL_GetTicks() / 1000.0;
	m_current_timestamp = time;
	m_deltatime = m_current_timestamp - m_previous_timestamp;
	m_previous_timestamp = m_current_timestamp;
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DeltaTime = (float)m_deltatime;
}

bool ValkyrieEngine::execute() {
	static SDL_Event s_event;
	while (SDL_PollEvent(&s_event)) {
		switch (s_event.type) {
		case SDL_QUIT:
			return false;
		case SDL_TEXTINPUT:
			userInput.handleSDLCharEvent(s_event);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			userInput.handleSDLKeyBoardEvent(s_event);
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			userInput.handleSDLMouseButtonEvent(s_event);
			break;
		case SDL_MOUSEWHEEL:
			userInput.handleSDLScrollEvent(s_event);
			break;
		}
		updateUserInput(s_event);
	}
	updateTime();
	render();
	return true;
}

VkResult ValkyrieEngine::initialize() {
	VkResult result;
	gp_valkyrie = this;

	SDL_StartTextInput();

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

	VkFenceCreateInfo present_fence_create = {};
	present_fence_create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	present_fence_create.pNext = nullptr;
	present_fence_create.flags = 0;
	result = vkCreateFence(m_device.handle, &present_fence_create, nullptr, &m_present_fence);
	assert(result == VK_SUCCESS);

	VkSemaphoreCreateInfo present_semaphore_create = {};
	present_semaphore_create.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	present_semaphore_create.pNext = nullptr;
	present_semaphore_create.flags = 0;
	result = vkCreateSemaphore(m_device.handle, &present_semaphore_create, nullptr, &m_present_semaphore);
	assert(result == VK_SUCCESS);

	return VK_SUCCESS;
}

VkResult ValkyrieEngine::render() {
	VkResult result;

	result = mp_swapchain->acquireNextImage(UINT64_MAX, m_present_semaphore, m_present_fence);
	assert(result == VK_SUCCESS);

	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &m_present_semaphore;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &renderCommands[mp_swapchain->getCurrent()].handle;

	result = vkQueueSubmit(m_graphics_queue.handle, 1, &submit, m_present_fence);
	assert(result == VK_SUCCESS);

	do {
		result = vkWaitForFences(m_device.handle, 1, &m_present_fence, VK_TRUE, 100000000);
	} while (result == VK_TIMEOUT);
	assert(result == VK_SUCCESS);
	vkResetFences(m_device.handle, 1, &m_present_fence);

	result = mp_swapchain->queuePresent(m_graphics_queue);
	assert(result == VK_SUCCESS);

	return VK_SUCCESS;
}

void ValkyrieEngine::initializeWindow(int width, int height, const std::string & title) {
	const SDL_WindowFlags flags = SDL_WINDOW_SHOWN;
	mp_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
	auto& imgui_io = ImGui::GetIO();
	imgui_io.DisplaySize.x = width;
	imgui_io.DisplaySize.y = height;
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
	pipelines[pipename_name] = MAKE_SHARED(Vulkan::PipelineModule)();
	vertexInputs[pipename_name] = MAKE_SHARED(Vulkan::VertexInput)();
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
	SDL_GetWindowSize(mp_window, &width, &height);
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
	SDL_GetWindowSize(mp_window, &width, &height);
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
