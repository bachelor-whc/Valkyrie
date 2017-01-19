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

		virtual VkImageCreateInfo getImageCreate() const = 0;
		virtual VkImageViewCreateInfo getImageViewCreate() const = 0;
		virtual VkFlags getMemoryType() const = 0;
	private:
		
	};
}

#endif