#pragma once

#include <memory>
#include <array>
#include <optional>
#include <type_traits>
#include <algorithm>
#include <cassert>

template<typename T, size_t N>
class sized_vector final {
private:
  // used so that complex types can avoid being default initialized
  char _untyped_data[sizeof(T) * N];

  size_t _size;

  T const* const _typed_data() const {
    return reinterpret_cast<T const* const>(_untyped_data);
  }

  T* const _typed_data() {
    return reinterpret_cast<T* const>(_untyped_data);
  }

public:
  using value_type = T;
  using iterator = T*;
  using const_iterator = T const*;
  using reference = T&;
  using const_reference = T const&;
  using pointer = T*;
  using const_pointer = T const*;
  using size_type = size_t;

  sized_vector() {
    _size = 0;
  }

  // copy constructor
  sized_vector(sized_vector<T, N> const& other) {
    _size = other._size;
    std::copy(other.cbegin(), other.cend(), begin());
  }

  // move constructor
  sized_vector(sized_vector<T, N>&& other) {
    _size = other._size;
    std::move(other.cbegin(), other.cend(), begin());
  }

  // copy assignment operator
  sized_vector<T, N>& operator= (sized_vector<T, N> const& other) {
    if (this != (&other)) {
      size = other._size;
      std::copy(other.cbegin(), other.cend(), begin());
    }
    return *this;
  }

  // move assignment operator
  sized_vector<T, N>& operator= (sized_vector<T, N>&& other) {
    if (this != (&other)) {
      size = other._size;
      std::move(other.cbegin(), other.cend(), begin());
    }
    return *this;
  }

  ~sized_vector() {
    clear();
  }

  iterator erase(const_iterator pos) {
    assert(pos < cback());
    std::destroy_at(pos);
    if (pos == (end() - 1)) {
      _size--;
      return end();
    }
    iterator ipos = const_cast<iterator>(pos);
    std::move(ipos + 1, end(), ipos);
    _size--;
    return ipos;
  }

  iterator erase(const_iterator first, const_iterator last) {
    assert(first <= last);
    assert(last < cback());
    std::destroy(first, last);

    if (last == end()) {
      _size -= std::distance(first, last);
      return end();
    }

    iterator ipos = const_cast<iterator>(first);
    std::move(const_cast<iterator>(last), end(), ipos);
    _size -= std::distance(first, last);
    return ipos;
  }

  void pop_back() {
    assert(_size > 0);
    _size--;
    std::destroy_at(_typed_data() + _size);
  }

  void clear() noexcept {
    std::destroy_n(begin(), _size);
    _size = 0;
  }

  bool empty() const noexcept {
    return (_size == 0);
  }

  void resize(size_type count) {
    assert(count < capacity());
    if (count > _size) {
      std::uninitialized_default_construct(_typed_data() + _size, _typed_data() + count);
    } else if (count < _size) {
      std::destroy(_typed_data() + count, _typed_data() + _size);
    }
    _size = count;
  }

  void resize(size_type count, T const& value) {
    assert(count < capacity());
    if (count > _size) {
      std::uninitialized_fill(_typed_data() + _size, _typed_data() + count, value);
    } else if (count < _size) {
      std::destroy(_typed_data() + count, _typed_data() + _size);
    }
    _size = count;
  }

  void swap(sized_vector<T, N>& other) noexcept {
    std::swap_ranges(begin(), end(), other.begin());
    if (other._size > _size) {
      std::move(other.begin() + _size, other.end(), end());
    }
    std::swap(_size, other._size);
  }

  pointer data() noexcept {
    return _typed_data();
  }

  const_pointer data() const noexcept {
    return _typed_data();
  }

  reference operator[](size_type pos) {
    assert(pos < _size);
    return _typed_data()[pos];
  }

  const_reference operator[](size_type pos) const {
    assert(pos < _size);
    return _typed_data()[pos];
  }

  reference front() {
    assert(_size > 0);
    return _typed_data()[0];
  }

  const_reference front() const {
    assert(_size > 0);
    return _typed_data()[0];
  }

  reference back() {
    assert(_size > 0);
    return _typed_data()[_size - 1];
  }

  const_reference back() const {
    assert(_size > 0);
    return _typed_data()[_size - 1];
  }

  size_t size() const {
    return _size;
  }

  size_t capacity() const {
    return N;
  }

  template<typename... Args>
  reference emplace_back(Args&&... args) {
    assert(_size < capacity());
    T* value = new (_typed_data() + _size)T(std::forward<Args>(args)...);
    _size++;
    return *value;
  }

  void push_back(T const& value) {
    assert(_size < capacity());
    _typed_data()[_size] = value;
    _size++;
  }

  void push_back(T&& value) {
    assert(_size < capacity());
    _typed_data()[_size] = value;
    _size++;
  }

  const_iterator cbegin() const {
    return _typed_data();
  }

  const_iterator cend() const {
    return _typed_data() + _size;
  }

  iterator begin() {
    return _typed_data();
  }

  iterator end() {
    return _typed_data() + _size;
  }
};