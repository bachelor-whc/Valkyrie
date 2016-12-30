#ifndef _VALKYRIE_MEMORY_CHUNK_H
#define _VALKYRIE_MEMORY_CHUNK_H
#include "utility.h"

namespace Valkyrie {
	class MemoryChunk {
	public:
		MemoryChunk();
		MemoryChunk(const MemoryChunk& src) = delete;
		virtual ~MemoryChunk();

		MemoryChunk& operator=(const MemoryChunk& src) = delete;
		void allocate(uint32_t size);
		inline uint32_t getSize() { return m_size; }
		inline uint32_t getMaxSize() { return m_max_size; }
		inline void* getData() { return mp_data; }
		inline bool avaliable() { return mp_data != nullptr; }
		
	private:
		uint32_t m_size;
		uint32_t m_max_size;
		unsigned char* mp_data = nullptr;
	};
}

#endif