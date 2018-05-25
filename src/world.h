#pragma once

#include "world_face.h"
#include "unit.h"
#include "team.h"
#include "units/grunt.h"

inline world::world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr) : stage(win, sr, dr) {
  s_ctx.init(&static_res.get_shader(static_shader_id::sprite), &static_res.get_vertex_array(static_vertex_array_id::sprite));
  p_ctx.init(&static_res.get_shader(static_shader_id::polygon_fill), &static_res.get_shader(static_shader_id::line));

  tri = new owning_polygon(&p_ctx, vertex_array::create_triangle());
  tri->fill_color.floats = {0.0, 0.5f, 0.5f, 1.0f};
  tri->edge_color.floats = { 0.0, 0.2f, 0.2f, 1.0f };
  tri->edge_width = 0.3f;//0.1f;
  add_orphan(tri);


  teams_layer = new ordered_parent();
  add_orphan(teams_layer);
    
  player_team = new team();
  enemy_team = new team();
  teams_layer->add_orphan(player_team);
  teams_layer->add_orphan(enemy_team);

  legion& p_first = player_team->create_legion();
  player_first_legion = &p_first;
  p_first.order.pos = { 100, 100 };
  player_team->add_orphan(new grunt(*this, *player_team, &p_first)).trans.angle = PI_F / 2;

  legion& e_first = enemy_team->create_legion();
  enemy_first_legion = &e_first;
  enemy_team->add_orphan(new grunt(*this, *enemy_team, &e_first)).trans.x = 100;

  enemy_team->establish_hostility(player_team);

  threat_layer = new threat_parent();
  sprite fire_sprite = static_sprite(static_texture_id::fire);
  fire_sprite.local_trans = matrix_3f::transformation_matrix(32, 32);
  point_threat* fire = new point_threat(fire_sprite, 10);
  fire->trans.set_position({200, 200});
  threat_layer->add_orphan(fire);
  add_orphan(threat_layer);

  over_effects_layer = new ordered_parent();
  add_orphan(over_effects_layer);

}

inline bool world::update() {
  frame_count += 1;

  s_ctx.update_projection(proj);
  p_ctx.update_projection(proj);

  float ang = frame_count / 100.0f;
  enemy_first_legion->order.pos = vector_2f::create_polar(ang, 100);
  player_first_legion->order.pos = mouse_pos;
  tri->local_trans = matrix_3f::transformation_matrix(100, 100, ang + PI_F);

  stage::update(); //update children

  // after children update
  return false;
}


inline void world::key_callback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

inline void world::cursor_position_callback(double xpos, double ypos) {
  mouse_pos = window_to_world(xpos, ypos);
}

inline void world::mouse_button_callback(int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      if (!mouse_down) {
        // press started
        //printf("Press\n");

        std::array<vector_2f, 4> square_arr{ {{1, 0}, {0, 1}, {-1, 0}, {0, -1}} };
        tri->arr.set_veritices(square_arr);

        /*std::array<vector_2f, 2> line_arr{{{ -1, 0 }, { 0, 1 }}};
        tri->arr.set_veritices(line_arr);*/
      }
      mouse_down = true;
    } else {
      if (mouse_down) {
        // release started
        //printf("Release\n");
        std::array<vector_2f, 3> tri_arr{{
          { 1, 0 },
          { cos(PI_F * 2 / 3), sin(PI_F * 2 / 3) },
          { cos(PI_F * 2 / 3), sin(PI_F * 4 / 3) }
         }};
        tri->arr.set_veritices(tri_arr);
      }
      mouse_down = false;
    }
  }
}

inline vector_2f world::window_to_world(double xpos, double ypos) {
  vector_2f fixed = { static_cast<float>(xpos) - width / 2, height / 2 - static_cast<float>(ypos) };
  return fixed;
}

inline sprite* world::static_sprite_orphan(static_texture_id id) {
  return s_ctx.create_orphan(&(static_res.get_texture(id)));
}

inline sprite world::static_sprite(static_texture_id id) {
  return s_ctx.create(&(static_res.get_texture(id)));
}
