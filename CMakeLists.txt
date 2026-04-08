cmake_minimum_required(VERSION 3.16)
project(LancelotEngine LANGUAGES C CXX)

if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,
        $<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,
        $<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

if (EXISTS "${CMAKE_SOURCE_DIR}/external/glfw/CMakeLists.txt")
    add_subdirectory(external/glfw)
endif()

if (EXISTS "${CMAKE_SOURCE_DIR}/external/glad/CMakeLists.txt")
    add_subdirectory(external/glad)
elseif (EXISTS "${CMAKE_SOURCE_DIR}/external/glad/src/glad.c")
    add_library(glad STATIC external/glad/src/glad.c)
    target_include_directories(glad PUBLIC external/glad/include)
endif()

if (TARGET glfw)
    set(LANCELOT_GLFW_TARGET glfw)
elseif (TARGET glfw3::glfw)
    set(LANCELOT_GLFW_TARGET glfw3::glfw)
else()
    find_package(glfw3 REQUIRED)
    if (TARGET glfw)
        set(LANCELOT_GLFW_TARGET glfw)
    elseif (TARGET glfw3::glfw)
        set(LANCELOT_GLFW_TARGET glfw3::glfw)
    else()
        message(FATAL_ERROR "GLFW target was not found. Provide external/glfw or install glfw3.")
    endif()
endif()

if (TARGET glad)
    set(LANCELOT_GLAD_TARGET glad)
elseif (TARGET glad::glad)
    set(LANCELOT_GLAD_TARGET glad::glad)
else()
    find_package(glad CONFIG REQUIRED)
    if (TARGET glad)
        set(LANCELOT_GLAD_TARGET glad)
    elseif (TARGET glad::glad)
        set(LANCELOT_GLAD_TARGET glad::glad)
    else()
        message(FATAL_ERROR "GLAD target was not found. Provide external/glad or install glad.")
    endif()
endif()

add_executable(LancelotEngine
    main.cpp
    backend/core/Application.cpp
    backend/window/WindowManager.cpp
    backend/render/GraphicsContext.cpp
    backend/render/RendererAPI.cpp
    backend/render/RenderCommand.cpp
    backend/render/Renderer.cpp
    backend/render/Buffer.cpp
    backend/render/VertexArray.cpp
    backend/render/Shader.cpp
    backend/render/OrthographicCamera.cpp
    backend/platform/opengl/OpenGLContext.cpp
    backend/platform/opengl/OpenGLRendererAPI.cpp
    backend/platform/opengl/OpenGLBuffer.cpp
    backend/platform/opengl/OpenGLVertexArray.cpp
    backend/platform/opengl/OpenGLShader.cpp
)

target_include_directories(LancelotEngine PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/backend
    ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(LancelotEngine PRIVATE
    ${LANCELOT_GLFW_TARGET}
    ${LANCELOT_GLAD_TARGET}
    OpenGL::GL
)

if (MSVC)
    target_compile_definitions(LancelotEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
