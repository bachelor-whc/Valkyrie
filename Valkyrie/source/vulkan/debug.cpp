#include "valkyrie/vulkan/debug.h"
#include "valkyrie/vulkan/instance.h"
#include "valkyrie/utility/vulkan_manager.h"

VkDebugReportCallbackEXT message_callback;

VkBool32 Vulkan::DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT object_type,
	uint64_t source_object,
	size_t location,
	int32_t code,
	const char* p_layer,
	const char* p_message,
	void* p_user_data) {
	std::string message(p_message);
	std::cout << message << std::endl;
	return false;
}

void Vulkan::SetupDebug(VkDebugReportFlagsEXT flags) {
	VkDebugReportCallbackCreateInfoEXT debug_report_callback_create = {};
	debug_report_callback_create.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	debug_report_callback_create.pfnCallback = (PFN_vkDebugReportCallbackEXT)Vulkan::DebugCallback;
	debug_report_callback_create.flags = flags;

	const auto& instance = Valkyrie::VulkanManager::getInstance();
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	VkResult result = CreateDebugReportCallback(instance, &debug_report_callback_create, nullptr, &message_callback);
	assert(result == VK_SUCCESS);
}