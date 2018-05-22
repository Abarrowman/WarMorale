#pragma once

class world;

#include "stage.h"
#include "sprite.h"
#include "team.h"

class world : public stage {

public:

  long frame_count = 0;

  team* player_team;
  team* enemy_team;
  legion* player_first_legion;
  legion* enemy_first_legion;


  ordered_parent* teams_layer;
  ordered_parent* effects_layer;

  world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr);

  bool update() override;
  
  void key_callback(int key, int scancode, int action, int mods) override;
  void cursor_position_callback(double xpos, double ypos) override;


  /*~world() {
    fprintf(stderr, "World Destructor\n");
  }*/

private:
  vector_2f window_to_world(double xpos, double ypos);
};