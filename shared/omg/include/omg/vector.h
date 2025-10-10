#pragma once

#include <assert.h>
#include <stddef.h>

#include <span>

/* The StaticVector class is a simple static array that can hold up to a max
 * number of elements and tracks its current size.
 * It's partly inspired by boost's static_vector class:
 * https://beta.boost.org/doc/libs/1_58_0/doc/html/boost/container/static_vector.html
 */

/* TODO: The class StaticVector should be used in many places in the code.
 * TIP: Search for "m_numberOf" or "int m_" or "size_t m_" to find places where
 * it could apply. */

namespace OMG {

template <typename T>
class AbstractStaticVector {
 public:
  using iterator = T*;
  using const_iterator = const T*;

  constexpr size_t size() const { return m_size; }
  constexpr void clear() { m_size = 0; }
  constexpr void resize(size_t size) {
    assert(size <= m_size);
    m_size = size;
  }

  constexpr const T& operator[](size_t index) const {
    assert(index < m_size);
    return m_data[index];
  }
  constexpr T& operator[](size_t index) {
    assert(index < m_size);
    return m_data[index];
  }

  constexpr iterator begin() { return m_data; }
  constexpr const_iterator begin() const { return m_data; }
  constexpr iterator end() { return m_data + m_size; }
  constexpr const_iterator end() const { return m_data + m_size; }

  constexpr void push(const T& value) { (*this)[m_size++] = value; }
  constexpr T& pop() {
    assert(m_size > 0);
    T& result = (*this)[m_size - 1];
    /* This cannot be decreased before accessing the element or the assert in
     * the operator[] will fail */
    --m_size;
    return result;
  }
  constexpr void removeAt(size_t index) {
    assert(index < m_size);
    for (size_t i = index; i < m_size - 1; i++) {
      (*this)[i] = (*this)[i + 1];
    }
    --m_size;
  }

 protected:
  constexpr AbstractStaticVector() = default;

  size_t m_size = 0;
  T m_data[0];
};

template <typename T, size_t CAPACITY>
class StaticVector : public AbstractStaticVector<T> {
 public:
  static_assert(CAPACITY > 0, "StaticVector must have a positive capacity");

  constexpr StaticVector() = default;

  // Use this constructor only if T has a proper default constructor
  constexpr StaticVector(size_t initialSize) : AbstractStaticVector<T>() {
    assert(initialSize <= CAPACITY);
    this->m_size = initialSize;
  }

  constexpr StaticVector(std::initializer_list<T> values) {
    assert(values.size() <= CAPACITY);
    for (const T& value : values) {
      this->push(value);
    }
  }

  constexpr size_t capacity() const { return CAPACITY; }
  constexpr bool isFull() const { return this->m_size == CAPACITY; }
  constexpr std::span<T> span() {
    return std::span<T>(this->m_data, this->size());
  }

 protected:
  T m_data[CAPACITY];
};

}  // namespace OMG
