#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <cmath>
#include <cstdio>

#include "shader.h"
#include "object.h"
#include "utils.h"

extern "C" {
  _declspec(dllexport) int NvOptimusEnablement = 1;
}

extern "C"
{
  __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return -1;
  }
  
  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  float t = 0;

  std::string const fragment_shader = read_file_to_string("./Debug/assets/shaders/2d.frag");

  std::string const vertex_shader = read_file_to_string("./Debug/assets/shaders/2d.vert");

  shader s{ vertex_shader.c_str(), fragment_shader.c_str() };
  object o;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)){
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    t += 0.001f;
    /*glBegin(GL_TRIANGLES);
    glColor3f(0.1f, 0.2f, 0.3f);
    glVertex3f(0, 0, 0);
    glVertex3f(cos(t), sin(t), 0);
    glVertex3f(-sin(t), cos(t), 0);
    glEnd();*/

    glUseProgram(s.program);

    float transMat[] = {
      cos(t), -sin(t), 0,
      sin(t), cos(t), 0,
      0, 0, 1
    };

    glUniformMatrix3fv(glGetUniformLocation(s.program, "transMat"), 1, GL_TRUE, transMat);
    glBindVertexArray(o.vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    check_gl_errors();
  }

  glfwTerminate();
  return 0;
}