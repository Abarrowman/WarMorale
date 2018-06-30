#pragma once

#include<array>
#include<algorithm>

template<typename T, size_t N>
class running_average {
private:
  size_t const capacity = N;
  std::array<T, N> values;
  size_t idx = 0;
  size_t count = 0;
  T pre_sum = 0;
public:

  // O(1)
  void insert(T val) {
    if (count < capacity) {
      count++;
    } else {
      pre_sum -= values[idx];
    }
    pre_sum += val;
    values[idx] = val;
    idx = idx + 1;
    if (idx >= capacity) {
      idx = 0;
    }
  }

  T latest() const {
    if (count == 0) {
      return 0;
    }
    if (idx == 0) {
      return values[capacity - 1];
    }
    return values[idx - 1];
  }

  // O(n) returns largest of last N
  T max() const {
    if (count == 0) {
      return 0;
    }
    auto it = std::max_element(values.begin(), values.begin() + count);
    return *it;
  }

  // O(1) returns average of last N
  T average() const {
    if (count == 0) {
      return 0;
    }
    return pre_sum / count;
  }
};

class frame_rate_meter {
private:
  running_average<float, 10> swap_times;
  typedef std::chrono::high_resolution_clock clock;
  clock::time_point last_swap;
public:
  frame_rate_meter() {
    last_swap = clock::now();
  }

  void count_frame() {
    clock::time_point current_swap = clock::now();
    std::chrono::duration<float, std::milli> swap_duration = current_swap - last_swap;
    last_swap = current_swap;
    float swap_time = swap_duration.count();
    swap_times.insert(swap_time);
  }

  float max_render_time() const {
    return swap_times.max();
  }

  float average_frame_rate() const {
    float avg = swap_times.average();
    if (avg == 0) {
      return 0;
    } else {
      return 1000 / swap_times.average();
    }
  }

  float average_frame_time() const {
    return swap_times.average();
  }

  float instantaneous_frame_rate() const {
    float latest = swap_times.latest();
    if (latest == 0) {
      return 0;
    } else {
      return 1000 / latest;
    }
  }

  float last_time() const {
    return swap_times.latest();
  }
};

class averaging_timer {
private:
  running_average<float, 10> times;
  typedef std::chrono::high_resolution_clock clock;
  clock::time_point begin_time;
public:
  void begin() {
    begin_time = clock::now();
  }
  void end() {
    clock::time_point end_time = clock::now();
    std::chrono::duration<float, std::milli> dur = end_time - begin_time;
    times.insert(dur.count());
  }

  float average() const {
    return times.average();
  }
};