#include "valkyrie/asset/lavy_asset.h"
#include "valkyrie/utility/memory_chunk.h"

Valkyrie::LavyAsset::LavyAsset(const JSON& src) : 
	m_json(src),
	m_buffer_ptr(MAKE_SHARED(MemoryChunk)()){

}

uint32_t Valkyrie::LavyAsset::getDrawVertexCount() {
	return m_indices_count;
}

uint32_t Valkyrie::LavyAsset::getVerticeBufferOffset() {
	return m_vertices_byte_offset;
}

uint32_t Valkyrie::LavyAsset::getIndiceBufferOffset() {
	return m_indices_byte_offset;
}

uint32_t Valkyrie::LavyAsset::getVerticeBufferLength() {
	return m_vertices_byte_length;
}

uint32_t Valkyrie::LavyAsset::getIndiceBufferLength() {
	return m_indices_byte_length;
}
