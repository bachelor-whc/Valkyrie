#ifndef _VULKAN_SURFACE_H
#define _VULKAN_SURFACE_H
#include <memory>
#include "utility.h"
#include "wendy.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Instance;

	struct Surface : public Attachment {
		VkSurfaceKHR handle = NULL;
		VkSurfaceFormatKHR format = {};

		virtual VkAttachmentDescription getAttachmentDescription() const;
	};

#ifdef WIN32
	VkResult setSurface(Surface& surface, const Wendy::Win32Window& window, const Instance& instance, const PhysicalDevice& physical_device);
#endif
	void DestroySurface(Instance& instance, Surface& surface);
}

#endif