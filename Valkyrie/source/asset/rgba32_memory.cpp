#include "common.h"
#include "valkyrie/asset/image.h"

Valkyrie::RGBA32Memory::RGBA32Memory(int width, int height) : MemoryChunk() {
	if (width <= 0 || height <= 0)
		throw std::exception("not avaliable size.");
	allocate(width * height * 4);
	m_width = width;
	m_height = height;
}

Valkyrie::RGBA32Memory::~RGBA32Memory() {
	
}