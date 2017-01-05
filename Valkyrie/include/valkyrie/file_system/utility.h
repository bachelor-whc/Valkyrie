#ifndef _VALKYRIE_FILE_UTILITY_H
#define _VALKYRIE_FILE_UTILITY_H
#include "valkyrie/utility/memory_access.h"
#include "common.h"

class FileSystem {
public:
	FileSystem() {};
	virtual ~FileSystem() {};
	virtual bool load(const std::string file_path) = 0;
};

#endif