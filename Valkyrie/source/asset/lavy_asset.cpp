#include "valkyrie/asset/lavy_asset.h"
#include "valkyrie/memory_chunk.h"

Valkyrie::LavyAsset::LavyAsset(const JSON& src) : 
	m_json(src),
	m_buffer_ptr(MAKE_SHARED(MemoryChunk)()){

}