#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <array>
#include "node.h"
#include "node_iterator.h"
#include "edition_reference.h"
#include "pool.h"

namespace PoincareJ {

class PatternMatching {
public:
  enum class Placeholder : uint8_t {
    A,
    B,
    C,
  };
  static constexpr int k_numberOfPlaceholders = 3;


  class Context {
  public:
    Node& operator[](Placeholder placeholder) {
      return m_array[static_cast<uint8_t>(placeholder)];
    }
    const Node& operator[](Placeholder placeholder) const {
      return m_array[static_cast<uint8_t>(placeholder)];
    }

    bool isUninitialized() const;
  private:
    Node m_array[k_numberOfPlaceholders];
  };

  static Context Match(const Node pattern, Node source, Context context = Context());
  static EditionReference Create(const Node structure, const Context context);
};

}

#endif
