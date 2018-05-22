#include "utils.h"

#include <cmath>
#include <cstdio>

#include "shader.h"
#include "vertex_array.h"
#include "utils.h"
#include "texture.h"
#include "matrix_3f.h"
#include "renderable.h"
#include "sprite.h"
#include "world.h"
#include "resources.h"
#include <memory>

stage* global_stage = nullptr;

#ifdef _MSC_VER
extern "C" {
  __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
}

extern "C"
{
  __declspec(dllexport) unsigned int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (global_stage != nullptr) {
    global_stage->key_callback(key, scancode, action, mods);
  }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
  if (global_stage != nullptr) {
    global_stage->cursor_position_callback(xpos, ypos);
  }
}

int main(void) {

  /* Initialize the library */
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return -1;
  }
  
  /* Create a windowed mode window and its OpenGL context */
  bool windowed = true;
  int window_width = 1280;
  int window_height = 720;
  char const* window_title = "War Morale";
  GLFWwindow* window;
  if (windowed) {
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(window_width, window_height, window_title, NULL, NULL);
  } else {
    window = glfwCreateWindow(window_width, window_height, window_title, glfwGetPrimaryMonitor(), NULL);
  }

  if (!window) {
    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, cursor_position_callback);
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  GLubyte const* renderer = glGetString(GL_RENDERER);
  GLubyte const* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  static_resources sr;
  int_keyed_resources dr;
  world w(window, sr, dr);
  global_stage = &w;
  matrix_3f global_trans = matrix_3f::identity();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)){
    global_stage->update();

    glClear(GL_COLOR_BUFFER_BIT);
    global_stage->render(global_trans);
    glfwSwapBuffers(window);
      
    glfwPollEvents();
    check_gl_errors();
  }

  glfwTerminate();
  return 0;
}