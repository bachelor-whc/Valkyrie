cmake_minimum_required(VERSION 3.0)
project(ValkyrieLib)

file(
    GLOB 
    VALKYRIE_BASE_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/common.h
    ${PROJECT_SOURCE_DIR}/include/valkyrie/valkyrie.h
    ${PROJECT_SOURCE_DIR}/source/*.cpp
)

file(
    GLOB
    VALKYRIE_ASSET_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/asset/*.h
    ${PROJECT_SOURCE_DIR}/source/asset/*.cpp
)

file(
    GLOB
    VALKYRIE_COMPONENT_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/component/*.h
    ${PROJECT_SOURCE_DIR}/source/component/*.cpp
)

file(
    GLOB 
    VALKYRIE_FACTORY_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/factory/*.h
    ${PROJECT_SOURCE_DIR}/source/factory/*.cpp
)

file(
    GLOB 
    VALKYRIE_GRAPHICS_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/graphics/*.h
    ${PROJECT_SOURCE_DIR}/source/graphics/*.cpp
)

file(
    GLOB 
    VALKYRIE_GRAPHICS_API_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/graphics_api_support/*.h
    ${PROJECT_SOURCE_DIR}/source/graphics_api_support/*.cpp
)

file(
    GLOB 
    VALKYRIE_SCENE_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/scene/*.h
    ${PROJECT_SOURCE_DIR}/source/scene/*.cpp
)

file(
    GLOB 
    VALKYRIE_UI_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/UI/*.h
    ${PROJECT_SOURCE_DIR}/source/UI/*.cpp
)

file(
    GLOB 
    VALKYRIE_UTILITY_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/utility/*.h
    ${PROJECT_SOURCE_DIR}/source/utility/*.cpp
)

file(
    GLOB 
    VALKYRIE_VULKAN_CODE
    ${PROJECT_SOURCE_DIR}/include/valkyrie/vulkan/*.h
    ${PROJECT_SOURCE_DIR}/source/vulkan/*.cpp
)

file(
    GLOB 
    THIRD_PARTY_IMGUI_CODE
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
)

include_directories(
    $ENV{VULKAN_SDK}/Include/
    ${PROJECT_SOURCE_DIR}/include/
    ${SDL2_SOURCE_DIR}/include/
    ${tbb_root}/include/
    ${zlib_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/json/src
    ${CMAKE_SOURCE_DIR}/third_party/imgui
    ${CMAKE_SOURCE_DIR}/third_party/glm
    ${CMAKE_SOURCE_DIR}/third_party/stb
)

source_group(" "                     FILES ${VALKYRIE_BASE_CODE})
source_group(asset                   FILES ${VALKYRIE_ASSET_CODE})
source_group(component               FILES ${VALKYRIE_COMPONENT_CODE})
source_group(factory                 FILES ${VALKYRIE_FACTORY_CODE})
source_group(graphics                FILES ${VALKYRIE_GRAPHICS_CODE})
source_group(graphics_api_support    FILES ${VALKYRIE_GRAPHICS_API_CODE})
source_group(scene                   FILES ${VALKYRIE_SCENE_CODE})
source_group(UI                      FILES ${VALKYRIE_UI_CODE})
source_group(utility                 FILES ${VALKYRIE_UTILITY_CODE})
source_group(vulkan                  FILES ${VALKYRIE_VULKAN_CODE})
source_group(third_party\\imgui      FILES ${THIRD_PARTY_IMGUI_CODE})

add_library(
    ValkyrieLib 
    STATIC 
    ${VALKYRIE_BASE_CODE}
    ${VALKYRIE_ASSET_CODE}
    ${VALKYRIE_COMPONENT_CODE}
    ${VALKYRIE_FACTORY_CODE}
    ${VALKYRIE_GRAPHICS_CODE}
    ${VALKYRIE_GRAPHICS_API_CODE}
    ${VALKYRIE_SCENE_CODE}
    ${VALKYRIE_UI_CODE}
    ${VALKYRIE_UTILITY_CODE}
    ${VALKYRIE_VULKAN_CODE}
    ${THIRD_PARTY_IMGUI_CODE}
)