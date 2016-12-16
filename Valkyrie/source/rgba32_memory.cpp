#include "utility.h"
#include "valkyrie/image.h"

ValkyrieRGBA32Memory::ValkyrieRGBA32Memory(int width, int height, void* src_data) :
	m_width(width),
	m_height(height),
	m_size(0)
{
	assert(width > 0);
	assert(height > 0);
	assert(src_data != nullptr);

	m_size = width * height * 4;
	mp_data = NEW_NT unsigned char[m_size];
	assert(mp_data != nullptr);

	memcpy(mp_data, src_data, m_size);
}

ValkyrieRGBA32Memory::~ValkyrieRGBA32Memory() {
	if (mp_data != nullptr)
		delete[] mp_data;
}