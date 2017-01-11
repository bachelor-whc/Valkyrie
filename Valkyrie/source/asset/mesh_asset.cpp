#include "valkyrie/asset/mesh_asset.h"
using namespace Valkyrie;

Mesh::Mesh(const MeshSupportPtr& mesh_ptr) :
	m_vulkan_buffer(),
	m_mesh_support_ptr(mesh_ptr) {

}

uint32_t Valkyrie::Mesh::getDrawVertexCount() {
	return m_mesh_support_ptr->getDrawVertexCount();
}

uint32_t Valkyrie::Mesh::getVerticeBufferOffset() {
	return m_mesh_support_ptr->getVerticeBufferOffset();
}

uint32_t Valkyrie::Mesh::getIndiceBufferOffset() {
	return m_mesh_support_ptr->getIndiceBufferOffset();
}

uint32_t Valkyrie::Mesh::getVerticeBufferLength() {
	return m_mesh_support_ptr->getVerticeBufferLength();
}

uint32_t Valkyrie::Mesh::getIndiceBufferLength() {
	return m_mesh_support_ptr->getIndiceBufferLength();
}
