cmake_minimum_required(VERSION 3.0)
project(Valkyrie)
if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes")
endif()
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/executable/dbg/)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/executable/rel/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/lib/dbg/)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/lib/rel/)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/third_party/tbb)
find_package(TBB REQUIRED)
set(tbb_root ${TBB_DIR}/../ )

execute_process(
  COMMAND git submodule init
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
execute_process(
  COMMAND git submodule update --recursive
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set(SDL_SHARED_ENABLED_BY_DEFAULT OFF)
set(OPT_DEF_LIBC ON)
add_subdirectory(third_party/SDL2)
add_subdirectory(third_party/zlib)
add_subdirectory(Valkyrie)
add_subdirectory(ValkyriePlayground)