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
      return data;
    } else {
      return others.template unsafe_get<A>();
    }
  }

  /* NOTE: [req_type] is used in ASSERTIONS to check if the fetched type is
   * indeed the currently stored type */
  template <typename A>
  A& get(uint8_t req_type) {
    if constexpr (std::is_same_v<A, T>) {
      assert(req_type == 0);
      return data;
    } else {
      --req_type;
      return others.template get<A>(req_type);
    }
  }

  template <typename A, typename... Args>
  uint8_t init(Args... args) {
    if constexpr (std::is_same_v<A, T>) {
      new (&data) A(args...);
      return 0;
    } else {
      return 1 + others.template init<A>(args...);
    }
  }

  void deinit(uint8_t type) {
    if (type == 0) {
      data.~T();
    } else {
      return others.deinit(--type);
    }
  }

 private:
  union {
    T data;
    VariantInternalStorage<Ts...> others;
  };
};

}  // namespace Internal

template <typename... Arg>
class Variant {
 public:
  Variant() : type(k_notInit){};
  ~Variant() {
    assert(type != k_notInit);
    variants.deinit(type);
  }

  // Delete all other constructors
  Variant(const Variant&) = delete;
  Variant& operator=(const Variant&) = delete;
  Variant(Variant&&) = delete;
  Variant& operator=(Variant&&) = delete;

  void deinit() {
    if (type != k_notInit) {
      variants.deinit(type);
    }
    type = k_notInit;
  }

  /* Same as [get<A>] but without asserting that the fetched type is the
   * stored typed.
   * This is useful when simultaneously needing pointers to 2 distinct members
   * (such as in a parent constructor) */
  template <typename A>
  A& unsafe_get() {
    return variants.template unsafe_get<A>();
  };

  /* Returns the fetched type.
   * In ASSERTIONS, checks if the fetched type is indeed the stored type */
  template <typename A>
  A& get() {
    return variants.template get<A>(type);
  };

  template <typename A, typename... Args>
  void init(Args... args) {
    assert(type == k_notInit);
    type = variants.template init<A>(args...);
  }

 private:
  static constexpr uint8_t k_notInit = -1;
  Internal::VariantInternalStorage<Arg...> variants;
  uint8_t type = k_notInit;
};

template <typename One, typename Two>
class Variant2 {
 public:
  Variant2() : type(k_notInit){};
  ~Variant2() { deinit(); }

  // Delete all other constructors
  Variant2(const Variant2&) = delete;
  Variant2& operator=(const Variant2&) = delete;
  Variant2(Variant2&&) = delete;
  Variant2& operator=(Variant2&&) = delete;

  template <typename A>
  bool has() const {
    return (std::is_same_v<A, One> && type == 0) ||
           (std::is_same_v<A, Two> && type == 1);
  }

  template <typename A>
  A& get() {
    if constexpr (std::is_same_v<A, One>) {
      assert(type == 0);
      return one;
    } else if constexpr (std::is_same_v<A, Two>) {
      assert(type == 1);
      return two;
    } else {
      OMG::unreachable();
    }
  }

  template <typename A>
  A& unsafe_get() {
    if constexpr (std::is_same_v<A, One>) {
      return one;
    } else if constexpr (std::is_same_v<A, Two>) {
      return two;
    } else {
      OMG::unreachable();
    }
  }

  template <typename A, typename... Args>
  A& init(Args... args) {
    deinit();
    if constexpr (std::is_same_v<A, One>) {
      type = 0;
      new (&one) A(args...);
      return one;
    } else if constexpr (std::is_same_v<A, Two>) {
      new (&two) A(args...);
      type = 1;
      return two;
    } else {
      OMG::unreachable();
    }
  }

  void deinit() {
    if (type == 0) {
      one.~One();
    } else if (type == 1) {
      two.~Two();
    } else {
      assert(type == k_notInit);
    }
    type = k_notInit;
  }

  static constexpr uint8_t k_notInit = -1;
  union {
    One one;
    Two two;
  };
  uint8_t type = k_notInit;
  static_assert(!std::is_same_v<One, Two>);
};

}  // namespace OMG
