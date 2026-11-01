/*
 * Circular Buffer Template
 *
 */
#pragma once

#include <array>
#include <cstdint>

namespace esphome {
namespace cat_feeder_logic {

/// Templated circular buffer
template <typename DataType, size_t MaxSize>
class CircularBuffer {
 public:
  /// Constructor
  CircularBuffer() {
    this->head_ = -1;
    this->capacity_ = 0;
  }

  /// Size
  int32_t Size() { return this->capacity_; }

  /// Push
  void Push(DataType obs) {
    this->head_ = (this->head_ + 1) % this->data_.max_size();
    this->data_[this->head_] = obs;
    if (this->capacity_ < static_cast<int32_t>(this->data_.max_size())) {
      this->capacity_++;
    }
  }

  /// Operator overload for read only access
  const DataType& operator[](int32_t age_index) const { return this->data_[this->GetArrayIndex(age_index)]; }

  /// Operator overload for access
  DataType& operator[](int32_t age_index) { return this->data_[this->GetArrayIndex(age_index)]; }

  /// Helper function for age index conversion
  int32_t GetArrayIndex(int32_t age_index) {
    return (static_cast<int32_t>(this->data_.max_size()) + this->head_ - (age_index % this->capacity_)) %
           static_cast<int32_t>(this->data_.max_size());
  }

  int32_t GetHead() { return this->head_; }

 private:
  int32_t head_;
  int32_t capacity_;
  std::array<DataType, MaxSize> data_;
};

}  // namespace cat_feeder_logic
}  // namespace esphome
