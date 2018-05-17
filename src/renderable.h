#pragma once
#include "matrix_3f.h"
#include <vector>
#include <memory>
#include <algorithm>

class renderable {
public:
  matrix_3f local_trans = matrix_3f::identity();
  bool visible = true;

  /*
  Renders this renderable with a parent transformation.
  */
  virtual void render(matrix_3f const& parent_trans) = 0;
  
  /*
  Updates the renderable.
  Return true if the renderable should be deleted by its parent.
  */
  virtual bool update() {
    return false;
  }
  
  virtual ~renderable() {} // this is a base class
};

template<bool ordered>
class renderable_parent : public renderable {
private:
  std::vector<std::unique_ptr<renderable>> children;

public:

  void add_orphan(renderable* child) {
    children.emplace_back(child);
  }

  void add_child(std::unique_ptr<renderable> child) {
    children.push_back(std::move(child));
  }

  renderable& child_at(int idx) {
    return *(children[idx].get());
  }

  /*
  Removes all children.
  O(n)
  */
  void remove_all_children() {
    children.clear();
  }

  /*
  Removes the child at the given index.
  Ordered - O(n)
  Not Ordered - O(1)
  */
  void remove_child_at(int idx) {
    if (ordered) {
      children.erase(children.cbegin() + idx);
    } else {
      std::swap(children[idx], children[children.size() - 1]);
      children.pop_back();
    }
  }

  /*
  Finds the index of the child.
  Returns -1 if the child is not one of the renderable_parent's children.
  O(n)
  */
  int index_of_child(renderable& target) {
    renderable* target_address = &target;
    auto it = std::find_if(children.begin(), children.end(), [target_address](std::unique_ptr<renderable> const& child) {
      return (target_address == child.get());
    });
    if (it == children.end()) {
      return -1;
    } else {
      auto diff = it - children.begin();
      return static_cast<int>(diff);
    }
  }

  /*
  Removes the child from the parent.
  O(n)
  */
  void remove_child(renderable& child) {
    int idx = index_of_child(child);
    if (idx != -1) {
      remove_child_at(idx);
    }
  }

  int child_count() {
    return static_cast<int>(children.size());
  }

  renderable_parent() {};
  virtual ~renderable_parent() {} // this is a base class
  renderable_parent(renderable_parent&) = delete; // do not copy

  virtual void render(matrix_3f const& parent_trans) {
    if (!visible) {
      return;
    }
    matrix_3f trans = parent_trans * local_trans;
    for (int i = 0; i < child_count(); i++) {
      child_at(i).render(trans);
    }
  }

  virtual bool update() {
    for (int i = child_count() - 1; i >= 0; i--) {
      if (child_at(i).update()) {
        remove_child_at(i);
      }
    }
    return false;
  }
};

using ordered_parent = renderable_parent<true>;
using unordered_parent = renderable_parent<false>;
