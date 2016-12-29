#ifndef _UTILITY_H 
#define _UTILITY_H 
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#include <vulkan/vulkan.h>
#ifdef _WIN32
#define SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif __ANDROID__

#endif
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <fstream>
#include <iostream>
#include <new>
#include <memory>
#include <set>
#include <unordered_set>
#include <GLFW/glfw3.h>
#include "valkyrie/default_create_info.h"
#define NEW_NT new(std::nothrow)

const int VALKYRIE_FRAME_BUFFER_COUNT = 2;
extern VkDevice g_device_handle;
extern VkPhysicalDevice g_physical_device_handle;

class FileSystem {
public:
	FileSystem() {};
	virtual ~FileSystem() {};
	virtual bool load(const std::string file_path) = 0;
};

#endif