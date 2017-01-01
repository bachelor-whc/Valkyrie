#ifndef _UTILITY_MEMORY_ACCESS_H
#define _UTILITY_MEMORY_ACCESS_H
#include <memory>
#include <cstdint>

class MemoryAccess {
public:
	MemoryAccess() {}
	virtual ~MemoryAccess() {};
	virtual void* getData() = 0;
	virtual uint32_t getSize() const = 0;
};

typedef std::shared_ptr<MemoryAccess> MemoryAccessPtr;

#endif