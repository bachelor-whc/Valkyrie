#include <glm/glm.hpp>
#include "utility.h"
#include "valkyrie/graphics_api_support/attribute.h"
using namespace Valkyrie;

template class GrpahicsAPIAttribute<uint16_t>;
template class GrpahicsAPIAttribute<uint32_t>;
template class GrpahicsAPIAttribute<glm::float32>;
template class GrpahicsAPIAttribute<glm::float64>;
template class GrpahicsAPIAttribute<glm::vec2>;
template class GrpahicsAPIAttribute<glm::vec3>;
template class GrpahicsAPIAttribute<glm::vec4>;
template class GrpahicsAPIAttribute<glm::mat2>;
template class GrpahicsAPIAttribute<glm::mat3>;
template class GrpahicsAPIAttribute<glm::mat4>;

template<typename T>
Valkyrie::GrpahicsAPIAttribute<T>::GrpahicsAPIAttribute(const MemoryAccessPtr& ptr, const uint32_t offset = 0, const uint32_t stride = 0, const uint32_t count = 1) :
	m_type_size(sizeof(T)),
	m_offset(offset),
	m_stride(stride),
	m_count(count) {
	unsigned char* base_ptr = (unsigned char*)ptr->getData();
	mp_implement = (T*)(base_ptr + offset);
}

template<typename T>
T& Valkyrie::GrpahicsAPIAttribute<T>::getInstance(uint32_t index) {
	assert(index < m_count);
	uint32_t stride = m_stride == 0 ? m_type_size : m_stride;
	return *(mp_implement + m_offset + index * stride);
}

void Valkyrie::GrpahicsAPIAttribute<uint16_t>::initializeFormats() {
	m_index_format = VK_INDEX_TYPE_UINT16;
}

void Valkyrie::GrpahicsAPIAttribute<uint32_t>::initializeFormats() {
	m_index_format = VK_INDEX_TYPE_UINT32;
}

void Valkyrie::GrpahicsAPIAttribute<glm::vec2>::initializeFormats() {
	m_format = VK_FORMAT_R32G32_SFLOAT;
}

void Valkyrie::GrpahicsAPIAttribute<glm::vec3>::initializeFormats() {
	m_format = VK_FORMAT_R32G32B32_SFLOAT;
}

void Valkyrie::GrpahicsAPIAttribute<glm::vec4>::initializeFormats() {
	m_format = VK_FORMAT_R32G32B32A32_SFLOAT;
}

template<typename T>
void Valkyrie::GrpahicsAPIAttribute<T>::initializeFormats() {
	
}