#pragma once

#include <assert.h>
#include <stdint.h>

#include <type_traits>

#include "unreachable.h"

namespace OMG {
namespace Internal {
// Forward declaration
template <typename... Ts>
class VariantInternalStorage;

// Base case: empty storage
template <>
class VariantInternalStorage<> {
 public:
  void deinit(uint8_t type) { OMG::unreachable(); }
};

// Recursive case: store the first type and the rest
template <typename T, typename... Ts>
class VariantInternalStorage<T, Ts...> {
 public:
  VariantInternalStorage() {}
  ~VariantInternalStorage() {}

  template <typename A>
  A& unsafe_get() {
    if constexpr (std::is_same_v<A, T>) {
      return m_data;
    } else {
      return m_others.template unsafe_get<A>();
    }
  }

  /* NOTE: [type] is used in ASSERTIONS to check if the fetched type is
   * indeed the currently stored type */
  template <typename A>
  A& get(uint8_t type) {
    if constexpr (std::is_same_v<A, T>) {
      assert(type == 0);
      return m_data;
    } else {
      --type;
      return m_others.template get<A>(type);
    }
  }

  template <typename A, typename... Args>
  uint8_t init(Args... args) {
    if constexpr (std::is_same_v<A, T>) {
      new (&m_data) A(args...);
      return 0;
    } else {
      return 1 + m_others.template init<A>(args...);
    }
  }

  void deinit(uint8_t type) {
    if (type == 0) {
      m_data.~T();
    } else {
      --type;
      return m_others.deinit(type);
    }
  }

 private:
  union {
    T m_data;
    VariantInternalStorage<Ts...> m_others;
  };
};

}  // namespace Internal

template <typename... Arg>
class Variant {
 public:
  Variant() = default;
  ~Variant() {
    assert(m_type != k_notInit);
    m_variants.deinit(m_type);
  }

  // Delete all other constructors
  Variant(const Variant&) = delete;
  Variant& operator=(const Variant&) = delete;
  Variant(Variant&&) = delete;
  Variant& operator=(Variant&&) = delete;

  void deinit() {
    if (m_type != k_notInit) {
      m_variants.deinit(m_type);
    }
    m_type = k_notInit;
  }

  /* Same as [get<A>] but without asserting that the fetched type is the
   * stored typed.
   * This is useful when simultaneously needing pointers to 2 distinct members
   * (such as in a parent constructor) */
  template <typename A>
  A& unsafe_get() {
    return m_variants.template unsafe_get<A>();
  };

  /* Returns the fetched type.
   * In ASSERTIONS, checks if the fetched type is indeed the stored type */
  template <typename A>
  A& get() {
    return m_variants.template get<A>(m_type);
  };

  template <typename A, typename... Args>
  void init(Args... args) {
    assert(m_type == k_notInit);
    m_type = m_variants.template init<A>(args...);
  }

 private:
  static constexpr uint8_t k_notInit = -1;
  Internal::VariantInternalStorage<Arg...> m_variants;
  uint8_t m_type = k_notInit;
};

template <typename One, typename Two>
class Variant2 {
 public:
  Variant2() = default;
  ~Variant2() { deinit(); }

  // Delete all other constructors
  Variant2(const Variant2&) = delete;
  Variant2& operator=(const Variant2&) = delete;
  Variant2(Variant2&&) = delete;
  Variant2& operator=(Variant2&&) = delete;

  template <typename A>
  bool has() const {
    return (std::is_same_v<A, One> && m_type == 0) ||
           (std::is_same_v<A, Two> && m_type == 1);
  }

  template <typename A>
  A& get() {
    if constexpr (std::is_same_v<A, One>) {
      assert(m_type == 0);
      return m_one;
    } else if constexpr (std::is_same_v<A, Two>) {
      assert(m_type == 1);
      return m_two;
    } else {
      OMG::unreachable();
    }
  }

  template <typename A>
  A& unsafe_get() {
    if constexpr (std::is_same_v<A, One>) {
      return m_one;
    } else if constexpr (std::is_same_v<A, Two>) {
      return m_two;
    } else {
      OMG::unreachable();
    }
  }

  template <typename A, typename... Args>
  A& init(Args... args) {
    deinit();
    if constexpr (std::is_same_v<A, One>) {
      m_type = 0;
      new (&m_one) A(args...);
      return m_one;
    } else if constexpr (std::is_same_v<A, Two>) {
      new (&m_two) A(args...);
      m_type = 1;
      return m_two;
    } else {
      OMG::unreachable();
    }
  }

  void deinit() {
    if (m_type == 0) {
      m_one.~One();
    } else if (m_type == 1) {
      m_two.~Two();
    } else {
      assert(m_type == k_notInit);
    }
    m_type = k_notInit;
  }

  static constexpr uint8_t k_notInit = -1;
  union {
    One m_one;
    Two m_two;
  };
  uint8_t m_type = k_notInit;
  static_assert(!std::is_same_v<One, Two>);
};

}  // namespace OMG
