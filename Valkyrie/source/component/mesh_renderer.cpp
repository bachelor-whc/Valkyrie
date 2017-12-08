#include "valkyrie/component/mesh_renderer.h"
using namespace ValkyrieComponent;

MeshRenderer::MeshRenderer(const Valkyrie::MeshPtr& mesh_ptr) :
	Recorder(),
	m_mesh_ptr(mesh_ptr) {

}

void MeshRenderer::recordDrawCommand(Vulkan::CommandBuffer& command) {
	//TODO: check command status
	//TODO: vkCmdBindVertexBuffers _2
	VkDeviceSize offsets[] = { m_mesh_ptr->getVerticeBufferOffset() };
	VkBuffer buffers[] = { m_mesh_ptr->getVulkanBuffer() };
	vkCmdBindVertexBuffers(command.handle, 0, 1, buffers, offsets);
	vkCmdBindIndexBuffer(command.handle, buffers[0], m_mesh_ptr->getIndiceBufferOffset(), VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(command.handle, m_mesh_ptr->getDrawVertexCount(), 1, 0, 0, 0);
}

