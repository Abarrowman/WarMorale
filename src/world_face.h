#pragma once

class world;

#include <random>
#include "stage.h"
#include "team.h"
#include "sprite.h"
#include "polygon.h"
#include "2d_math.h"
#include "threat_face.h"
#include "explosion_effect.h"
#include "text/bitmap_text.h"
#include "running_average.h"
#include "gl_includes.h"
#include "space_buckets.h"
#include "obstacle.h"

class world : public stage {

public:

  sprite_context s_ctx;
  polygon_context p_ctx;
  point_particle_context pp_ctx;
  bitmap_text_context bt_ctx;


  frame_rate_meter frm;
  averaging_timer update_times;

  long frame_count = 0;
  vector_2f mouse_pos;
  bool mouse_down = false;

  /*
  If buckets aretoo small big units potential fields do not propogate far enough.
  */
  space_buckets<unit_reference> unit_buckets{200};

  team* player_team;
  team* enemy_team;
  legion* player_first_legion;
  legion* enemy_first_legion;
  owning_polygon* player_first_legion_formation;
  owning_polygon* enemy_first_legion_formation;



  owning_polygon* tri;

  using explosion_parent = sparse_parent<explosion_effect, 1024>;



  // Begin rendering layers
  ordered_parent* under_effects_layer; // A container for effects to render underneath the main game elements
  obstacle_parent* obstacle_layer;
  team_parent* teams_layer;
  threat_parent* threat_layer;
  explosion_parent* explosion_layer;
  ordered_parent* over_effects_layer; // A container for effects to render over-top the main game elements
  ordered_parent* ui_layer;
  // End rendering layers

  mono_bitmap_text* frame_rate_text;
  prop_bitmap_text* log_text;

  world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr);

  bool update() override;
  
  void key_callback(int key, int scancode, int action, int mods) override;
  void cursor_position_callback(double xpos, double ypos) override;
  void mouse_button_callback(int button, int action, int mods) override;


  sprite* world::static_sprite_orphan(static_texture_id id);
  sprite world::static_sprite(static_texture_id id);

  generator_type& get_generator();

  void add_explosion(explosion_effect e);

  /*~world() {
    fprintf(stderr, "World Destructor\n");
  }*/

private:
  vector_2f window_to_world(double xpos, double ypos);
  generator_type gen;
  static std::array<unsigned int, 4> get_seed();
};