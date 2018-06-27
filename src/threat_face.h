#pragma once

#pragma once
#include "renderable.h"
#include "sprite.h"
#include "utils.h"
#include "unit_face.h"
#include "team_face.h"
#include "space_buckets.h"
#include "explosion_effect.h"
#include "world_face.h"

class threat {
public:
  trans_state trans;
  bool visible = true;
  int damage; // TODO maybe this should be const

  threat(int dp) : damage(dp) {}

  virtual bool update() = 0;
  virtual void render(matrix_3f const& parent_trans) = 0;
  virtual void hurt(unit& target) = 0;
  virtual ~threat() {} // this is a base clase
};



//using threat_parent = renderable_parent<threat, true>;

class threat_parent : public renderable_parent<threat, true> {
private:
  using parent_type = renderable_parent<threat, true>;
  space_buckets<threat*> buckets{ 50 };

public:

  bool update() override {
    for (int i = child_count() - 1; i >= 0; i--) {
      threat& ob = child_at(i);
      buckets.remove_entry(ob.trans.get_position(), &ob);
      if (ob.update()) {
        remove_child_at(i);
      } else {
        buckets.add_entry(ob.trans.get_position(), &ob);
      }
    }
    return false;
  }

  std::array<std::vector<threat*>*, 9>  get_nearby_threats(vector_2f location) {
    return buckets.find_nearby_buckets(location);
  }
};


class point_threat : public threat {
protected:
  sprite image;
  bool destroyed = false;
  vector_2f velocity;

  // How many frames the point_threat should last for.
  // When negative the point_threat will last forever.
  int lifetime;

  team* allegiance;

public:

  point_threat(sprite img, int damage_points, vector_2f vel, int life, team* ally) :
    image(img), threat(damage_points), velocity(vel), lifetime(life), allegiance(ally) {
  }

  void hurt(unit& target) override;
  bool update() override;
  void render(matrix_3f const& parent_trans) override;

  virtual ~point_threat() {} // this is a base clase
};