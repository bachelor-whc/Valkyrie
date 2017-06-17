#ifndef _VULKAN_ATTACHMENT_H
#define _VULKAN_ATTACHMENT_H
#include "valkyrie/common.h"

namespace Vulkan {
	struct Attachment {
		Attachment() {}
		virtual ~Attachment() {}
		virtual VkAttachmentDescription getAttachmentDescription() const = 0;

		VkFormat format = VK_FORMAT_UNDEFINED;
	};
}

#endif