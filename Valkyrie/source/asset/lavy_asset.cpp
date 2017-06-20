#include "valkyrie/asset/lavy_asset.h"
#include "valkyrie/utility/memory_chunk.h"
using namespace Valkyrie;

LavyMesh::LavyMesh(const std::string& name, const JSON& src) :
	m_json(src),
	m_name(name),
	m_buffer_ptr(nullptr){

}

LavyMesh::~LavyMesh() {

}

uint32_t LavyMesh::getDrawVertexCount() {
	return m_indices_count;
}

uint32_t LavyMesh::getVerticeBufferOffset() {
	return m_vertices_byte_offset;
}

uint32_t LavyMesh::getIndiceBufferOffset() {
	return m_indices_byte_offset;
}

uint32_t LavyMesh::getVerticeBufferLength() {
	return m_vertices_byte_length;
}

uint32_t LavyMesh::getIndiceBufferLength() {
	return m_indices_byte_length;
}
