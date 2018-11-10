#pragma once

#include <memory> 
#include <type_traits>
#include <algorithm>
#include <cassert>
#include <bitset>
#include <type_traits>

#include "fast_bitset.h"

#define USE_FAST_BITSET

// sparse_container is a fixed capacity container which maintains the validity of references and iterators for the lifetime of the the object.
// This allows for some cool tricks like iterating through a sparse_container while inserting or deleting values before or after the current iterator.
// It also allows other structures to hold references to elements of a sparse_container safely so long as that the object outlives its reference.
// Unlike std::vector sparse_container allocates its buffers as part of its memory so its buffer can stack allocated or declared in static memory.
// The end and cend iterators remain valid regardless of any or all insertions and deletions.
template<typename T, size_t N>
class sparse_container {
private:
  // used so that complex types can avoid being default initialized
  alignas(T) char _untyped_data[sizeof(T) * N];

  size_t _size;
#ifdef USE_FAST_BITSET
  fast_bitset<N> _occupied;
#else
  std::bitset<N> _occupied;
#endif

  T const* const _typed_data() const {
    return reinterpret_cast<T const* const>(_untyped_data);
  }

  T* const _typed_data() {
    return reinterpret_cast<T* const>(_untyped_data);
  }

  using raw_iterator = T* ;
  using raw_const_iterator = T const*;

  raw_const_iterator craw_begin() const {
    return _typed_data();
  }

  raw_const_iterator craw_end() const {
    return _typed_data() + capacity();
  }

  raw_iterator raw_begin() {
    return _typed_data();
  }

  raw_iterator raw_end() {
    return _typed_data() + capacity();
  }

public:

  using value_type = T;
  using reference = T & ;
  using const_reference = T const&;
  using pointer = T * ;
  using const_pointer = T const*;
  using size_type = size_t;

  template<bool C>
  class base_iterator {
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = std::conditional_t<C, T const, T>;
    using reference = std::conditional_t<C, T const&, T&>;
    using pointer = std::conditional_t<C, T const*, T>;
    using iterator_category = std::input_iterator_tag;
    using container_pointer = std::conditional_t<C, sparse_container<T,N> const*, sparse_container<T, N>*>;

    template<bool B>
    base_iterator(base_iterator<B> other) : _con(other.container()), _idx(other.index()) {}

    base_iterator& operator++() { // pre-increment
      assert(_idx != _con->capacity());

      for (_idx++; _idx < _con->capacity(); _idx++) {
        if (_con->_occupied[_idx]) {
          break;
        }
      }

      return *this;
    }

    base_iterator operator++(int) { // post-increment
      iterator other = *this;
      operator++();
      return other;
    }

    base_iterator operator+(difference_type amount) const {
      iterator other = *this;
      other += amount;
      return other;
    }

    base_iterator operator+=(difference_type amount) {
      for (difference_type d = 0; d < amount; d++) {
        operator++();
      }
      return *this;
    }

    reference operator*() const {
      return _con->_typed_data()[_idx];
    }

    pointer operator->() const {
      return _con->_typed_data() + _idx;
    }

    bool operator==(base_iterator<C> const& other) const {
      return (_con == other._con) && (_idx == other._idx);
    }

    bool operator!=(base_iterator<C> const& other) const {
      return !(operator==(other));
    }

    size_t index() const {
      return _idx;
    }

    container_pointer container() const {
      return _con;
    }

  private:
    container_pointer _con;
    size_t _idx;

    friend class sparse_container<T, N>;
    base_iterator(container_pointer con, size_t i) : _con(con), _idx(i) {}
  };

  using iterator = base_iterator<false>;
  using const_iterator = base_iterator<true>;


  sparse_container() {
    _size = 0;
  }

  // copy constructor  O(N)
  sparse_container(sparse_container<T, N> const& other) {
    _size = other._size;
    _occupied = other._occupied;

    if (_size > 0) {
      for (size_t i = 0; i < capacity(); i++) {
        if (_occupied[i]) {
          _typed_data()[i] = other._typed_data()[i];
        }
      }
    }
  }

  // move constructor  O(N)
  sparse_container(sparse_container<T, N>&& other) {
    _size = other._size;
    _occupied = other._occupied;

    for (size_t i = 0; i < capacity(); i++) {
      if (_occupied[i]) {
        _typed_data()[i] = std::move(other._typed_data()[i]);
      }
    }

    // leave other empty after having been moved from
    other._occupied.reset();
    other._size = 0;
  }

  // copy assignment operator O(N)
  sparse_container<T, N>& operator= (sparse_container<T, N> const& other) {
    if (this != (&other)) {
      size = other._size;
      _occupied = other._occupied;

      for (size_t i = 0; i < capacity(); i++) {
        if (_occupied[i]) {
          _typed_data()[i] = other._typed_data()[i];
        }
      }
    }
    return *this;
  }

  // move assignment operator O(N)
  sparse_container<T, N>& operator=(sparse_container<T, N>&& other) {
    if (this != (&other)) {
      size = other._size;

      for (size_t i = 0; i < capacity(); i++) {
        if (_occupied[i]) {
          _typed_data()[i] = std::move(other._typed_data()[i]);
        }
      }

      // leave other empty after having been moved from
      other._occupied.reset();
      other._size = 0;
    }
    return *this;
  }

  ~sparse_container() {
    clear();
  }

  // O(N)
  iterator begin() {
    size_t _idx;
#ifdef USE_FAST_BITSET
    _idx = _occupied.first_true();
#else
    for (_idx = 0; _idx < capacity(); _idx++) {
      if (_occupied[_idx]) {
        break;
      }
    }
#endif
    return iterator{ this, _idx };
  }

  // O(1)
  iterator end() {
    return iterator{ this, capacity() };
  }

  // O(N)
  const_iterator cbegin() const {
    size_t _idx;
#ifdef USE_FAST_BITSET
    _idx = _occupied.first_true();
#else
    for (_idx = 0; _idx < capacity(); _idx++) {
      if (_occupied[_idx]) {
        break;
      }
    }
#endif
    return const_iterator{ this, _idx };
  }

  // O(1)
  const_iterator cend() const {
    return const_iterator{ this, capacity() };
  }

  // O(N)
  void push(T const& value) {
    assert(_size < capacity());
#ifdef USE_FAST_BITSET
    size_t _idx = _occupied.first_false();
    new (_typed_data() + _idx) T(value);
    _occupied.set(_idx);
#else
    for (size_t _idx = 0; _idx < capacity(); _idx++) {
      if (!_occupied[_idx]) {
        new (_typed_data() + _idx) T(value);
        _occupied.set(_idx);
        break;
      }
    }
#endif 
    _size++;
  }

  // O(N)
  void push(T&& value) {
    assert(_size < capacity());

#ifdef USE_FAST_BITSET
    size_t _idx = _occupied.first_false();
    new (_typed_data() + _idx) T(std::move(value));
    _occupied.set(_idx);
#else
    for (size_t _idx = 0; _idx < capacity(); _idx++) {
      if (!_occupied[_idx]) {
        new (_typed_data() + _idx) T(std::move(value));
        _occupied.set(_idx);
        break;
      }
    }
#endif
    _size++;
  }

  // O(N)
  void clear() noexcept {
    for (reference r : *this) {
      std::destroy_at(&r);
    }
    _occupied.reset();
    _size = 0;
  }

  size_t capacity() const {
    return N;
  }

  size_t size() const {
    return _size;
  }

  void erase(const_iterator pos) {
    assert(pos.index() < capacity());
    assert(_occupied[pos.index()]);
    std::destroy_at(&(*pos));
    _occupied[pos.index()] = false;
    _size--;
  }

  void erase(const_iterator first, const_iterator last) {
    assert(first.index() <= last.index());
    assert(last.index() <= capacity());

    size_t num_deleted = 0;
    const_iterator pos = first;
    while (pos != last) {
      assert(_occupied[pos.index()]);
      std::destroy_at(&(*pos));
      _occupied[pos.index()] = false;
      num_deleted++;
      ++pos;
    }
    _size -= num_deleted;
  }

};