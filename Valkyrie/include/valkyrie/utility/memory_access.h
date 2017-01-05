#ifndef _UTILITY_MEMORY_ACCESS_H
#define _UTILITY_MEMORY_ACCESS_H
#include <memory>
#include <cstdint>

class MemoryAccess {
public:
	static const int NONE = 0;
	static const int ALLOCATED = 1;
	static const int READY = ALLOCATED << 1;

	MemoryAccess() {}
	virtual ~MemoryAccess() {};
	virtual void* getData() = 0;
	virtual uint32_t getSize() const = 0;
	void setFlags(int flag) { m_status |= flag; }
	void unsetFlags(int flag) { m_status = m_status & (~flag); }
	bool allocated() const { return (m_status & ALLOCATED) > 0; }
	bool ready() const { return (m_status & READY) > 0; }
private:
	int m_status = NONE;
};

typedef std::shared_ptr<MemoryAccess> MemoryAccessPtr;

#endif