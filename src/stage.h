#pragma once

#include "gl_includes.h"
#include "sprite.h"
#include "resources.h"

class stage : public ordered_parent {
public:
  GLFWwindow* window;
  static_resources& static_res;
  int_keyed_resources& dynamic_res;
  matrix_3f proj;
  int width;
  int height;

  stage(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr) : window(win), static_res(sr), dynamic_res(dr) {

    glfwGetFramebufferSize(window, &width, &height);

    float x_min = -width / 2.0f;
    float y_min = -height / 2.0f;
    float x_max = width / 2.0f;
    float y_max = height / 2.0f;
    proj = matrix_3f::orthographic_projection(x_min, x_max, y_min, y_max);
  }

  virtual void key_callback(int key, int scancode, int action, int mods) {}

  virtual void cursor_position_callback(double xpos, double ypos) {}

  virtual void mouse_button_callback(int button, int action, int mods) {};

  virtual ~stage() {} // this is a base class

};