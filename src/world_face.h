#pragma once

class world;

#include "stage.h"
#include "team.h"
#include "sprite.h"
#include "polygon.h"
#include "matrix_3f.h"

class world : public stage {

public:

  sprite_context s_ctx;
  polygon_context p_ctx;
  long frame_count = 0;
  vector_2f mouse_pos = { 0, 0 };
  bool mouse_down = false;

  team* player_team;
  team* enemy_team;
  legion* player_first_legion;
  legion* enemy_first_legion;


  ordered_parent* teams_layer;
  ordered_parent* effects_layer;

  owning_polygon* tri;

  world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr);

  bool update() override;
  
  void key_callback(int key, int scancode, int action, int mods) override;
  void cursor_position_callback(double xpos, double ypos) override;
  void mouse_button_callback(int button, int action, int mods) override;


  sprite* world::static_sprite_orphan(static_texture_id id);


  /*~world() {
    fprintf(stderr, "World Destructor\n");
  }*/

private:
  vector_2f window_to_world(double xpos, double ypos);
};