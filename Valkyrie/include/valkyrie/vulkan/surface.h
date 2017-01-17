#ifndef _VULKAN_SURFACE_H
#define _VULKAN_SURFACE_H
#include <memory>
#include "common.h"
#include "valkyrie/UI/window.h"
#include "valkyrie/vulkan/attachment.h"

namespace Vulkan {
	struct PhysicalDevice;
	struct Instance;

	struct Surface : public Attachment {
		VkSurfaceKHR handle = NULL;
		VkSurfaceFormatKHR format = {};

		virtual VkAttachmentDescription getAttachmentDescription() const;
	};

	VkResult setSurface(Surface& surface, const Valkyrie::WindowPtr& window_ptr);
	void DestroySurface(Surface& surface);
}

#endif