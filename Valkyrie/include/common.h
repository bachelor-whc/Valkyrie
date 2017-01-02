#ifndef _COMMON_H 
#define _COMMON_H 
#ifdef _WIN32
#include <Windows.h>
#define SDL_MAIN_HANDLED
#define VK_USE_PLATFORM_WIN32_KHR
#define SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif __ANDROID__
#define VK_USE_PLATFORM_ANDROID_KHR
#define SURFACE_EXTENSION_NAME VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#elif __linux__
#define VK_USE_PLATFORM_XCB_KHR
#define SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
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
#include <SDL2/SDL.h>
#include "valkyrie/default_create_info.h"
#define NEW_NT new(std::nothrow)
#define MAKE_SHARED(t) std::make_shared<t>

const int VALKYRIE_FRAME_BUFFER_COUNT = 2;
extern VkDevice g_device_handle;
extern VkPhysicalDevice g_physical_device_handle;
using JSON = nlohmann::json;

#endif