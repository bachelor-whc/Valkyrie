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

void Valkyrie::MemoryChunk::allocate(uint32_t size) throw(...) {
	uint32_t allocating_size = getAlignedSize(size);
	if (allocated() && allocating_size <= getSize()) {
		m_size = size;
		return;
	}
	if (allocated()) {
		delete[] mp_data;
		mp_data = nullptr;
	}
	m_size = size;
	mp_data = NEW_NT unsigned char[allocating_size]();
	if(mp_data == nullptr) {
		std::string ex_message = std::to_string(allocating_size) + " bytes cannot be allocated.";
		throw std::exception(ex_message.c_str());
	}
	setFlags(ALLOCATED);
}
