#ifndef _VALKYRIE_TEXTURE_H 
#define _VALKYRIE_TEXTURE_H  
#include <png.h>
#include "utility.h"

class ValkyriePNG {
public:
	ValkyriePNG();
	~ValkyriePNG();

	bool load(const std::string file_path);
	inline int getWidth() const { return m_width; }
	inline int getHeight() const { return m_height; }
	inline png_bytep getDataPointer() { return mp_data; }
	inline bool available() const { return mp_data != nullptr; }
	inline size_t getSize() const { return m_size; }

private:
	png_bytep mp_data;
	int m_width;
	int m_height;
	png_byte m_color;
	png_byte m_depth;
	size_t m_size;
};

typedef std::shared_ptr<ValkyriePNG> ValkyriePNGPointer;

#endif