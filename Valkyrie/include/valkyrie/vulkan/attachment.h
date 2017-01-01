#ifndef _VULKAN_ATTACHMENT_H
#define _VULKAN_ATTACHMENT_H
#include "common.h"

namespace Vulkan {
	struct Attachment {
		virtual VkAttachmentDescription getAttachmentDescription() const = 0;
	};
}

#endif