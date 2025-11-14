#pragma once

#include <assert.h>

#include <initializer_list>
#include <string_view>

namespace OMG {

template <size_t CAPACITY>
class String {
  template <size_t OTHER_CAPACITY>
  friend class String;

 public:
  using char_type = char;
  using value_type = char_type;
  using size_type = size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = pointer;
  using const_iterator = const_pointer;

 public:
  // Constructors

  constexpr String() = default;

  constexpr String(const String& other) = default;

  constexpr String(const char_type* string, size_type stringLength) {
    assert(stringLength <= CAPACITY);
    size_type i = 0;
    while (i < stringLength) {
      (*this)[m_length++] = *(string + (i++));
    }
  }

  constexpr explicit String(const char_type* string) {
    size_type i = 0;
    while (*(string + i) != '\0') {
      assert(i <= CAPACITY);
      (*this)[m_length++] = *(string + (i++));
    }
  }

  constexpr explicit String(std::string_view view)
      : String(view.data(), view.length()) {}

  constexpr String(std::initializer_list<char_type> characters) {
    assert(characters.size() <= CAPACITY);
    for (const char_type& element : characters) {
      (*this)[m_length++] = element;
    }
  }

  // Implicit conversion to an std::string_view
  constexpr operator std::string_view() const {
    return std::string_view(data(), length());
  }

  // Assignement

  constexpr String& operator=(const String& other) = default;

  constexpr String& operator=(std::string_view view) {
    *this = String(view);
    return *this;
  }

  // Methods

  constexpr const_reference operator[](size_type index) const {
    assert(index < m_length);
    return m_data[index];
  }
  constexpr reference operator[](size_type index) {
    assert(index < m_length);
    return m_data[index];
  }

  constexpr size_type length() const { return m_length; }
  constexpr size_type capacity() const { return CAPACITY; }
  constexpr const char_type* data() const { return m_data; }
  constexpr char_type* data() { return m_data; }

  // Operators

  // (String + String) concatenation
  template <size_t R_CAPACITY>
  constexpr String<CAPACITY + R_CAPACITY> operator+(
      const String<R_CAPACITY>& right) {
    String<CAPACITY + R_CAPACITY> result(m_data, m_length);
    size_type i = 0;
    while (i < right.m_length) {
      result.m_data[result.m_length++] = right.m_data[i++];
    }
    return result;
  }

  // (String + char) concatenation
  constexpr String<CAPACITY>& operator+=(char right) {
    assert(m_length + 1 < CAPACITY);
    m_data[m_length++] = right;
    return *this;
  }

  // (String + list of char) concatenation
  constexpr String<CAPACITY>& operator+=(std::initializer_list<char> right) {
    assert(m_length + right.size() < CAPACITY);
    for (char c : right) {
      *this += c;
    }
    return *this;
  }

 private:
  size_type m_length = 0;
  value_type m_data[CAPACITY];
};

}  // namespace OMG
