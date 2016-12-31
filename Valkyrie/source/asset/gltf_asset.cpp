#include "valkyrie/asset/gltf_asset.h"
#include "valkyrie/memory_chunk.h"

Valkyrie::glTFAsset::glTFAsset(const JSON& src) : m_json(src) {

}

void Valkyrie::glTFAsset::setJSON(const JSON& src) {
	m_json = src;
}

Valkyrie::glTFBuffer::glTFBuffer(const Valkyrie::MemoryChunkPtr& mcptr) : m_memory_chunk_ptr(mcptr) {

}

Valkyrie::glTFBuffer::~glTFBuffer() {

}

Valkyrie::glTFBufferView::glTFBufferView(const glTFBufferPtr& buffer_ptr, const uint32_t length, const uint32_t offset) :
	m_buffer_ptr(buffer_ptr),
	m_length(length),
	m_offset(offset) {

}

Valkyrie::glTFBufferView::~glTFBufferView() {

}

Valkyrie::glTFAccessor::glTFAccessor(const glTFBufferViewPtr & buffer_view_ptr, const GAPIAttributeSupportPtr& gaas_ptr, GLTF_TYPE type, GLTF_COMPONENT_TYPE component_type) :
	m_buffer_view_ptr(buffer_view_ptr),
	m_type(type),
	m_component_type(component_type),
	m_attribute_ptr(gaas_ptr) {
	
}

Valkyrie::glTFAccessor::~glTFAccessor() {
}
