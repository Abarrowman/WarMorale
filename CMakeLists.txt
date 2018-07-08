cmake_minimum_required(VERSION 3.0.0)
project(WarMorale)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)

include_directories(src)

# Declare main executable
file(GLOB WARMORALE_SOURCES src/*.cpp)
add_executable(WarMorale ${WARMORALE_SOURCES})


# Compiler specific
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Hide Console
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  
  # TODO make this work on other computers
  set(GLEW_DIR "C:\\A\\Git\\ThirdParty\\C\\glew-2.1.0")
  set(GLFW_DIR "C:\\A\\Git\\ThirdParty\\C\\glfw-3.2.1")
  set(CATCH_DIR "C:\\A\\Git\\ThirdParty\\C\\catch2-2.0.1")
  set(STB_DIR "C:\\A\\Git\\ThirdParty\\C\\stb")


  find_library(GLEW glew32 HINTS "${GLEW_DIR}\\lib\\Release\\Win32")
  find_library(GLFW glfw3 HINTS "${GLFW_DIR}\\lib-vc2015")
  find_library(OPENGL opengl32)
  set(GLEW_INCLUDE "${GLEW_DIR}\\include")
  set(GLFW_INCLUDE "${GLFW_DIR}\\include")
  
  add_custom_command(TARGET WarMorale POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${GLEW_DIR}/bin/Release/Win32/glew32.dll $<TARGET_FILE_DIR:WarMorale>/glew32.dll)
  add_custom_command(TARGET WarMorale POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:WarMorale>/assets)
endif()

# Testing libraries
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_DIR})

add_library(STB INTERFACE)
target_include_directories(STB INTERFACE ${STB_DIR})

# Interface libraries with main executable
include_directories(WarMorale ${GLEW_INCLUDE} ${GLFW_INCLUDE})
target_link_libraries(WarMorale ${GLEW} ${GLFW} ${OPENGL} STB)


# Unit tests
add_executable(WarMoraleTests src/tests/tests.cpp)
target_link_libraries(WarMoraleTests Catch)
enable_testing()
add_test(NAME Tests COMMAND WarMoraleTests)