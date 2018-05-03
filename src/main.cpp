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

#ifdef _MSC_VER
extern "C" {
  __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
}

extern "C"
{
  __declspec(dllexport) unsigned int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return -1;
  }
  
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "War Morale", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  // start GLEW extension handler
  glewExperimental = GL_TRUE;
  glewInit();

  const GLubyte* renderer = glGetString(GL_RENDERER);
  const GLubyte* version = glGetString(GL_VERSION);
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  static_resources sr;
  world w(sr);
  matrix_3f global_trans = matrix_3f::identity();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)){
    w.update();

    glClear(GL_COLOR_BUFFER_BIT);
    w.render(global_trans);
    glfwSwapBuffers(window);
      
    glfwPollEvents();
    check_gl_errors();
  }

  glfwTerminate();
  return 0;
}