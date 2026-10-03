
cmake_minimum_required(VERSION 3.21)

project(VanillaEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# DirectXTex is vendored under Core/Vendor and built as a static library.
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BUILD_SAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(Core/Vendor/DirectXTex EXCLUDE_FROM_ALL)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Core/Vendor/glfw)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Core/Vendor/imgui")

# Build tinyobjloader as a submodule dependency for mesh loading.
set(TINYOBJLOADER_BUILD_TEST_LOADER OFF CACHE BOOL "" FORCE)
add_subdirectory(Core/Vendor/tinyobjloader EXCLUDE_FROM_ALL)

# Discover engine shaders automatically. CONFIGURE_DEPENDS makes CMake
# reconfigure when another shader is added under Core/Shaders.
file(GLOB_RECURSE ENGINE_SHADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Shaders/*.hlsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/Shaders/*.hlsli"
)

set(ENGINE_SOURCES
    "Core/Application.cpp"
    "Core/Platform/Windows/vlWindow.cpp"
    "Core/Platform/DirectX/Buffer.h"
     "Core/Platform/DirectX/Buffer.cpp"
    "Core/Platform/Windows/vlRenderer.cpp"
     "Core/Platform/Windows/vlCore.h"
     "Core/Platform/Windows/vlCore.cpp"
     "Core/Platform/DirectX/VertexShader.cpp"
     "Core/Platform/DirectX/VertexShader.h"
     "Core/Platform/Windows/vlUserInterface.cpp"
     "Core/Platform/Windows/vlUserInterface.h"
     "Core/Platform/DirectX/PixelShader.h"
     "Core/Platform/DirectX/PixelShader.cpp"
     "Core/Resources/TextureRes.cpp"
     "Core/Resources/TextureRes.h"
     
     
     "Core/Platform/DirectX/vlDebugLayer.cpp"
     "Core/Platform/DirectX/vlDebugLayer.h" "Core/Platform/Windows/Viewport.cpp" "Core/Platform/Windows/Viewport.h" "Core/Platform/Windows/vlCam.h" "Core/Platform/Windows/vlCam.cpp" "Core/Platform/DirectX/Logger.cpp" "Core/Platform/DirectX/Logger.h")

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp)

add_library(VanillaEngine STATIC ${ENGINE_SOURCES} ${IMGUI_SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Core/Shaders"
    PREFIX "Shaders"
    FILES ${ENGINE_SHADERS}
)

target_include_directories(VanillaEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(VanillaEngine PUBLIC glfw DirectXTex tinyobjloader d3d11 d3dcompiler dxgi gdi32)

add_executable(vanilla
    "Sandbox/Main.cpp"
)

target_link_libraries(vanilla PRIVATE VanillaEngine)

foreach(SHADER_FILE IN LISTS ENGINE_SHADERS)
    file(RELATIVE_PATH SHADER_RELATIVE_PATH
        "${CMAKE_CURRENT_SOURCE_DIR}/Core/Shaders"
        "${SHADER_FILE}"
    )
    get_filename_component(SHADER_RELATIVE_DIRECTORY
        "${SHADER_RELATIVE_PATH}"
        DIRECTORY
    )

    add_custom_command(TARGET vanilla POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:vanilla>/Shaders/${SHADER_RELATIVE_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE}"
            "$<TARGET_FILE_DIR:vanilla>/Shaders/${SHADER_RELATIVE_PATH}"
        VERBATIM
    )
endforeach()
