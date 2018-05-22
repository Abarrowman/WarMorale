#pragma once

#include "utils.h"
#include "sprite.h"
#include "resources.h"

class stage : public ordered_parent {
public:
  GLFWwindow * window;
  static_resources& static_res;
  int_keyed_resources& dynamic_res;
  sprite_context ctx;
  matrix_3f proj;
  int width;
  int height;

  stage(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr) : window(win), static_res(sr), dynamic_res(dr) {
    ctx.init(&static_res.get_shader(static_shader_id::sprite), &static_res.get_vertex_array(static_vertex_array_id::sprite));

    glfwGetFramebufferSize(window, &width, &height);

    float x_min = -width / 2.0f;
    float y_min = -height / 2.0f;
    float x_max = width / 2.0f;
    float y_max = height / 2.0f;
    proj = matrix_3f::orthographic_projection(x_min, x_max, y_min, y_max);
  }

  sprite* static_sprite_orphan(static_texture_id id) {
    return ctx.create_orphan(&(static_res.get_texture(id)));
  }


  virtual void key_callback(int key, int scancode, int action, int mods) {}

  virtual void cursor_position_callback(double xpos, double ypos) {}

  virtual ~stage() {} // this is a base class

};