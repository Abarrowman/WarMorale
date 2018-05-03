#pragma once

#include "utils.h"
#include "sprite.h"
#include "resources.h"

class world : public ordered_parent {
protected:
  virtual bool inner_update() {

    angle += 0.001f;

    face->local_trans = matrix_3f::transformation_matrix(480, 480, angle);

    fire->local_trans = matrix_3f::transformation_matrix(240, 240, -angle, 240);
    container->local_trans = matrix_3f::transformation_matrix(1, 1, angle);

    ctx.update_projection(proj);
    return false;
  }

public:
  static_resources& static_res;
  sprite_context ctx;
  matrix_3f proj;

  //hard coded objects
  float angle;
  sprite* face;
  sprite* fire;
  ordered_parent* container;


  world(static_resources& sr) : static_res(sr) {
    ctx.init(&static_res.get_shader(static_shader_id::sprite), &static_res.get_vertex_array(static_vertex_array_id::sprite));

    float x_min = -320;
    float y_min = -240;
    float x_max = 320;
    float y_max = 240;
    proj = matrix_3f::orthographic_projection(x_min, x_max, y_min, y_max);

    angle = 0;
    face = new sprite(&ctx, &static_res.get_texture(static_texture_id::face));
    add_orphan(face);

    fire = new sprite(&ctx, &static_res.get_texture(static_texture_id::fire));
    add_orphan(fire);

    container = new ordered_parent();
    container->add_orphan(new sprite(&ctx, &static_res.get_texture(static_texture_id::fire)));
    container->add_orphan(new sprite(&ctx, &static_res.get_texture(static_texture_id::fire)));
    container->add_orphan(new sprite(&ctx, &static_res.get_texture(static_texture_id::ship)));
    container->child_at(0).local_trans = matrix_3f::transformation_matrix(120, 120, 0, 110);
    container->child_at(1).local_trans = matrix_3f::transformation_matrix(-120, 120, 0, -110);
    add_orphan(container);

    sprite& ship_sprite = static_cast<sprite&>(container->child_at(2));
    ship_sprite.frame_width = 4;
    ship_sprite.frame_height = 4;
    ship_sprite.local_trans = matrix_3f::transformation_matrix(32, 32, PI_F / 2);

  }
};