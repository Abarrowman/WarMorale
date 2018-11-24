#pragma once
#include "2d_math.h"
#include "sparse_container.h"
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

template<typename child_type, bool ordered>
class renderable_parent : public renderable {
private:
  std::vector<std::unique_ptr<child_type>> children;

public:

  /*
  Takes ownership of the raw pointer child adding it to the parent.
  */
  template<typename child_sub_type>
  child_sub_type* add_orphan(child_sub_type* child) {
    children.emplace_back(child);
    return child;
  }

  void add_child(std::unique_ptr<child_type> child) {
    children.push_back(std::move(child));
  }

  child_type& child_at(int idx) {
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
  int index_of_child(child_type& target) {
    child_type* target_address = &target;
    auto it = std::find_if(children.begin(), children.end(), [target_address](std::unique_ptr<child_type> const& child) {
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
  void remove_child(child_type& child) {
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

template<typename T, size_t N>
class sparse_parent : public renderable {
private:
  sparse_container<T, N> children;
public:

  T& push(T const& value) {
    return children.push(value);
  }

  T& push(T&& value) {
    return children.push(std::move(value));
  }

  template<typename... Args>
  T& emplace(Args&&... args) {
    return children.emplace(args);
  }

  int child_count() {
    return static_cast<int>(children.size());
  }

  virtual void render(matrix_3f const& parent_trans) {
    if (!visible) {
      return;
    }
    matrix_3f trans = parent_trans * local_trans;
    for (T& child: children) {
      child.render(trans);
    }
  }

  virtual bool update() {
    for (auto it = children.begin(); it != children.end(); ++it) {
      if (it->update()) {
        children.erase(it);
      }
    }
    return false;
  }
};

using ordered_parent = renderable_parent<renderable, true>;
using unordered_parent = renderable_parent<renderable, false>;

inline void inner_variadic_render(matrix_3f const& parent_trans) {
}

template<typename T, typename... Args>
inline void inner_variadic_render(matrix_3f const& parent_trans, T& first, Args&... remaining) {
  first.render(parent_trans);
  inner_variadic_render(parent_trans, remaining...);
}

template<typename P, typename... Args>
inline void variadic_local_render(matrix_3f const& parent_trans, P const& parent, Args&... remaining) {
  if (parent.visible) {
    matrix_3f trans = parent_trans * parent.local_trans;
    inner_variadic_render(trans, remaining...);
  }
}

template<typename P, typename... Args>
inline void variadic_trans_render(matrix_3f const& parent_trans, P const& parent, Args&... remaining) {
  if (parent.visible) {
    matrix_3f trans = parent_trans * parent.trans.to_matrix();
    inner_variadic_render(trans, remaining...);
  }
}

