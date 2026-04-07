cmake_minimum_required(VERSION 4.2)
project(BorionLauncher)

set(CMAKE_CXX_STANDARD 23)

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

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)

file(GLOB_RECURSE VK_GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/vulkan/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/vulkan/*.vert"
        "${PROJECT_SOURCE_DIR}/shaders/vulkan/*.comp"
)

foreach(GLSL ${VK_GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/vulkan/${FILE_NAME}.spv")
    message(STATUS ${GLSL})
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL})
    b_embed(BorionLauncher "shaders/vulkan/${FILE_NAME}.spv")
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
        VK_Shaders
        DEPENDS ${SPIRV_BINARY_FILES}
)

file(GLOB_RECURSE OPENGL_GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/opengl/*.fsh"
        "${PROJECT_SOURCE_DIR}/shaders/opengl/*.vsh"
)

foreach(GLSL ${OPENGL_GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    b_embed(BorionLauncher "shaders/opengl/${FILE_NAME}")
endforeach(GLSL)

file(GLOB_RECURSE ASSET_FILES
        "${PROJECT_SOURCE_DIR}/assets/*.png"
        "${PROJECT_SOURCE_DIR}/assets/*.gif"
)

foreach(ASSET ${ASSET_FILES})
    get_filename_component(FILE_NAME ${ASSET} NAME)
    b_embed(BorionLauncher "assets/${FILE_NAME}")
endforeach(ASSET)
