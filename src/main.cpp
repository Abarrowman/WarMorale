#include <GLFW/glfw3.h>
#include <cmath>

int main(void) {
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit()) {
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
  
  float t = 0;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)){
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);
    
    t -= 0.001f;
    glBegin(GL_TRIANGLES);
    glColor3f(0.1f, 0.2f, 0.3f);
    glVertex3f(0, 0, 0);
    glVertex3f(cos(t), sin(t), 0);
    glVertex3f(-sin(t), cos(t), 0);
    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}