cmake_minimum_required(VERSION 3.21)

project(PlatformerDemo VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# custom options to toggle
option(ENABLE_PREPROCESS_OUTPUT "Preprocessed files instead of compiling" OFF)
option(ENABLE_ASM_OUTPUT "Assembly files instead of compiling" OFF)
option(ENABLE_SIMD_OPTIMISATIONS "Enable SIMD compiler optimisations (AVX2)" OFF)
#------

set(EXEC_TARGET_NAME "${CMAKE_PROJECT_NAME}Exec")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TARGET_PLATFORM "windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_PLATFORM "linux")
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${TARGET_PLATFORM}/$<CONFIG>")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
file(GLOB_RECURSE CPP_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable("${EXEC_TARGET_NAME}" ${CPP_SOURCES})

target_compile_definitions("${EXEC_TARGET_NAME}" PRIVATE
        "$<$<CONFIG:Debug>:DEBUG>"
        "$<$<CONFIG:Release>:RELEASE>"
)

target_compile_options("${EXEC_TARGET_NAME}" PRIVATE
        "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang>>:-O0;-g;-Wall;-Wextra;-Wpedantic>"
        "$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:GNU,Clang>>:-O2;-Wall;-Wextra>" # O3 risky(try it to check performances) - add flto=thin for Clang(LLVM) and ???? for GCC?
        "$<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od;/W4;/Zi>"
        "$<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2;/W4>"
        "$<$<AND:$<BOOL:${ENABLE_PREPROCESS_OUTPUT}>,$<CXX_COMPILER_ID:GNU,Clang>>:-save-temps=obj>"
        "$<$<AND:$<BOOL:${ENABLE_PREPROCESS_OUTPUT}>,$<CXX_COMPILER_ID:MSVC>>:/P>"
        "$<$<AND:$<BOOL:${ENABLE_ASM_OUTPUT}>,$<CXX_COMPILER_ID:GNU,Clang>>:-save-temps=obj>"
        "$<$<AND:$<BOOL:${ENABLE_ASM_OUTPUT}>,$<CXX_COMPILER_ID:MSVC>>:/FAs;/FS>"
)

#TODO: SIMD optimisation, test them one day, to see if it really helps for my little project. Check for other optimisation flags too.
# btw, later in the real project maybe set a lower level of SIMD kike SSE2 and "set_source_files_properties()" or #pragma some files/functions
# to use AVX2 if we know these would benefit from it a lot, whereas some other parts would perhaps be slowed down by it
if(ENABLE_SIMD_OPTIMISATIONS)
    message(STATUS "SIMP optimisation flags enabled for ${EXEC_TARGET_NAME}")
    if(MSVC)
        target_compile_options("${EXEC_TARGET_NAME}" PRIVATE /arch:AVX2)
    else()
        target_compile_options("${EXEC_TARGET_NAME}" PRIVATE -mavx2 -mfma)
    endif()
endif()

#TODO: for gcc, test it on the real project to see if there are benefits or bugs. May need to be set to MSVC/CLang only (clang for win/linux/mac)
# LTO (link time optimization)
set_property(TARGET "${EXEC_TARGET_NAME}" PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set_property(TARGET "${EXEC_TARGET_NAME}" PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)


# MSVC/Windows/Visual Studio specifics
set_target_properties("${EXEC_TARGET_NAME}" PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Hide console and open as a window app (release only) 
#TODO: enable it back once i've got my custom logger (in-window console and/or writing to a file)
if(MSVC)
# remove deprecation warnings for c-style memory operation like strcpy, memcpy etc, those without the "_s" suffix, don't need it atm i think
    #target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC "$<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS>") 
# Subsystem Windows = same as in the vcxproj, not a console app but a window one
# however if not a console app, msvc linker tells the CRT to expect a "WinMain" function as the entry point 
# but we want it to target our old friend the main() function so we add this second property to override this behavior
    
    #target_link_options("${EXEC_TARGET_NAME}" PRIVATE
    #    "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    #    "$<$<CONFIG:Release>:/ENTRY:mainCRTStartup>"
    #)

# mingw/gcc on windows use the -mwindows linker flag equivalent, but no need to override the entry point its CRT already searches for main()
elseif(MINGW)
    target_link_options("${EXEC_TARGET_NAME}" PRIVATE "$<$<CONFIG:Release>:-mwindows>")
endif()

if(MSVC)
# which project is launched at solution level when you hit F5, but as i need to go into my project view to see all files/folders in the explorer
# the CMake presets launch configs override it so it's kind of useless for me tbf but it's there if you want it lol
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "${EXEC_TARGET_NAME}")
endif()

# Remove UNICODE for compile-time Windows shenanigan sometimes turning functions from narrow-style/ANSI (char etc) to wide-style/unicode (wchar_t etc)
# we can still use the WFunctions if needed but that way it's not random and we can assume the code will run the same on win/linux bc no compile-time guess
if (WIN32)
    target_compile_options("${EXEC_TARGET_NAME}" PRIVATE /UUNICODE /U_UNICODE)
endif()

# trying to make renderdoc work (on linux) (https://github.com/baldurk/renderdoc/issues/853)
#set(DEBUG_RENDERDOC OFF)
#if(DEBUG_RENDERDOC EQUAL ON)
#    set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
#    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
#endif()

# lib options
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # static lib (used by GLFW, Assimp, libcurl)

# GLFW options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_WIN32 OFF CACHE BOOL "" FORCE) # idc about win32 support => but needed bc fucking win64 still relies on win32 api.

# GLM options (don't enable fast math, it makes float math undeterministic, np for singleplayer games overall but better disabled for my goal)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "" FORCE)
if(ENABLE_SIMD_OPTIMISATIONS)
    set(GLM_ENABLE_SIMD_AVX2 ON CACHE BOOL "" FORCE)
endif()

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/stb)
add_subdirectory(vendor/imgui)
target_link_libraries("${EXEC_TARGET_NAME}" PRIVATE glfw glad glm stb imgui)