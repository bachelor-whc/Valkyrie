#include <cstdio>
#include "valkyrie/utility/memory_access.h"
#include "common.h"

long GetFileSize(FILE* p_file) throw(...) {
	if (p_file == nullptr) {
		std::string ex_message = "File is not opened.";
		throw std::exception(ex_message.c_str());
	}
	fseek(p_file, 0L, SEEK_END);
	long ret = ftell(p_file);
	rewind(p_file);
	return ret;
}

void FillMemoryFromFile(MemoryAccessPtr ptr, const std::string& filename) throw(...) {
	FILE* p_file = fopen(filename.c_str(), "rb");
	uint32_t file_size = GetFileSize(p_file);
	if (p_file == nullptr) {
		std::string ex_message = "File is not opened.";
		throw std::exception(ex_message.c_str());
	}
	unsigned char* base_ptr = (unsigned char*)ptr->getData();
	uint32_t memory_size = ptr->getSize();
	uint32_t read_size = file_size < memory_size ? file_size : memory_size;
	fread(ptr->getData(), read_size, 1, p_file);
	fclose(p_file);
}

void FillMemoryFromPartialFile(MemoryAccessPtr ptr, const std::string& filename) throw(...) {
	FILE* p_file = fopen(filename.c_str(), "rb");
	uint32_t file_size = GetFileSize(p_file);
	if (p_file == nullptr) {
		std::string ex_message = "File is not opened.";
		throw std::exception(ex_message.c_str());
	}
	unsigned char* base_ptr = (unsigned char*)ptr->getData();
	uint32_t memory_size = ptr->getSize();
	uint32_t read_size = file_size < memory_size ? file_size : memory_size;
	fread(ptr->getData(), read_size, 1, p_file);
	fclose(p_file);
}