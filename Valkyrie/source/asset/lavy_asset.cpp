#include "valkyrie/asset/lavy_asset.h"
#include "valkyrie/utility/memory_chunk.h"
using namespace Valkyrie;

LavyAsset::LavyAsset(const JSON& src) : 
	m_json(src),
	m_buffer_ptr(MAKE_SHARED(MemoryChunk)()){

}

LavyAsset::~LavyAsset() {

}

uint32_t LavyAsset::getDrawVertexCount() {
	return m_indices_count;
}

uint32_t LavyAsset::getVerticeBufferOffset() {
	return m_vertices_byte_offset;
}

uint32_t LavyAsset::getIndiceBufferOffset() {
	return m_indices_byte_offset;
}

uint32_t LavyAsset::getVerticeBufferLength() {
	return m_vertices_byte_length;
}

uint32_t LavyAsset::getIndiceBufferLength() {
	return m_indices_byte_length;
}
