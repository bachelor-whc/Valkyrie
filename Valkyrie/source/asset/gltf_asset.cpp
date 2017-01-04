#include "valkyrie/asset/gltf_asset.h"
#include "valkyrie/memory_chunk.h"

Valkyrie::glTFAsset::glTFAsset(const JSON& src) : m_json(src) {

}

void Valkyrie::glTFAsset::setJSON(const JSON& src) {
	m_json = src;
}

void Valkyrie::glTFAsset::setBufferView(const std::string& name, const glTFBufferViewPtr& buffer_view) {
	if (bufferViewSet.count(name) > 0)
		m_buffer_view_ptrs[name] = buffer_view;
}

void Valkyrie::glTFAsset::setAccessor(const std::string& name, const glTFAccessorPtr& accessor) {
	if (accessorSet.count(name) > 0)
		m_accessor_ptrs[name] = accessor;
}

Valkyrie::glTFAccessorPtr Valkyrie::glTFAsset::getAccessor(const std::string& name) throw(...) {
	if (m_accessor_ptrs.count(name) < 1) {
		std::string ex_message = name + " accessor not found.";
		throw std::exception(ex_message.c_str());
	}
	return m_accessor_ptrs[name];
}

Valkyrie::glTFBufferView::glTFBufferView(const MemoryChunkPtr& buffer_ptr, const uint32_t length, const uint32_t offset) :
	m_buffer_ptr(buffer_ptr),
	m_length(length),
	m_offset(offset) {

}

Valkyrie::glTFBufferView::~glTFBufferView() {

}

Valkyrie::glTFAccessor::glTFAccessor(const GAPIAttributeSupportPtr& gaas_ptr, GLTF_TYPE type, GLTF_COMPONENT_TYPE component_type) :
	m_type(type),
	m_component_type(component_type),
	m_attribute_ptr(gaas_ptr) {
	
}

Valkyrie::glTFAccessor::~glTFAccessor() {
}
