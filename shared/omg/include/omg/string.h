#pragma once

#include <assert.h>

#include <initializer_list>
#include <string_view>

namespace OMG {

template <size_t CAPACITY>
class String {
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

  String() = default;

  String(const String& other) = default;

  String(const char_type* string, size_type stringLength) {
    assert(stringLength <= CAPACITY);
    size_type i = 0;
    while (i < stringLength) {
      (*this)[m_length++] = *(string + (i++));
    }
  }

  String(const char_type* string) {
    size_type i = 0;
    while (*(string + i) != '\0') {
      assert(i <= CAPACITY);
      (*this)[m_length++] = *(string + (i++));
    }
  }

  explicit String(std::string_view view) : String(view.data(), view.length()) {}

  String(std::initializer_list<char_type> characters) {
    assert(characters.size() <= CAPACITY);
    for (const char_type& element : characters) {
      (*this)[m_length++] = element;
    }
  }

  // Implicit conversion to an std::string_view
  operator std::string_view() const {
    return std::string_view(data(), length());
  }

  // Assignement

  String& operator=(const String& other) = default;

  String& operator=(std::string_view view) {
    *this = String(view);
    return *this;
  }

  // Methods

  const_reference operator[](size_type index) const {
    assert(index < m_length);
    return m_data[index];
  }
  reference operator[](size_type index) {
    assert(index < m_length);
    return m_data[index];
  }

  size_type length() const { return m_length; }
  const char_type* data() const { return m_data; }
  char_type* data() { return m_data; }

 private:
  size_type m_length = 0;
  value_type m_data[CAPACITY];
};

}  // namespace OMG
