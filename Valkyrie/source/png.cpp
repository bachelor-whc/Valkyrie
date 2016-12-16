#include <cstdlib>
#include <cstdio>
#include "valkyrie/image.h"

ValkyriePNG::ValkyriePNG() :
	mp_data(nullptr),
	m_width(0),
	m_height(0),
	m_color(NULL),
	m_depth(NULL),
	m_size(0) {

}

ValkyriePNG::~ValkyriePNG() {
	if (mp_data != nullptr)
		delete[] mp_data;
}

bool ValkyriePNG::load(const std::string file_path) {
	FILE* file_ptr = fopen(file_path.c_str(), "rb");
	if (file_ptr == nullptr) {
		fclose(file_ptr);
		return false;
	}
	png_structp struct_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop information_ptr = png_create_info_struct(struct_ptr);
	if (struct_ptr == nullptr || information_ptr == nullptr) {
		fclose(file_ptr);
		return false;
	}
	if (setjmp(png_jmpbuf(struct_ptr))) {
		png_destroy_read_struct(&struct_ptr, &information_ptr, nullptr);
		fclose(file_ptr);
		return false;
	}
	png_init_io(struct_ptr, file_ptr);
	png_set_sig_bytes(struct_ptr, 0);
	png_set_add_alpha(struct_ptr, 0xff, PNG_FILLER_AFTER);
	png_read_png(struct_ptr, information_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, nullptr);

	m_width = png_get_image_width(struct_ptr, information_ptr);
	m_height = png_get_image_width(struct_ptr, information_ptr);
	m_color = png_get_color_type(struct_ptr, information_ptr);
	m_depth = png_get_bit_depth(struct_ptr, information_ptr);

	auto row_bytes = png_get_rowbytes(struct_ptr, information_ptr);
	m_size = m_height * row_bytes;
	mp_data = NEW_NT png_byte[m_size];

	assert(mp_data != nullptr);

	png_bytepp row_ptrs = png_get_rows(struct_ptr, information_ptr);
	if (mp_data == nullptr)
		return false;
	for (int i = 0; i < m_height; ++i)
		memcpy(mp_data + row_bytes * (m_height - i - 1), row_ptrs[i], row_bytes);

	png_destroy_read_struct(&struct_ptr, &information_ptr, nullptr);
	fclose(file_ptr);
	return true;
}