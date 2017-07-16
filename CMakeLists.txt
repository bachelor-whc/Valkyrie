cmake_minimum_required(VERSION 3.0)
project(Valkyrie)
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes")
endif()
include(TBBGet.cmake)
tbb_get(
    TBB_ROOT tbb_root
    SAVE_TO ${CMAKE_SOURCE_DIR}/third_party
    CONFIG_DIR TBB_DIR
)
find_package(TBB REQUIRED tbb)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
add_subdirectory(third_party/SDL2)
add_subdirectory(third_party/googletest)
add_subdirectory(third_party/zlib)
add_subdirectory(Valkyrie)
add_subdirectory(ValkyriePlayground)