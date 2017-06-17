#ifndef _VALKYRIE_MEMORY_CHUNK_H
#define _VALKYRIE_MEMORY_CHUNK_H
#include "valkyrie/asset/asset.h"
#include "valkyrie/utility/memory_access.h"
#include "common.h"

namespace Valkyrie {
	class MemoryChunk : 
		public MemoryAccess,
		public Asset {
	public:
		MemoryChunk();
		MemoryChunk(const MemoryChunk& src) = delete;
		virtual ~MemoryChunk();

		virtual void* getData() { return mp_data; }
		virtual uint32_t getSize() const { return m_size; }

		MemoryChunk& operator=(const MemoryChunk& src) = delete;
		virtual void allocate(uint32_t size) throw(...);
		inline uint32_t getMaxSize() { return m_max_size; }
		
	private:
		inline uint32_t getAlignedSize(uint32_t size) { return (size / 4 + (size % 4 == 0 ? 0 : 1)) * 4; }

		uint32_t m_size;
		uint32_t m_max_size;
		unsigned char* mp_data = nullptr;
	};
	
	using MemoryChunkPtr = std::shared_ptr<MemoryChunk>;
}

#endif