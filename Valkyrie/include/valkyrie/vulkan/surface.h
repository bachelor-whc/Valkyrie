#ifndef _VULKAN_SURFACE_H
#define _VULKAN_SURFACE_H
#include <memory>
#include "common.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Instance;

	struct Surface : public Attachment {
		VkSurfaceKHR handle = NULL;
		VkSurfaceFormatKHR format = {};

		virtual VkAttachmentDescription getAttachmentDescription() const;
	};

	VkResult setSurface(Surface& surface);
	void DestroySurface(Surface& surface);
}

#endif