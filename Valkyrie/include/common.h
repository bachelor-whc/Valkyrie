#ifndef _COMMON_H 
#define _COMMON_H 
#ifdef _WIN32
#include <Windows.h>
#endif
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
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
#include <exception>
#include <json.hpp>
#include <GLFW/glfw3.h>
#include "valkyrie/default_create_info.h"
#include "valkyrie/utility/memory_access.h"
#define NEW_NT new(std::nothrow)
#define MAKE_SHARED(t) std::make_shared<t>

const int VALKYRIE_FRAME_BUFFER_COUNT = 2;
extern VkDevice g_device_handle;
extern VkPhysicalDevice g_physical_device_handle;
using JSON = nlohmann::json;

#endif