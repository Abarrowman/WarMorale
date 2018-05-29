#pragma once
#include "renderable.h"
#include "sprite.h"
#include "utils.h"
#include "unit_face.h"

class threat : public renderable {
public:
  int damage;

  threat(int dp) : damage(dp) {}

  virtual void hurt(unit& target) = 0;
  virtual ~threat() {} // this is a base clase
};

class point_threat : public threat {
protected:
  sprite image;
  bool destroyed = false;

public:
  trans_state trans;
  point_threat(sprite img, int dp) : image(img), threat(dp) {}

  void hurt(unit& target) override {
    if (destroyed) {
      return;
    }
    if (target.take_point_threat(*this)) {
      destroyed = true;
    }
  }

  bool update() {
    local_trans = trans.to_matrix();
    return destroyed;
  }

  void render(matrix_3f const& parent_trans) override {
    if (!visible) {
      return;
    }
    matrix_3f trans = parent_trans * local_trans;
    image.render(trans);
  }

  virtual ~point_threat() {} // this is a base clase
};

using threat_parent = renderable_parent<threat, true>;