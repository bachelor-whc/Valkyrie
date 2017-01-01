#ifndef _VULKAN_RENDER_PASS_H
#define _VULKAN_RENDER_PASS_H
#include "common.h"

namespace Vulkan {
	typedef std::vector<VkSubpassDependency> SubpassDependencies;
	typedef std::vector<VkAttachmentReference> AttachmentReferences;
	struct Device;

	struct Subpass {
		Subpass();
		~Subpass();

		VkSubpassDescription createSubpassDescription();

		void setDepthAttachmentReferences(const VkAttachmentReference& reference);
		AttachmentReferences colorAttachmentReferences;
		AttachmentReferences inputAttachmentReferences;
		AttachmentReferences resolveAttachmentReferences;
		std::vector<uint32_t> preserveAttachments;

	private:
		VkAttachmentReference* mp_depth_attachment;
	};

	struct RenderPass {
		RenderPass();
		~RenderPass();

		bool initialize(const SubpassDependencies& dependencies);
		
		VkRenderPass handle = NULL;
		std::vector<VkSubpassDescription> subpasses;
		std::vector<VkAttachmentDescription> attachments;
	private:
		
	};
}

#endif