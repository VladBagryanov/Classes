#ifndef VECTOR_H
#define VECTOR_H
#include <cstddef>
#include <stdexcept>

template <class T>
class Vector {
  T* value_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  void Extension(const size_t& new_capacity) {
    auto new_vec = value_;
    try {
      value_ = new T[new_capacity];
      for (size_t i = 0; i < size_; ++i) {
        value_[i] = std::move(new_vec[i]);
      }
      capacity_ = new_capacity;
      delete[] new_vec;
    } catch (...) {
      value_ = new_vec;
      throw;
    }
  }

 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using SizeType = size_t;
  using Iterator = T*;
  using ConstIterator = const T*;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
  ~Vector() {
    delete[] value_;
  }
  Vector(const std::initializer_list<T> list) : Vector(list.begin(), list.end()) {
  }
  Vector() = default;
  template <class Iterator, class = std::enable_if_t<std::is_base_of_v<
                                std::forward_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>>>
  Vector(Iterator first, Iterator last) {
    if (first == last) {
      value_ = nullptr;
      capacity_ = 0;
      size_ = 0;
      return;
    }
    auto tmp = first;
    size_ = std::distance(first, last);
    try {
      capacity_ = size_;
      value_ = new T[capacity_];
      for (size_t i = 0; first < last; ++first, ++i) {
        value_[i] = *first;
      }
    } catch (...) {
      delete[] value_;
      value_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      throw;
    }
  }
  explicit Vector(size_t count) : size_(count), capacity_(count) {
    if (count == 0) {
      return;
    }
    try {
      value_ = new T[count];
    } catch (...) {
      delete[] value_;
      value_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      throw;
    }
  }
  Vector(const size_t& count, const T& value) : size_(count), capacity_(count) {
    if (count == 0) {
      size_ = 0;
      capacity_ = 0;
      value_ = nullptr;
      return;
    }
    size_t i = 0;
    try {
      value_ = new T[size_];
      for (; i < size_; ++i) {
        value_[i] = value;
      }
    } catch (...) {
      if (i != 0) {
        delete[] value_;
      }
      value_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      throw;
    }
  }
  Vector(const Vector& other) : size_(other.size_), capacity_(other.capacity_) {
    if (other.value_ == nullptr) {
      value_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      return;
    }
    try {
      value_ = new T[other.Capacity()];
      for (size_t i = 0; i < size_; ++i) {
        value_[i] = other[i];
      }
    } catch (...) {
      delete[] value_;
      value_ = nullptr;
      size_ = 0;
      capacity_ = 0;
      throw;
    }
  }

  Vector(Vector&& other) noexcept
      : value_(std::exchange(other.value_, nullptr))
      , size_(std::exchange(other.size_, 0))
      , capacity_(std::exchange(other.capacity_, 0)) {
  }

  Vector& operator=(const Vector& other) {
    if (this == &other) {
      return *this;
    }
    auto value = std::exchange(value_, nullptr);
    try {
      Vector(other).Swap(*this);
      delete[] value;
    } catch (...) {
      value_ = value;
      throw;
    }
    return *this;
  }
  Vector& operator=(Vector&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    Vector(std::move(other)).Swap(*this);
    return *this;
  }
  void Resize(size_t new_size) {
    try {
      if (value_ == nullptr) {
        *this = Vector(new_size);
      }
      if (capacity_ < new_size) {
        Extension(new_size);
      }
      size_ = new_size;
    } catch (...) {
      throw;
    }
  }
  void Resize(size_t new_size, const T& value) {
    if (new_size <= size_) {
      size_ = new_size;
      return;
    }
    if (new_size <= capacity_) {
      for (size_t i = size_; i < new_size; ++i) {
        value_[i] = value;
      }
      size_ = new_size;
    }
    if (capacity_ < new_size) {
      auto new_vec = value_;
      try {
        value_ = new T[new_size];
        for (size_t i = 0; i < size_; ++i) {
          value_[i] = std::move(new_vec[i]);
        }
        for (size_t i = size_; i < new_size; ++i) {
          value_[i] = value;
        }
        size_ = new_size;
        capacity_ = new_size;
        delete[] new_vec;
      } catch (...) {
        delete[] value_;
        value_ = new_vec;
        throw;
      }
    }
  }
  void PushBack(const T& symbol) {
    auto back_up = capacity_;
    auto new_vec = value_;
    try {
      if (size_ == capacity_) {
        auto new_capacity = std::max<size_t>(1, 2 * capacity_);
        value_ = new T[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
          value_[i] = std::move(new_vec[i]);
        }
        capacity_ = new_capacity;
      }
      value_[size_++] = symbol;
      if (new_vec != value_) {
        delete[] new_vec;
      }
    } catch (...) {
      if (new_vec != value_) {
        delete[] value_;
      }
      value_ = new_vec;
      capacity_ = back_up;
      size_ = capacity_;
      throw;
    }
  }
  void PushBack(T&& symbol) {
    try {
      if (size_ == capacity_) {
        Extension(std::max<size_t>(2 * capacity_, 1));
      }
      value_[size_++] = std::move(symbol);
    } catch (...) {
      throw;
    }
  }
  void Swap(Vector& other) {
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    std::swap(value_, other.value_);
  }
  void Reserve(const size_t& new_capacity) {
    if (new_capacity > capacity_) {
      Extension(new_capacity);
    }
  }
  void Clear() {
    size_ = 0;
  }

  SizeType Capacity() const {
    return capacity_;
  }

  Reference operator[](size_t idx) noexcept {
    return value_[idx];
  }

  ConstReference operator[](size_t idx) const noexcept {
    return value_[idx];
  }

  Reference At(const size_t& idx) {
    if (idx < size_) {
      return value_[idx];
    }
    throw std::out_of_range("");
  }

  ConstReference At(const size_t& idx) const {
    if (idx < size_) {
      return value_[idx];
    }
    throw std::out_of_range("");
  }

  Reference Front() {
    return value_[0];
  }

  ConstReference Front() const {
    return value_[0];
  }

  Pointer Data() {
    return value_;
  }

  ConstPointer Data() const {
    return value_;
  }

  Reference Back() {
    return value_[size_ - 1];
  }

  ConstReference Back() const {
    return value_[size_ - 1];
  }

  SizeType Size() const {
    return size_;
  }

  bool Empty() const {
    return size_ == 0;
  }

  void PopBack() {
    size_--;
  }

  void ShrinkToFit() {
    if (size_ == 0) {
      capacity_ = 0;
      delete[] value_;
      value_ = nullptr;
      return;
    }
    Extension(size_);
  }

  Pointer begin() {  // NOLINT
    return value_;
  }
  ConstPointer begin() const {  // NOLINT
    return value_;
  }
  Pointer end() {  // NOLINT
    return value_ + size_;
  }
  ConstPointer end() const {  // NOLINT
    return value_ + size_;
  }
  ConstPointer cbegin() const {  // NOLINT
    return value_;
  }
  ConstPointer cend() const {  // NOLINT
    return value_ + size_;
  }
  ReverseIterator rbegin() {  // NOLINT
    return std::make_reverse_iterator(value_ + size_);
  }
  ConstReverseIterator rbegin() const {  // NOLINT
    return std::make_reverse_iterator(value_ + size_);
  }
  ConstReverseIterator crbegin() const {  // NOLINT
    return std::make_reverse_iterator(value_ + size_);
  }
  ReverseIterator rend() {  // NOLINT
    return std::make_reverse_iterator(value_);
  }
  ConstReverseIterator rend() const {  // NOLINT
    return std::make_reverse_iterator(value_);
  }
  ConstReverseIterator crend() const {  // NOLINT
    return std::make_reverse_iterator(value_);
  }
};
template <class T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vector) {
  for (size_t i = 0; i < vector.Size(); ++i) {
    os << vector[i];
  }
  return os;
}
template <typename T>
bool operator==(const Vector<T>& first, const Vector<T>& other) {
  if (first.Size() == other.Size()) {
    for (size_t i = 0; i < first.Size(); ++i) {
      if (first[i] != other[i]) {
        return false;
      }
    }
  }
  return first.Size() == other.Size();
}
template <typename T>
bool operator!=(const Vector<T>& first, const Vector<T>& other) {
  return !(first == other);
}
template <typename T>
bool operator<(const Vector<T>& first, const Vector<T>& other) {
  return other > first;
}
template <typename T>
bool operator>(const Vector<T>& first, const Vector<T>& other) {
  for (size_t i = 0; i < std::min<size_t>(first.Size(), other.Size()); ++i) {
    if (first[i] > other[i]) {
      return true;
    }
    if (first[i] < other[i]) {
      return false;
    }
  }
  return first.Size() > other.Size();
}
template <typename T>
bool operator<=(const Vector<T>& first, const Vector<T>& other) {
  return (first == other || first < other);
}
template <typename T>
bool operator>=(const Vector<T>& first, const Vector<T>& other) {
  return (first == other || first > other);
}
#endif