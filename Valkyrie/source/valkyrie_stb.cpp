#include <stb_image.h>
#include <cstdlib>
#include <cstdio>
#include "valkyrie/image.h"

Valkyrie::STB::STB() :
	mp_data(nullptr),
	m_width(0),
	m_height(0),
	m_size(0) {

}

Valkyrie::STB::~STB() {
	if (mp_data != nullptr)
		stbi_image_free(mp_data);
}

bool Valkyrie::STB::load(const std::string file_path) {
	FILE* file_ptr = fopen(file_path.c_str(), "rb");
	if (file_ptr == nullptr) {
		fclose(file_ptr);
		return false;
	}
	mp_data = stbi_load(file_path.c_str(), &m_width, &m_height, &m_channels, STBI_rgb_alpha);
	m_size = m_width * m_height * sizeof(unsigned char) * 4;
	return true;
}