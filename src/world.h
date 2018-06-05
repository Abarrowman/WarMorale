#pragma once

#include "world_face.h"
#include "unit.h"
#include "team.h"
#include "units/grunt.h"
#include "kd_tree.h"


inline world::world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr) : stage(win, sr, dr) {
  std::array<unsigned int, 4> seed = world::get_seed();
  printf("Seeded PRNG with %08x-%08x-%08x-%08x\n", seed[0], seed[1], seed[2], seed[3]);
  std::seed_seq seed_val{ seed[0], seed[1], seed[2], seed[3] };
  gen = generator_type(seed_val);

  s_ctx.init(&static_res.get_shader(static_shader_id::sprite), &static_res.get_vertex_array(static_vertex_array_id::sprite));
  p_ctx.init(&static_res.get_shader(static_shader_id::polygon_fill), &static_res.get_shader(static_shader_id::line));
  pp_ctx.init(&static_res.get_shader(static_shader_id::point_particle));
  bt_ctx.init(&static_res.get_shader(static_shader_id::fixed_width_bitmap_text), &static_res.get_shader(static_shader_id::variable_width_bitmap_text));



  tri = add_orphan(new owning_polygon(&p_ctx, simple_vertex_array::create_triangle()));
  tri->fill_color.floats = {0.0, 0.5f, 0.5f, 1.0f};
  tri->edge_color.floats = { 0.0, 0.2f, 0.2f, 1.0f };
  tri->edge_width = 0.3f;//0.1f;
  


  teams_layer = add_orphan(new team_parent());
    
  player_team = teams_layer->add_orphan(new team());
  enemy_team = teams_layer->add_orphan(new team());

  legion& p_first = player_team->create_legion();
  player_first_legion = &p_first;
  p_first.order.pos = { 100, 100 };
  player_team->add_orphan(new grunt(*this, *player_team, &p_first))->trans.angle = PI_F / 2;
  player_team->add_orphan(new grunt(*this, *player_team, &p_first))->trans.set_position({-100, -200});


  legion& e_first = enemy_team->create_legion();
  enemy_first_legion = &e_first;
  enemy_team->add_orphan(new grunt(*this, *enemy_team, &e_first))->trans.x = 100;

  enemy_team->establish_hostility(player_team);

  threat_layer = add_orphan(new threat_parent());
  sprite fire_sprite = static_sprite(static_texture_id::fire);
  fire_sprite.local_trans = matrix_3f::transformation_matrix(32, 32);
  point_threat* fire = threat_layer->add_orphan(new point_threat(fire_sprite, 10));
  fire->trans.set_position({200, 200});

  over_effects_layer = add_orphan(new ordered_parent());
  

  ui_layer = add_orphan(new ordered_parent());

  {
    frame_rate_text = ui_layer->add_orphan(new fixed_width_bitmap_text(&bt_ctx, &(static_res.get_mono_font(static_mono_font_id::consolas_12))));
    vector_2f trans = window_to_world(0, 0);
    frame_rate_text->local_trans = matrix_3f::transformation_matrix(1, 1, 0, trans.x, trans.y);
    frame_rate_text->text_color = { 1, 1, 0, 1 };
  }

  {
    log_text = ui_layer->add_orphan(new variable_width_bitmap_text(&bt_ctx, &(static_res.get_prop_font(static_prop_font_id::impact_24)), "Hello World!"));
    vector_2f trans = window_to_world(0, height - log_text->font->char_height());
    log_text->local_trans = matrix_3f::transformation_matrix(1, 1, 0, trans.x, trans.y);
    log_text->text_color = { 0, 1, 1, 1 };
    
  }

  {
    kd_tree<char> kd{{
      { { 2, 3 }, 'a'},
      { { 5, 4 }, 'b' },
      { { 9, 6 }, 'c' },
      { { 4, 7 }, 'd' },
      { { 8, 1 }, 'e' },
      { { 7, 2 }, 'f' },
    }};

    /*std::vector<kd_node<char>> nodes;
    int node_count = 1000;
    for (int n = 0; n < node_count; n++) {
      nodes.push_back({{ rand_float(gen) * 10.0f , rand_float(gen)  * 10.0f }, static_cast<char>(n)});
    }
    kd_tree<char> kd{ std::move(nodes) };*/

    for (float x = -1; x < 11; x += 0.1f) {
      for (float y = -1; y < 11; y += 0.1f) {
        vector_2f pos{ x, y };
        kd_node<char>* close_rec = kd.find_closest_recursive(pos);
        kd_node<char>* close_brute = kd.find_closest_brute(pos);
        kd_node<char>* close_it = kd.find_closest_iterative(pos);

        if (close_rec != close_brute) {
          float dis_brute = (close_brute->pos - pos).magnitude();
          float dis_rec = (close_rec->pos - pos).magnitude();
          if (dis_brute < dis_rec) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-1);
          }
        }
        if (close_it != close_brute) {
          float dis_brute = (close_brute->pos - pos).magnitude();
          float dis_it = (close_it->pos - pos).magnitude();
          if (dis_brute < dis_it) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-1);
          }
        }

        size_t count = 3;
        std::vector<kd_node_dist<char>> closests_brute = kd.find_count_closest_brute(pos, count);
        if (closests_brute.size() != count) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }

        if (closests_brute[0].ptr != close_brute) {
          float dis_brute = (close_brute->pos - pos).magnitude();
          if (dis_brute < closests_brute[0].dist) {
            fprintf(stderr, "Error at (%f, %f)\n", x, y);
            exit(-1);
          }
        }

        std::vector<kd_node_dist<char>> closests_rec = kd.find_count_closest_recursive(pos, count);
        if (closests_rec.size() != count) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }
        for (size_t i = 0; i < count; i++) {
          if (closests_brute[i].ptr != closests_rec[i].ptr) {
            if (closests_brute[i].dist < closests_rec[i].dist) {
              fprintf(stderr, "Error at (%f, %f)\n", x, y);
              exit(-1);
            }
          }
        }

        std::vector<kd_node_dist<char>> closests_it = kd.find_count_closest_iterative(pos, count);
        if (closests_it.size() != count) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }
        for (size_t i = 0; i < count; i++) {
          if (closests_brute[i].ptr != closests_it[i].ptr) {
            if (closests_brute[i].dist < closests_it[i].dist) {
              fprintf(stderr, "Error at (%f, %f)\n", x, y);
              exit(-1);
            }
          }
        }

        std::vector<kd_node_dist<char>> within_brtue = kd.find_within_brute(pos, 3.0f);
        std::vector<kd_node_dist<char>> within_rec = kd.find_within_recursive(pos, 3.0f);
        if (within_rec.size() != within_brtue.size()) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }
        for (size_t i = 0; i < within_brtue.size(); i++) {
          if (within_brtue[i].ptr != within_rec[i].ptr) {
            if (within_brtue[i].dist < within_rec[i].dist) {
              fprintf(stderr, "Error at (%f, %f)\n", x, y);
              exit(-1);
            }
          }
        }

        std::vector<kd_node_dist<char>> within_it = kd.find_within_iterative(pos, 3.0f);
        if (within_it.size() != within_brtue.size()) {
          fprintf(stderr, "Error at (%f, %f)\n", x, y);
          exit(-1);
        }
        for (size_t i = 0; i < within_brtue.size(); i++) {
          if (within_brtue[i].ptr != within_it[i].ptr) {
            if (within_brtue[i].dist < within_it[i].dist) {
              fprintf(stderr, "Error at (%f, %f)\n", x, y);
              exit(-1);
            }
          }
        }

      }
    }

  }
}

inline bool world::update() { 
  frm.count_frame();
  frame_count += 1;
  frame_rate_text->text = string_format("FPS:%3.1f", frm.average_frame_rate());

  float ang = frame_count / 100.0f;
  enemy_first_legion->order.pos = vector_2f::create_polar(ang, 100);
  player_first_legion->order.pos = mouse_pos;
  tri->local_trans = matrix_3f::transformation_matrix(100, 100, ang + PI_F);

  stage::update(); //update children



  // after children update
  s_ctx.update_projection(proj);
  p_ctx.update_projection(proj);
  pp_ctx.update_projection(proj);
  bt_ctx.update_projection(proj);
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

inline std::array<unsigned int, 4> world::get_seed() {
  /*std::random_device rd;
  unsigned int a = rd();
  unsigned int b = rd();
  unsigned int c = rd();
  unsigned int d = rd();
  // C++ has unspecified execution order of arguments thus the temporaries
  return { { a, b, c, d } };*/
  return { { 0xabedbead, 0xcafebabe, 0xdeadbeef, 0xfacefeed } };
}

inline sprite* world::static_sprite_orphan(static_texture_id id) {
  return s_ctx.create_orphan(&(static_res.get_texture(id)));
}

inline sprite world::static_sprite(static_texture_id id) {
  return s_ctx.create(&(static_res.get_texture(id)));
}

inline generator_type& world::get_generator() {
  return gen;
}
