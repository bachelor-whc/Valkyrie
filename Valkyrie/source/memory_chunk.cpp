#include "valkyrie/memory_chunk.h"

Valkyrie::MemoryChunk::MemoryChunk() :
	m_size(0),
	m_max_size(0) {

}

Valkyrie::MemoryChunk::~MemoryChunk() {
	if (mp_data != nullptr) {
		delete[] mp_data;
	}
}

void Valkyrie::MemoryChunk::allocate(uint32_t size) {
	if (avaliable() && size <= getSize()) {
		m_size = size;
		return;
	}
	if (avaliable()) {
		delete[] mp_data;
		mp_data = nullptr;
	}
	m_size = size;
	mp_data = NEW_NT unsigned char[m_size]();
}
