cmake_minimum_required(VERSION 3.0.0)
project(WarMorale)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 14)

include_directories(src)
file(GLOB SOURCES src/*.cpp)


add_executable(WarMorale ${SOURCES})



if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Hide Console
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  
  set(GLEW_DIR "C:\\A\\Git\\ThirdParty\\C\\glew-2.1.0")
  set(GLFW_DIR "C:\\A\\Git\\ThirdParty\\C\\glfw-3.2.1")
  find_library(GLEW glew32 HINTS "${GLEW_DIR}\\lib\\Release\\Win32")
  find_library(GLFW glfw3 HINTS "${GLFW_DIR}\\lib-vc2015")
  find_library(OPENGL opengl32)
  set(GLEW_INCLUDE "${GLEW_DIR}\\include")
  set(GLFW_INCLUDE "${GLFW_DIR}\\include")
endif()



include_directories(WarMorale ${GLEW_INCLUDE} ${GLFW_INCLUDE})
target_link_libraries(WarMorale ${GLEW} ${GLFW} ${OPENGL})