#pragma once

#include "world_face.h"
#include "unit.h"
#include "team.h"
#include "units/grunt.h"
#include "space_buckets.h"


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
  enemy_team->establish_hostility(player_team);

  {
    legion& p_first = player_team->create_legion();
    player_first_legion = &p_first;
    p_first.order.pos = { 100, 100 };
    for (int i = 0; i < 100; i++) {
      grunt* g = player_team->add_orphan(new grunt(*this, *player_team, &p_first));
      g->trans.x = -300 + 100.0f * rand_centered_float(get_generator());
      g->trans.y = 100.0f * rand_centered_float(get_generator());
    }
  }

  {
    legion& e_first = enemy_team->create_legion();
    enemy_first_legion = &e_first;
    for (int i = 0; i < 100; i++) {
      grunt* g = enemy_team->add_orphan(new grunt(*this, *enemy_team, &e_first));
      g->trans.x = 100.0f * rand_centered_float(get_generator());
      g->trans.y = 100.0f * rand_centered_float(get_generator());
    }
  }


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
    space_buckets<int> buckets{ 100 };
    buckets.add_entry({ 0, 0 }, 0);
    buckets.add_entry({ 10, 10 }, 1);
    buckets.add_entry({ 50, 110 }, 2);
    buckets.add_entry({ 800, 900 }, 3);

    vector_2f search_loc{ 30.0f, 30.0f };

    auto range = buckets.find_local_bucket(search_loc);
    for (auto it = range.first; it != range.second; ++it) {
      printf("Found Locally %d\n", it->second);
    }

    for (int i : buckets.find_nearby_buckets(search_loc)) {
      printf("Found Nearby %d\n", i);
    }

    buckets.move_entry({ 0, 0 }, { 820, 910 }, 0);

    for (int i : buckets.find_nearby_buckets(search_loc)) {
      printf("Found Nearby After Move %d\n", i);
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
  tri->local_trans = matrix_3f::transformation_matrix(100, 100, ang + math_consts::pi());

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
          { cos(math_consts::pi() * 2 / 3), sin(math_consts::pi() * 2 / 3) },
          { cos(math_consts::pi() * 2 / 3), sin(math_consts::pi() * 4 / 3) }
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
