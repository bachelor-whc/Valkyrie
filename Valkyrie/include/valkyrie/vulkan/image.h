#ifndef _VULKAN_IMAGE_H
#define _VULKAN_IMAGE_H
#include <vulkan/vulkan.h>

namespace Vulkan {
	class Image {
	public:
		Image();
		virtual ~Image();

		VkImage handle = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkImageAspectFlags mask = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
		VkImageLayout oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout newLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		virtual VkImageCreateInfo getImageCreate() = 0;
		virtual VkMemoryAllocateInfo getMemoryAllocate() = 0;
		virtual VkImageViewCreateInfo getImageViewCreate() = 0;
	private:
		
	};
}

#endif