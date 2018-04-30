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
#include <memory>

extern "C" {
  __declspec(dllexport) unsigned int NvOptimusEnablement = 1;
}

extern "C"
{
  __declspec(dllexport) unsigned int AmdPowerXpressRequestHighPerformance = 1;
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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  float x_min = -320;
  float y_min = -240;
  float x_max = 320;
  float y_max = 240;
  matrix_3f proj = matrix_3f::orthographic_projection(x_min, x_max, y_min, y_max);


  float angle = 0;

  std::string const fragment_shader = read_file_to_string("./assets/shaders/2d.frag");
  std::string const vertex_shader = read_file_to_string("./assets/shaders/2d.vert");
  shader sprite_shader{ vertex_shader.c_str(), fragment_shader.c_str() };
  texture face_texture{"./assets/textures/sad.png"};
  texture fire_texture{ "./assets/textures/fire.png" };
  vertex_array sprite_vertex = vertex_array::create_rectangle();

  sprite_context ctx{ sprite_shader, sprite_vertex };
  ctx.update_projection(proj);

  sprite face{ctx, face_texture};
  sprite fire{ctx, fire_texture};


  ordered_parent container;
  container.add_orphan(new sprite(ctx, fire_texture));
  container.add_orphan(new sprite(ctx, fire_texture));
  container.child_at(0).local_trans = matrix_3f::transformation_matrix(120, 120, 0, 110);
  static_cast<sprite&>(container.child_at(1)).local_trans = matrix_3f::transformation_matrix(-120, 120, 0, -110);


  vertex_array tru = vertex_array::create_triangle();

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)){

    // Start Update
    angle += 0.001f;
    face.local_trans = matrix_3f::transformation_matrix(480, 480, angle);
    fire.local_trans = matrix_3f::transformation_matrix(240, 240, -angle, 240);
    container.local_trans = matrix_3f::transformation_matrix(1, 1, angle);
    //ctx.update_projection(proj);
    // End Update

    // Start Render
    glClear(GL_COLOR_BUFFER_BIT);

    matrix_3f global_trans = matrix_3f::identity();
    face.render(global_trans);
    fire.render(global_trans);
    container.render(global_trans);

    glfwSwapBuffers(window);
    // End Render

      
    glfwPollEvents();
    check_gl_errors();
  }

  glfwTerminate();
  return 0;
}