#ifndef _VALKYRIE_IMAGE_H 
#define _VALKYRIE_IMAGE_H 
#include "valkyrie/memory_chunk.h"
#include "common.h"

namespace Valkyrie {

	class RGBA32 {
	public:
		RGBA32() {};
		virtual ~RGBA32() {};

		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;
	};

	class RGBA32Memory : public RGBA32, public MemoryChunk {
	public:
		RGBA32Memory() = delete;
		RGBA32Memory(int width, int height);
		virtual ~RGBA32Memory();

		virtual int getWidth() const { return m_width; }
		virtual int getHeight() const { return m_height; }

	private:
		int m_width;
		int m_height;
	};

	typedef std::shared_ptr<RGBA32Memory> ImageMemoryPointer;
}

#endif