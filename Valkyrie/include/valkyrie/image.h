#ifndef _VALKYRIE_IMAGE_H 
#define _VALKYRIE_IMAGE_H 
#include "utility.h"

class ValkyrieRGBA32 {
public:
	ValkyrieRGBA32() {};
	virtual ~ValkyrieRGBA32() {};

	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual void* getDataPointer() = 0;
	virtual bool available() const = 0;
	virtual size_t getSize() const = 0;
};

class ValkyrieRGBA32Memory : public ValkyrieRGBA32 {
public:
	ValkyrieRGBA32Memory() = delete;
	ValkyrieRGBA32Memory(int width, int height, void* src_data);
	virtual ~ValkyrieRGBA32Memory();

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

class ValkyrieImageFile : 
	public ValkyrieRGBA32, 
	public FileSystem {
public:
	ValkyrieImageFile() {};
	virtual ~ValkyrieImageFile() {};
};

class ValkyrieSTB : public ValkyrieImageFile {
public:
	ValkyrieSTB();
	virtual ~ValkyrieSTB();

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

typedef std::shared_ptr<ValkyrieImageFile> ValkyrieImageFilePointer;
typedef std::shared_ptr<ValkyrieRGBA32Memory> ValkyrieImageMemoryPointer;

#endif