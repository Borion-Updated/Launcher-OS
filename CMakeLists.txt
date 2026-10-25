cmake_minimum_required(VERSION 4.2)
project(BorionLauncher)

set(CMAKE_CXX_STANDARD 23)

option(IGNORE_DXBC "Don't compile DirectX 9 shaders" OFF)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE PROJ_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")

set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(SPDLOG_WCHAR_FILENAMES ON)

add_definitions(-DUNICODE -D_UNICODE)

add_compile_options(/GR-)
add_compile_options(/GS-)
add_compile_options(/GL)
add_compile_options(/Os)
add_compile_options(/Oi)
add_compile_options(/nologo)

string(REPLACE "/GR" "/GR-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_compile_definitions(_HAS_STATIC_RTTI=0)

include(include/FetchLibraries.cmake)

add_executable(BorionLauncher WIN32 ${PROJ_SOURCES} resources/resources.rc resources/resources.h)

target_link_libraries(BorionLauncher PRIVATE glm spdlog::spdlog magic_enum giflib Vulkan::Vulkan dwmapi.lib gdiplus.lib shlwapi.lib urlmon.lib wininet.lib)

target_precompile_headers(BorionLauncher PRIVATE PCH.h)

if(MSVC)
    enable_language(RC)
endif()

find_program(GLSL_VALIDATOR glslangValidator HINTS $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE VK_GLSL_SOURCE_FILES
        "resources/shaders/vulkan/*.frag"
        "resources/shaders/vulkan/*.vert"
        "resources/shaders/vulkan/*.comp"
)

foreach(GLSL ${VK_GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "resources/shaders/vulkan/${FILE_NAME}.spv")
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        VK_Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

if(NOT IGNORE_DXBC)
file(GLOB_RECURSE DX9_HLSL_FRAG_FILES "resources/shaders/dx9/*.frag.hlsl")

foreach(HLSL ${DX9_HLSL_FRAG_FILES})
    get_filename_component(FILE_NAME ${HLSL} NAME)
    get_filename_component(RAW_NAME ${HLSL} NAME_WLE)
    set(DXBC "resources/shaders/dx9/${RAW_NAME}.dxbc")
    add_custom_command(
            OUTPUT ${DXBC}
            COMMAND $<SHELL_PATH:${CMAKE_SOURCE_DIR}/DX9BCCreator.exe> ${HLSL} ${DXBC} "ps_2_0"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            DEPENDS ${HLSL})
    list(APPEND DX9_FRAG_BINARY_FILES ${DXBC})
endforeach(HLSL)

add_custom_target(
        DX9_Frag_Shaders
        DEPENDS ${DX9_FRAG_BINARY_FILES}
)

file(GLOB_RECURSE DX9_HLSL_VERT_FILES "resources/shaders/dx9/*.vert.hlsl")

foreach(HLSL ${DX9_HLSL_VERT_FILES})
    get_filename_component(FILE_NAME ${HLSL} NAME)
    get_filename_component(RAW_NAME ${HLSL} NAME_WLE)
    set(DXBC "resources/shaders/dx9/${RAW_NAME}.dxbc")
    add_custom_command(
            OUTPUT ${DXBC}
            COMMAND $<SHELL_PATH:${CMAKE_SOURCE_DIR}/DX9BCCreator.exe> ${HLSL} ${DXBC} "vs_2_0"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            DEPENDS ${HLSL})
    list(APPEND DX9_VERT_BINARY_FILES ${DXBC})
endforeach(HLSL)

add_custom_target(
        DX9_Vert_Shaders
        DEPENDS ${DX9_VERT_BINARY_FILES}
)

add_dependencies(BorionLauncher DX9_Vert_Shaders DX9_Frag_Shaders)
endif()
