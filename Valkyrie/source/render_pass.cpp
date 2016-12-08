#include "valkyrie/vulkan/device.h"
#include "valkyrie/vulkan/render_pass.h"
using namespace Vulkan;

RenderPass::RenderPass() {

}

RenderPass::~RenderPass() {
	
}

bool RenderPass::initialize(const Device& device, const SubpassDependencies& dependencies) {
	if (attachments.size() == 0 || subpasses.size() == 0)
		return false;

	VkRenderPassCreateInfo renderpass_create = {};
	renderpass_create.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderpass_create.attachmentCount = attachments.size();
	renderpass_create.pAttachments = attachments.data();
	renderpass_create.subpassCount = subpasses.size();
	renderpass_create.pSubpasses = subpasses.data();
	renderpass_create.dependencyCount = dependencies.size();
	renderpass_create.pDependencies = dependencies.data();

	VkResult result = vkCreateRenderPass(device.handle, &renderpass_create, nullptr, &handle);
	return result == VK_SUCCESS;
}

Subpass::Subpass() :
	mp_depth_attachment(nullptr) {

}

Subpass::~Subpass() {
	if (mp_depth_attachment != nullptr)
		delete mp_depth_attachment;
}

VkSubpassDescription Subpass::createSubpassDescription() {
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.inputAttachmentCount = inputAttachmentReferences.size();
	subpass.pInputAttachments = inputAttachmentReferences.data();
	subpass.colorAttachmentCount = colorAttachmentReferences.size();
	subpass.pColorAttachments = colorAttachmentReferences.data();
	subpass.pResolveAttachments = resolveAttachmentReferences.data();
	subpass.pDepthStencilAttachment = mp_depth_attachment;
	subpass.preserveAttachmentCount = preserveAttachments.size();
	subpass.pPreserveAttachments = preserveAttachments.data();
	return subpass;
}

void Subpass::setDepthAttachmentReferences(const VkAttachmentReference& reference) {
	mp_depth_attachment = NEW_NT VkAttachmentReference;
	*mp_depth_attachment = reference;
}