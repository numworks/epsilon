#pragma once

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <initializer_list>
#include <span>

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

  String(const char_type* string, size_type stringLength) {
    assert(stringLength <= CAPACITY);
    size_type i = 0;
    while (i < stringLength) {
      (*this)[m_size++] = *(string + (i++));
    }
  }

  String(const char_type* string) {
    size_type i = 0;
    while (*(string + i) != '\0') {
      assert(i <= CAPACITY);
      (*this)[m_size++] = *(string + (i++));
    }
  }

  String(std::initializer_list<char_type> characters) {
    assert(characters.size() <= CAPACITY);
    for (const char_type& element : characters) {
      (*this)[m_size++] = element;
    }
  }

  // Methods

  const_reference operator[](size_type index) const {
    assert(index < m_size);
    return m_data[index];
  }
  reference operator[](size_type index) {
    assert(index < m_size);
    return m_data[index];
  }

  size_type length() const { return m_size; }
  size_type size() const { return m_size; }
  const char_type* data() const { return m_data; }
  char_type* data() { return m_data; }

 private:
  size_type m_size = 0;
  value_type m_data[CAPACITY];
};

// constexpr version of strlen
constexpr static size_t StringLength(const char* string) {
  size_t result = 0;
  while (string[result] != 0) {
    result++;
  }
  return result;
}

// Helper methods for the std::span<const char> "string view"
// TODO: add std::string_view to the code base
constexpr static inline size_t StringLength(std::span<const char> string) {
  assert(string.back() == '\0');
  // The end character does not count in the string length
  return string.size() - 1;
}

constexpr static inline std::span<const char> ToSpan(const char* string) {
  return std::span<const char>{string, StringLength(string) + 1};
}

constexpr static inline std::span<const char> ToSpan(const char* string,
                                                     size_t stringLength) {
  assert(StringLength(string) == stringLength);
  return ToSpan(string);
}

/* FIXME : This can be replaced by std::string_view when moving to C++17 */
constexpr static bool StringsAreEqual(const char* s1, const char* s2) {
  return *s1 == *s2 &&
         ((*s1 == '\0' && *s2 == '\0') || StringsAreEqual(s1 + 1, s2 + 1));
}
constexpr static bool StringsAreEqual(std::span<const char> s1,
                                      std::span<const char> s2) {
  return StringsAreEqual(s1.data(), s2.data());
}

}  // namespace OMG
