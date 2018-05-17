#pragma once

#include "stage.h"
#include "sprite.h"
class world : public stage {

public:
  float angle;
  sprite* face;
  sprite* fire;
  ordered_parent* container;


  world(GLFWwindow* win, static_resources& sr, int_keyed_resources& dr) : stage(win, sr, dr) {
    angle = 0;
    face = new sprite(&ctx, &static_res.get_texture(static_texture_id::face));
    add_orphan(face);

    fire = new sprite(&ctx, &static_res.get_texture(static_texture_id::fire));
    add_orphan(fire);

    dynamic_res.add_texutre(0, std::make_unique<texture>("./assets/textures/ship.png"));
    texture* ship_texture = &(dynamic_res.get_texture(0));

    container = new ordered_parent();
    container->add_orphan(new sprite(&ctx, &static_res.get_texture(static_texture_id::fire)));
    container->add_orphan(new sprite(&ctx, &static_res.get_texture(static_texture_id::fire)));
    container->add_orphan(new sprite(&ctx, ship_texture));
    container->child_at(0).local_trans = matrix_3f::transformation_matrix(120, 120, 0, 110);
    container->child_at(1).local_trans = matrix_3f::transformation_matrix(-120, 120, 0, -110);
    add_orphan(container);

    sprite& ship_sprite = static_cast<sprite&>(container->child_at(2));
    ship_sprite.frame_width = 4;
    ship_sprite.frame_height = 4;
    ship_sprite.local_trans = matrix_3f::transformation_matrix(32, 32, PI_F / 2);

  }

  bool update() override {
    // before children update
    angle += 0.001f;

    face->local_trans = matrix_3f::transformation_matrix(480, 480, angle);

    fire->local_trans = matrix_3f::transformation_matrix(240, 240, -angle, 240);
    container->local_trans = matrix_3f::transformation_matrix(1, 1, angle);

    ctx.update_projection(proj);

    stage::update(); //update children

    // after children update
    return false;
  }


  void key_callback(int key, int scancode, int action, int mods) override {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }
};