#ifndef _VALKYRIE_VULKAN_MANAGER_H
#define _VALKYRIE_VULKAN_MANAGER_H
#include "valkyrie/vulkan/instance.h"
#include "valkyrie/vulkan/physical_device.h"
#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/queue.h"
#include "valkyrie/vulkan/surface.h"
#include "valkyrie/vulkan/command_buffer.h"
#include "valkyrie/vulkan/memory_buffer.h"
#include "valkyrie/vulkan/shader.h"
#include "valkyrie/vulkan/command_pool.h"
#include "valkyrie/vulkan/texture.h"
#include "valkyrie/vulkan/image.h"

namespace Valkyrie {
	class VulkanManager {
	public:
		static int initialize();
		static void close();
		static bool initialized();
		static VulkanManager& instance() { return *gp_vulkan_manager; }
		static VkInstance getInstance();
		static VkPhysicalDevice getPhysicalDevice();
		static VkDevice getDevice();
		static VkResult initializeImage(Vulkan::Image& image);
		static VkResult initializeTexture(Vulkan::Texture& texture);
		static VkResult setImageLayout(const Vulkan::Image& image);
		static bool getSupportFormat(VkFormat& format, const VkFlags flag);
		static inline Vulkan::Queue getGraphicsQueue() { return gp_vulkan_manager->m_graphics_queue; }
		static Vulkan::CommandBuffer createCommandBuffer();
		static Vulkan::CommandPoolPtr createCommandPool(VkQueueFlags queue);
		virtual ~VulkanManager();

	private:
		static Vulkan::CommandBuffer getSetupCommandBuffer();
		static VkResult allocateMemory(Vulkan::Image& image);

		void initializeVulkan();
		void initializeInstance();
		void initializePhysicalDevice();
		void initializeDevice();
		void initializePipelineCache();
		void initializeCommandPool();

		static VulkanManager* gp_vulkan_manager;
		VulkanManager();

		Vulkan::Instance m_instatnce;
		Vulkan::PhysicalDevice m_physical_device;
		Vulkan::Device m_device;
		Vulkan::Queue m_graphics_queue;
		Vulkan::CommandBuffer m_setup_command_buffer;
		Vulkan::CommandPoolPtr m_command_pool_ptr;
	};
}

#endif