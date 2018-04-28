#pragma once
#include "matrix_3f.h"

class renderable {
public:
  virtual void render(matrix_3f const& parent_trans) = 0;
};