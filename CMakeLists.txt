cmake_minimum_required(VERSION 3.16)
if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()

project(Text2Mdx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TEXT2MDX_IMAGE_BACKEND "Auto" CACHE STRING "Image backend: Auto, ResIL, FreeImage, or Internal")
set_property(CACHE TEXT2MDX_IMAGE_BACKEND PROPERTY STRINGS Auto ResIL FreeImage Internal)

option(TEXT2MDX_ENABLE_FREEIMAGE_FALLBACK "Allow Auto backend to use FreeImage when ResIL is not found" OFF)
set(TEXT2MDX_FREEIMAGE_ROOT "E:/SynologyDrive/Project/库/freeimage" CACHE PATH "Optional local FreeImage root")
set(TEXT2MDX_RESIL_ROOT "" CACHE PATH "Optional ResIL root")
set(TEXT2MDX_RESIL_SDK_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/resil-sdk/ResIL-SDK-win32-2026-04-23" CACHE PATH "Bundled ResIL SDK root")

set(TEXT2MDX_SOURCES
    src/blp_writer.cpp
    src/cli.cpp
    src/image.cpp
    src/json_value.cpp
    src/main.cpp
    src/mdx_writer.cpp
    src/style.cpp
    src/text_renderer.cpp
    src/tga_writer.cpp
    src/texture_writer.cpp
    src/utf.cpp
)

add_executable(Text2Mdx ${TEXT2MDX_SOURCES})
target_include_directories(Text2Mdx PRIVATE src)
target_compile_features(Text2Mdx PRIVATE cxx_std_17)
target_compile_definitions(Text2Mdx PRIVATE UNICODE _UNICODE NOMINMAX)

if(WIN32)
    target_link_libraries(Text2Mdx PRIVATE d2d1 dwrite windowscodecs ole32)
else()
    message(FATAL_ERROR "Text2Mdx currently uses DirectWrite/Direct2D and is Windows-only.")
endif()

set(TEXT2MDX_RESIL_LIBRARY_DIRS)
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND TEXT2MDX_RESIL_LIBRARY_DIRS
            "${TEXT2MDX_RESIL_ROOT}/win32_x64"
            "${TEXT2MDX_RESIL_SDK_ROOT}/win32_x64"
            "$ENV{RESIL_ROOT}/win32_x64"
            "$ENV{DEVIL_ROOT}/win32_x64"
        )
    else()
        list(APPEND TEXT2MDX_RESIL_LIBRARY_DIRS
            "${TEXT2MDX_RESIL_ROOT}/win32_x86"
            "${TEXT2MDX_RESIL_SDK_ROOT}/win32_x86"
            "$ENV{RESIL_ROOT}/win32_x86"
            "$ENV{DEVIL_ROOT}/win32_x86"
        )
    endif()
endif()
list(APPEND TEXT2MDX_RESIL_LIBRARY_DIRS
    "${TEXT2MDX_RESIL_ROOT}/lib"
    "${TEXT2MDX_RESIL_SDK_ROOT}/lib"
    "$ENV{RESIL_ROOT}/lib"
    "$ENV{DEVIL_ROOT}/lib"
)

find_path(TEXT2MDX_RESIL_INCLUDE_DIR
    NAMES IL/il2.h IL/il.h
    PATHS
        "${TEXT2MDX_RESIL_ROOT}/include"
        "${TEXT2MDX_RESIL_SDK_ROOT}/include"
        "$ENV{RESIL_ROOT}/include"
        "$ENV{DEVIL_ROOT}/include"
)

find_library(TEXT2MDX_RESIL_LIBRARY
    NAMES ResIL IL DevIL devil
    PATHS ${TEXT2MDX_RESIL_LIBRARY_DIRS}
)

set(TEXT2MDX_RESIL_RUNTIME_DLLS)
if(WIN32 AND TEXT2MDX_RESIL_LIBRARY)
    get_filename_component(TEXT2MDX_RESIL_LIBRARY_DIR "${TEXT2MDX_RESIL_LIBRARY}" DIRECTORY)
    find_file(TEXT2MDX_RESIL_DLL NAMES ResIL.dll PATHS "${TEXT2MDX_RESIL_LIBRARY_DIR}" NO_DEFAULT_PATH)
    find_file(TEXT2MDX_RESIL_INS_DLL NAMES ins.dll PATHS "${TEXT2MDX_RESIL_LIBRARY_DIR}" NO_DEFAULT_PATH)
    if(TEXT2MDX_RESIL_DLL)
        list(APPEND TEXT2MDX_RESIL_RUNTIME_DLLS "${TEXT2MDX_RESIL_DLL}")
    endif()
    if(TEXT2MDX_RESIL_INS_DLL)
        list(APPEND TEXT2MDX_RESIL_RUNTIME_DLLS "${TEXT2MDX_RESIL_INS_DLL}")
    endif()
endif()

find_path(TEXT2MDX_FREEIMAGE_INCLUDE_DIR
    NAMES FreeImage.h
    PATHS
        "${TEXT2MDX_FREEIMAGE_ROOT}/include"
        "$ENV{FREEIMAGE_ROOT}/include"
)

find_library(TEXT2MDX_FREEIMAGE_LIBRARY
    NAMES FreeImage freeimage
    PATHS
        "${TEXT2MDX_FREEIMAGE_ROOT}"
        "${TEXT2MDX_FREEIMAGE_ROOT}/lib"
        "$ENV{FREEIMAGE_ROOT}"
        "$ENV{FREEIMAGE_ROOT}/lib"
)

string(TOLOWER "${TEXT2MDX_IMAGE_BACKEND}" TEXT2MDX_BACKEND_LOWER)

function(text2mdx_use_resil_backend)
    target_sources(Text2Mdx PRIVATE src/texture_writer_resil.cpp)
    target_include_directories(Text2Mdx PRIVATE "${TEXT2MDX_RESIL_INCLUDE_DIR}")
    target_link_libraries(Text2Mdx PRIVATE "${TEXT2MDX_RESIL_LIBRARY}")
    target_compile_definitions(Text2Mdx PRIVATE TEXT2MDX_HAS_RESIL=1)
    if(MSVC)
        target_compile_options(Text2Mdx PRIVATE /wd4828)
    endif()
    foreach(TEXT2MDX_RESIL_RUNTIME_DLL IN LISTS TEXT2MDX_RESIL_RUNTIME_DLLS)
        add_custom_command(TARGET Text2Mdx POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${TEXT2MDX_RESIL_RUNTIME_DLL}"
                "$<TARGET_FILE_DIR:Text2Mdx>"
        )
    endforeach()
endfunction()

if(TEXT2MDX_BACKEND_LOWER STREQUAL "resil")
    if(NOT TEXT2MDX_RESIL_INCLUDE_DIR OR NOT TEXT2MDX_RESIL_LIBRARY)
        message(FATAL_ERROR "TEXT2MDX_IMAGE_BACKEND=ResIL but ResIL headers or library were not found. Set TEXT2MDX_RESIL_ROOT or TEXT2MDX_RESIL_SDK_ROOT.")
    endif()
    text2mdx_use_resil_backend()
    message(STATUS "Text2Mdx image backend: ResIL")
elseif(TEXT2MDX_BACKEND_LOWER STREQUAL "freeimage")
    if(NOT TEXT2MDX_FREEIMAGE_INCLUDE_DIR OR NOT TEXT2MDX_FREEIMAGE_LIBRARY)
        message(FATAL_ERROR "TEXT2MDX_IMAGE_BACKEND=FreeImage but FreeImage was not found. Set TEXT2MDX_FREEIMAGE_ROOT.")
    endif()
    target_sources(Text2Mdx PRIVATE src/texture_writer_freeimage.cpp)
    target_include_directories(Text2Mdx PRIVATE "${TEXT2MDX_FREEIMAGE_INCLUDE_DIR}")
    target_link_libraries(Text2Mdx PRIVATE "${TEXT2MDX_FREEIMAGE_LIBRARY}")
    target_compile_definitions(Text2Mdx PRIVATE TEXT2MDX_HAS_FREEIMAGE=1 FREEIMAGE_LIB)
    if(MSVC)
        set_property(TARGET Text2Mdx PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
elseif(TEXT2MDX_BACKEND_LOWER STREQUAL "auto")
    if(TEXT2MDX_RESIL_INCLUDE_DIR AND TEXT2MDX_RESIL_LIBRARY)
        text2mdx_use_resil_backend()
        message(STATUS "Text2Mdx image backend: ResIL")
    elseif(TEXT2MDX_ENABLE_FREEIMAGE_FALLBACK AND TEXT2MDX_FREEIMAGE_INCLUDE_DIR AND TEXT2MDX_FREEIMAGE_LIBRARY)
        target_sources(Text2Mdx PRIVATE src/texture_writer_freeimage.cpp)
        target_include_directories(Text2Mdx PRIVATE "${TEXT2MDX_FREEIMAGE_INCLUDE_DIR}")
        target_link_libraries(Text2Mdx PRIVATE "${TEXT2MDX_FREEIMAGE_LIBRARY}")
        target_compile_definitions(Text2Mdx PRIVATE TEXT2MDX_HAS_FREEIMAGE=1 FREEIMAGE_LIB)
        if(MSVC)
            set_property(TARGET Text2Mdx PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        endif()
        message(STATUS "Text2Mdx image backend: FreeImage plus internal BLP")
    else()
        message(STATUS "Text2Mdx image backend: internal BLP/TGA only")
    endif()
elseif(NOT TEXT2MDX_BACKEND_LOWER STREQUAL "internal")
    message(FATAL_ERROR "Unknown TEXT2MDX_IMAGE_BACKEND value: ${TEXT2MDX_IMAGE_BACKEND}")
endif()

if(MSVC)
    target_compile_options(Text2Mdx PRIVATE /W4 /permissive- /utf-8)
endif()

if(MINGW)
    target_link_options(Text2Mdx PRIVATE -municode)
endif()

install(TARGETS Text2Mdx RUNTIME DESTINATION bin)
install(DIRECTORY presets DESTINATION .)
