#ifndef _VALKYRIE_IMAGE_H 
#define _VALKYRIE_IMAGE_H 
#include "common.h"
#include "file_system/utility.h"

namespace Valkyrie {

	class RGBA32 {
	public:
		RGBA32() {};
		virtual ~RGBA32() {};

		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
		virtual void* getDataPointer() = 0;
		virtual bool available() const = 0;
		virtual size_t getSize() const = 0;
	};

	class RGBA32Memory : public RGBA32 {
	public:
		RGBA32Memory() = delete;
		RGBA32Memory(int width, int height, void* src_data);
		virtual ~RGBA32Memory();

		virtual int getWidth() const { return m_width; }
		virtual int getHeight() const { return m_height; }
		virtual void* getDataPointer() { return mp_data; }
		virtual bool available() const { return mp_data != nullptr; }
		virtual size_t getSize() const { return m_size; }

	private:
		unsigned char* mp_data = nullptr;
		int m_width;
		int m_height;
		size_t m_size;
	};

	class ImageFile :
		public RGBA32,
		public FileSystem {
	public:
		ImageFile() {};
		virtual ~ImageFile() {};
	};

	class STB : public ImageFile {
	public:
		STB();
		virtual ~STB();

		virtual bool load(const std::string file_path);
		virtual int getWidth() const { return m_width; }
		virtual int getHeight() const { return m_height; }
		virtual void* getDataPointer() { return mp_data; }
		virtual bool available() const { return mp_data != nullptr; }
		virtual size_t getSize() const { return m_size; }

	private:
		unsigned char* mp_data;
		int m_width;
		int m_height;
		int m_channels;
		size_t m_size;
	};

	typedef std::shared_ptr<ImageFile> ImageFilePointer;
	typedef std::shared_ptr<RGBA32Memory> ImageMemoryPointer;
}

#endif