#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <array>

namespace fast_bitset_utils {
  size_t constexpr npos() {
    return std::numeric_limits<size_t>::max();
  }

  uint32_t count_chunk_bits(uint32_t v) {
    // Bit Twiddling Hacks 
    // Counting bits set, in parallel
    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
  }

  uint32_t mask_most_significant_bit(uint32_t v) {
    // Bit Twiddling Hacks 
    // Find the log base 2 of an N-bit integer in O(lg(N)) operations with multiply and lookup 
    static int const multiply_de_bruijn_bit_position[32] =
    {
      0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
      31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
    };
    return multiply_de_bruijn_bit_position[(uint32_t)(v * 0x077CB531U) >> 27];
  }

  uint32_t chunk_most_significant_bit(uint32_t v) {
    // Bit Twiddling Hacks 
    // Find the log base 2 of an N-bit integer in O(lg(N)) operations with multiply and lookup 
    static int const multiply_de_bruijn_bit_position[32] =
    {
      0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
      8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
    };

    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return multiply_de_bruijn_bit_position[(uint32_t)(v * 0x07C4ACDDU) >> 27];
  }

  uint32_t chunk_least_significant_mask(uint32_t v) {
    return v & (~(v - 1));
  }

  uint32_t chunk_least_significant_bit(uint32_t v) {
    return mask_most_significant_bit(chunk_least_significant_mask(v));
  }

};

template<size_t N>
class fast_bitset {
private:

  size_t constexpr chunk_count() const {
    return (N + 31) / 32;
  }

  size_t constexpr full_chunk_count() const {
    return N / 32;
  }

  size_t constexpr last_chunk_size() const {
    return N - 32 * (N / 32);
  }

  uint32_t constexpr last_chunk_mask() const {
    return std::numeric_limits<uint32_t>::max() >> (32 - last_chunk_size());
  }

  std::array<uint32_t, (N + 31) / 32> _chunks;

  uint32_t& get_chunk(size_t pos) {
    assert(pos < size());
    return _chunks[pos / 32];
  }

  uint32_t const& get_chunk(size_t pos) const {
    assert(pos < size());
    return _chunks[pos / 32];
  }

  static void set_chunk_bit(uint32_t& chunk, size_t pos) {
    chunk |= (uint32_t{ 1 } << pos);
  }

  static void reset_chunk_bit(uint32_t& chunk, size_t pos) {
    chunk &= (~(uint32_t{ 1 } << pos));
  }

  static void set_chunk_bit(uint32_t& chunk, size_t pos, bool value) {
    // If this needs to be faster use:
    // Bit Twiddling Hacks 
    // Conditionally set or clear bits without branching 
    if (value) {
      set_chunk_bit(chunk, pos);
    } else {
      reset_chunk_bit(chunk, pos);
    }
  }

  static bool get_chunk_bit(uint32_t chunk, size_t pos) {
    return (chunk >> pos) & 1;
  }

  static void flip_chunk_bit(uint32_t& chunk, size_t pos) {
    chunk ^= (uint32_t{ 1 } << pos);
  }

public:

  class reference {
  private:
    fast_bitset & _con;
    size_t _idx;

    friend class fast_bitset<N>;

    reference(fast_bitset& c, size_t i) : _con(c), _idx(i) {}

  public:
    reference & operator=(bool x) noexcept {
      set_chunk_bit(_con.get_chunk(_idx), _idx % 32, x);
      return *this;
    }
    reference& operator=(reference const& x) noexcept {
      set_chunk_bit(_con.get_chunk(_idx), _idx % 32, x);
      return *this;
    }

    operator bool() const noexcept {
      return get_chunk_bit(_con.get_chunk(_idx), _idx % 32);
    }

    reference& flip() noexcept {
      flip_chunk_bit(_con.get_chunk(_idx), _idx % 32);
      return *this;
    }
  };

  size_t first_true(size_t start) const {
    assert(start < size());

    size_t chunk_idx = start / 32;
    size_t chunk_pos = start % 32;

    uint32_t chunk = _chunks[chunk_idx] >> chunk_pos;

    if (chunk != 0) {
      return fast_bitset_utils::chunk_least_significant_bit(chunk) + start;
    }

    for (chunk_idx++; chunk_idx < chunk_count(); chunk_idx++) {
      chunk = _chunks[chunk_idx];
      if (chunk == 0) {
        continue;
      }
      return fast_bitset_utils::chunk_least_significant_bit(chunk) + 32 * chunk_idx;
    }

    return fast_bitset_utils::npos();
  }

  size_t first_true() const {
    for (size_t chunk_idx = 0; chunk_idx < chunk_count(); chunk_idx++) {
      uint32_t chunk = _chunks[chunk_idx];
      if (chunk == 0) {
        continue;
      }
      return fast_bitset_utils::chunk_least_significant_bit(chunk) + 32 * chunk_idx;
    }

    return fast_bitset_utils::npos();
  }

  size_t first_false(size_t start) const {
    assert(start < size());

    size_t chunk_idx = start / 32;
    size_t chunk_pos = start % 32;

    uint32_t chunk = _chunks[chunk_idx] >> chunk_pos;

    // the last chunk may be incomplete
    if (chunk_idx == (chunk_count() - 1)) {
      uint32_t max_left = last_chunk_mask() >> chunk_pos;

      if (chunk != max_left) {
        return fast_bitset_utils::chunk_least_significant_bit(~chunk) + start;
      }

      return fast_bitset_utils::npos();
    } else {
      uint32_t max_left = std::numeric_limits<uint32_t>::max() >> chunk_pos;

      if (chunk != max_left) {
        return fast_bitset_utils::chunk_least_significant_bit(~chunk) + start;
      }
    }
    
    // check the full chunks
    for (chunk_idx++; chunk_idx < full_chunk_count(); chunk_idx++) {
      chunk = ~_chunks[chunk_idx];
      if (chunk == 0) {
        continue;
      }
      return fast_bitset_utils::chunk_least_significant_bit(chunk) + 32 * chunk_idx;
    }

    // check the last chunk if it has not been checked yet
    if (chunk_idx < chunk_count()) {
      uint32_t chunk = _chunks[chunk_count() - 1];
      if (chunk != last_chunk_mask()) {
        return fast_bitset_utils::chunk_least_significant_bit(~chunk) + 32 * (chunk_count() - 1);
      }
    }

    return fast_bitset_utils::npos();
  }

  size_t first_false() const {
    for (size_t chunk_idx = 0; chunk_idx < full_chunk_count(); chunk_idx++) {
      uint32_t chunk = ~_chunks[chunk_idx];
      if (chunk == 0) {
        continue;
      }
      return fast_bitset_utils::chunk_least_significant_bit(chunk) + 32 * chunk_idx;
    }

    if (full_chunk_count() < chunk_count()) {
      uint32_t chunk = _chunks[chunk_count() - 1];
      if (chunk != last_chunk_mask()) {
        return fast_bitset_utils::chunk_least_significant_bit(~chunk) + 32 * (chunk_count() - 1);
      }
    }

    return fast_bitset_utils::npos();
  }

  reference operator[](size_t pos) {
    assert(pos < size());
    return reference(*this, pos);
  }

  // O(N/32)
  size_t count() const {
    size_t total = 0;
    for (size_t i = 0; i < chunk_count(); i++) {
      total += fast_bitset_utils::count_chunk_bits(_chunks[i]);
    }
    return total;
  }

  constexpr size_t size() const noexcept {
    return N;
  }

  bool test(size_t pos) const {
    assert(pos < size());
    return get_chunk_bit(get_chunk(pos), pos);
  }

  bool operator[](size_t pos) const {
    return test(pos);
  }

  // O(N/32)
  fast_bitset<N>& set() {
    for (size_t i = 0; i < full_chunk_count(); i++) {
      _chunks[i] = std::numeric_limits<uint32_t>::max();
    }
    _chunks[chunk_count() - 1] = last_chunk_mask();
    return *this;
  }

  // O(1)
  fast_bitset<N>& set(size_t pos, bool value = true) {
    assert(pos < size());
    this->operator[](pos) = value;
    return *this;
  }

  // O(N/32)
  fast_bitset<N>& reset() {
    for (size_t i = 0; i < chunk_count(); i++) {
      _chunks[i] = 0;
    }
    return *this;
  }

  // O(1)
  fast_bitset<N>& reset(size_t pos) {
    assert(pos < size());
    this->operator[](pos) = false;
    return *this;
  }

  // O(N/32)
  fast_bitset<N>& flip() {
    for (size_t i = 0; i < full_chunk_count(); i++) {
      _chunks[i] = ~_chunks[i];
    }
    _chunks[chunk_count() - 1] ^= last_chunk_mask();
    return *this;
  }

  // O(1)
  fast_bitset<N>& flip(size_t pos) {
    assert(pos < size());
    this->operator[](pos).flip();
    return *this;
  }

  fast_bitset() {
    reset();
  }
};