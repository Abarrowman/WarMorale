#pragma once


#include "../world_face.h"
#include "../team_face.h"
#include "../unit_face.h"
#include "../threat_face.h"
#include "../potential_field.h"


class grunt : public unit {
public:
  static unit_archetype const& get_archetype() {
    static unit_archetype const archetype{
      10, // max_health
      16.0f, // potential_radius
      10.0f, // max_speed
      0.05f, // max_turn_speed
      10 // max_reload
    };
    return archetype;
  };

private:
  sprite ship;
  sharing_polygon poly;

public:
  grunt(world& w, team& t, legion* l) :
      unit(w, t, l, get_archetype()),
      poly(&(world_ref.p_ctx), &(world_ref.static_res.get_vertex_array(static_vertex_array_id::dodecagon))) {
    ship = world_ref.static_sprite(static_texture_id::grunt);
    ship.local_trans = matrix_3f::transformation_matrix(32, 32);
    poly.edge_width = 1.0f / type.potential_radius;
    poly.local_trans = matrix_3f::transformation_matrix(type.potential_radius, type.potential_radius);
    poly.edge_color = team_ref.col.with_alpha(0.5);
    poly.fill_color = color::transparent_black();
  }

protected:
  void death_action() override {
    vector_2f center = trans.get_position();
    matrix_3f parent_trans = trans.to_matrix();
    world_ref.over_effects_layer->add_orphan(explosion_effect::explode_sprite(&(world_ref.pp_ctx), ship, center, parent_trans, world_ref.get_generator()));
  }

  void render(matrix_3f const& parent_trans) override {
    if (visible) {
      matrix_3f matrix = parent_trans * trans.to_matrix();
      inner_variadic_render(matrix, ship, poly);
    }
  }
};

