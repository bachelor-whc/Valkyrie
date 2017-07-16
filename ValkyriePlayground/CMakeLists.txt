cmake_minimum_required(VERSION 3.0)
project(ValkyriePlayground)
file(
    GLOB_RECURSE 
    VALKYRIE_PG_SOURCES 
    ${PROJECT_SOURCE_DIR}/*.cpp
)
include_directories(
    $ENV{VULKAN_SDK}/Include/
    ${ValkyrieLib_SOURCE_DIR}/include/
    ${SDL2_SOURCE_DIR}/include/
    ${tbb_root}/include/
    ${zlib_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/json/src
    ${CMAKE_SOURCE_DIR}/third_party/imgui
    ${CMAKE_SOURCE_DIR}/third_party/glm
    ${CMAKE_SOURCE_DIR}/third_party/stb
)
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    link_directories($ENV{VULKAN_SDK}/Lib/)
else ()
    link_directories($ENV{VULKAN_SDK}/Lib32/)
endif ()
add_executable(ValkyriePlayground ${VALKYRIE_PG_SOURCES})
if(MSVC)
    set_target_properties(ValkyriePlayground PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
    set_target_properties(ValkyriePlayground PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
endif()  
target_link_libraries(
    ValkyriePlayground
    ValkyrieLib
    SDL2-static
    SDL2main
    zlibstatic
    vulkan-1.lib
    ${TBB_IMPORTED_TARGETS}
)