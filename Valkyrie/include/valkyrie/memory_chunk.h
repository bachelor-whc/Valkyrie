#ifndef _VALKYRIE_MEMORY_CHUNK_H
#define _VALKYRIE_MEMORY_CHUNK_H
#include "valkyrie/utility/memory_access.h"
#include "common.h"

namespace Valkyrie {
	class MemoryChunk : public MemoryAccess {
	public:
		MemoryChunk();
		MemoryChunk(const MemoryChunk& src) = delete;
		virtual ~MemoryChunk();

		virtual void* getData() { return mp_data; }
		virtual uint32_t getSize() const { return m_size; }

		MemoryChunk& operator=(const MemoryChunk& src) = delete;
		virtual void allocate(uint32_t size);
		inline uint32_t getMaxSize() { return m_max_size; }
		inline bool avaliable() { return mp_data != nullptr; }
		
	private:
		uint32_t m_size;
		uint32_t m_max_size;
		unsigned char* mp_data = nullptr;
	};

	typedef std::shared_ptr<MemoryChunk> MemoryChunkPtr;
}

#endif