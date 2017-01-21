#include "valkyrie/vulkan/instance.h"
#include "common.h"
using namespace Vulkan;
const std::vector<const char*> g_instance_extensions = { VK_KHR_SURFACE_EXTENSION_NAME, SURFACE_EXTENSION_NAME, "VK_EXT_debug_report" };
const std::vector<const char*> g_instance_layers = { "VK_LAYER_LUNARG_standard_validation" };

VkResult Vulkan::CreateInstance(const char* application_name, Instance& instance) {
	VkApplicationInfo application = {};
	application.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	application.pApplicationName = application_name;
	application.applicationVersion = 1;
	application.pEngineName = "Valkyrie";
	application.engineVersion = 1;
	application.apiVersion = VK_API_VERSION_1_0;

	uint32_t instance_extension_count;
	VkInstanceCreateInfo instance_create = {};
	instance_create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_create.pApplicationInfo = &application;
	instance_create.enabledExtensionCount = g_instance_extensions.size();
	instance_create.ppEnabledExtensionNames = g_instance_extensions.data();
	instance_create.enabledLayerCount = g_instance_layers.size();
	instance_create.ppEnabledLayerNames = g_instance_layers.data();

	VkResult result;
	result = vkCreateInstance(&instance_create, NULL, &instance.handle);
	return result;
}

void Vulkan::DestroyInstance(Instance& instance)
{
	vkDestroyInstance(instance.handle, nullptr);
}