cmake_minimum_required(VERSION 3.0.0)
project(WarMorale)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(LibraryPaths)

# Declare main executable
file(GLOB WARMORALE_SOURCES src/*.cpp)
add_executable(WarMorale ${WARMORALE_SOURCES})

# Compiler specific
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Hide Console
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

  find_library(GLEW glew32 HINTS "${GLEW_DIR}\\lib\\Release\\Win32")
  find_library(GLFW glfw3 HINTS "${GLFW_DIR}\\lib-vc2015")
  find_library(OPENGL opengl32)
  set(GLEW_INCLUDE "${GLEW_DIR}\\include")
  set(GLFW_INCLUDE "${GLFW_DIR}\\include")
  
  add_custom_command(TARGET WarMorale POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${GLEW_DIR}/bin/Release/Win32/glew32.dll $<TARGET_FILE_DIR:WarMorale>/glew32.dll)
  add_custom_command(TARGET WarMorale POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:WarMorale>/assets)
endif()

# Header only libraries
add_library(STB INTERFACE)
target_include_directories(STB INTERFACE ${STB_DIR})

# Interface libraries with main executable
target_include_directories(WarMorale PUBLIC src ${GLEW_INCLUDE} ${GLFW_INCLUDE})
target_link_libraries(WarMorale ${GLEW} ${GLFW} ${OPENGL} STB)

# Testing header only libraries
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_DIR})

# Unit tests
add_executable(WarMoraleTests src/tests/tests.cpp)
target_link_libraries(WarMoraleTests Catch)
target_include_directories(WarMoraleTests PUBLIC src)
enable_testing()
add_test(NAME Tests COMMAND WarMoraleTests)