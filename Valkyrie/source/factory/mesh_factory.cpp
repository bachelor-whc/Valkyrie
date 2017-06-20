#include "valkyrie/common.h"
#include "valkyrie/factory/mesh.h"
#include "valkyrie/utility/memory_chunk.h"
using namespace Valkyrie;
using namespace ValkyrieFactory;
MeshFactory* MeshFactory::gp_mesh_factory = nullptr;

int MeshFactory::initialize() {
	if (gp_mesh_factory != nullptr) {
		return 0;
	}
	gp_mesh_factory = NEW_NT MeshFactory();
	if (gp_mesh_factory == nullptr)
		return 1;
	return 0;
}

void MeshFactory::close() {
	if (gp_mesh_factory != nullptr) {
		delete gp_mesh_factory;
		gp_mesh_factory = nullptr;
	}
}

bool MeshFactory::initialized() {
	return gp_mesh_factory != nullptr;
}

MeshFactory::~MeshFactory() {

}

MeshPtr MeshFactory::createMesh(const LavyMeshPtr lavy_ptr) {
	MeshPtr ptr = MAKE_SHARED(Mesh)(lavy_ptr);
	auto& lavy = *lavy_ptr;
	auto& buffer = ptr->m_vulkan_buffer;
	auto& memory_chunk = *lavy_ptr->m_buffer_ptr;
	buffer.allocate( 
		{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		lavy.m_buffer_ptr->getSize()
	);
	buffer.write(memory_chunk.getData(), 0, memory_chunk.getSize());
	const int stride = 32;
	const auto vertex_length = ptr->getVerticeBufferLength();
	const auto vertex_count = vertex_length / stride;
	const auto vertex_offset = ptr->getVerticeBufferOffset();
	ptr->boundingBox.min = lavy_ptr->m_bounding_box.min;
	ptr->boundingBox.max = lavy_ptr->m_bounding_box.max;
	return ptr;
}

MeshFactory::MeshFactory() {

}
