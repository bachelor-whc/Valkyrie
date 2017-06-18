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
	const int stride = 32;
	const auto vertex_length = ptr->getVerticeBufferLength();
	const auto vertex_count = vertex_length / stride;
	const auto vertex_offset = ptr->getVerticeBufferOffset();
	std::vector<float> pos_xs(vertex_count);
	std::vector<float> pos_ys(vertex_count);
	std::vector<float> pos_zs(vertex_count);
	unsigned int index = 0;
	for (int offset = 0; offset < vertex_length; offset += stride) {
		float* v = (float*)((unsigned char*)memory_chunk.getData() + vertex_offset + offset);
		pos_xs[index] = v[0];
		pos_ys[index] = v[1];
		pos_zs[index] = v[2];
		++index;
	}
	glm::vec3 min_xyz(
		*std::min_element(pos_xs.begin(), pos_xs.end()),
		*std::min_element(pos_ys.begin(), pos_ys.end()),
		*std::min_element(pos_zs.begin(), pos_zs.end())
	);
	glm::vec3 max_xyz(
		*std::max_element(pos_xs.begin(), pos_xs.end()),
		*std::max_element(pos_ys.begin(), pos_ys.end()),
		*std::max_element(pos_zs.begin(), pos_zs.end())
	);
	ptr->mp_min_xyz = NEW_NT glm::vec3(min_xyz);
	ptr->mp_max_xyz = NEW_NT glm::vec3(max_xyz);
	return ptr;
}

MeshFactory::MeshFactory() {

}
