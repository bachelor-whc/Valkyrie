#include "valkyrie/factory/mesh.h"
#include "common.h"
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

MeshFactory::~MeshFactory() {

}

MeshPtr MeshFactory::createMesh(const LavyAssetPtr lavy_ptr) {
	MeshPtr ptr = MAKE_SHARED(Mesh)(lavy_ptr);
	auto& lavy = *lavy_ptr;
	auto& buffer = ptr->m_vulkan_buffer;
	auto& memory_chunk = *lavy_ptr->m_buffer_ptr;
	buffer.allocate( 
		{ VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
		lavy.m_buffer_ptr->getSize()
	);
	buffer.write(memory_chunk.getData(), 0, memory_chunk.getSize());
	return ptr;
}

MeshFactory::MeshFactory() {

}
