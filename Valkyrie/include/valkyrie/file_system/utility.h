#ifndef _VALKYRIE_FILE_UTILITY_H
#define _VALKYRIE_FILE_UTILITY_H
#include "common.h"

class FileSystem {
public:
	FileSystem() {};
	virtual ~FileSystem() {};
	virtual bool load(const std::string file_path) = 0;
};

uint32_t GetFileSize(FILE* p_file) throw(...);
void FillMemoryFromFile(MemoryAccessPtr ptr, const std::string& file) throw(...);

#endif