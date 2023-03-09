#ifndef POINCARE_MEMORY_PATTERN_MATCHING_H
#define POINCARE_MEMORY_PATTERN_MATCHING_H

#include <array>
#include "node.h"
#include "node_iterator.h"
#include "k_creator.h"
#include "edition_reference.h"
#include "pool.h"

namespace PoincareJ {

namespace PatternMatching {
  enum class PlaceholderTag : uint8_t {
    A,
    B,
    C,
  };
  static constexpr int k_numberOfPlaceholders = 3;

  template <PlaceholderTag P> struct Placeholder : public AbstractTreeCompatible {
    template <Block...B> consteval operator Tree<B...> () const { return Tree<B...>(); }
    constexpr operator const Node () const { return Tree(Placeholder<P>()); }
  };

  namespace Placeholders {
  static constexpr Placeholder<PlaceholderTag::A> A;
  static constexpr Placeholder<PlaceholderTag::B> B;
  static constexpr Placeholder<PlaceholderTag::C> C;
  }

  class Context {
  public:
    Node& operator[](PlaceholderTag placeholder) {
      return m_array[static_cast<uint8_t>(placeholder)];
    }

    template <PlaceholderTag P> Node& operator[](Placeholder<P> placeholder) {
      return m_array[static_cast<uint8_t>(P)];
    }

    const Node& operator[](PlaceholderTag placeholder) const {
      return m_array[static_cast<uint8_t>(placeholder)];
    }

    template <PlaceholderTag P> const Node& operator[](Placeholder<P> placeholder) const {
      return m_array[static_cast<uint8_t>(P)];
    }

    bool isUninitialized() const;
  private:
    Node m_array[k_numberOfPlaceholders];
  };

  Context Match(const Node pattern, Node source, Context context = Context());
  EditionReference Create(const Node structure, const Context context = Context());
};

template <PatternMatching::PlaceholderTag P> Tree(PatternMatching::Placeholder<P>) -> Tree<BlockType::Placeholder, static_cast<uint8_t>(P), BlockType::Placeholder>;

}

#endif
