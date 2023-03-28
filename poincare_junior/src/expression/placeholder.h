#ifndef POINCARE_EXPRESSION_PLACEHOLDER_H
#define POINCARE_EXPRESSION_PLACEHOLDER_H

#include <omg/bit_helper.h>
#include <poincare_junior/src/memory/node.h>
#include <poincare_junior/src/memory/value_block.h>

namespace PoincareJ {

/* TODO : This class could use an union to hide bit manipulation */
class Placeholder {
 public:
  enum Tag : uint8_t { A = 0, B, C, numberOfTags };
  enum Filter : uint8_t {
    // Placeholder match any node, which replaces the placeholder on creation
    None = 0,
    // When matching, Placeholder must be on of these two nAry types
    Addition,
    Multiplication,
    // When creating, Placeholder is first child of matched nAry
    First,
    // When creating, Placeholder is other children of matched nAry
    Others,
    numberOfFilters
  };

  consteval static uint8_t ParamsToValue(Tag tag, Filter filter) {
    return (tag << k_bitsForFilter) | filter;
  }
  constexpr static Tag NodeToTag(const Node n) {
    return ValueToTag(NodeToValue(n));
  }
  constexpr static Filter NodeToFilter(const Node n) {
    return ValueToFilter(NodeToValue(n));
  }

 private:
  constexpr static size_t k_bitsForTag =
      OMG::BitHelper::numberOfBitsToCountUpTo(Tag::numberOfTags);
  constexpr static size_t k_bitsForFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(Filter::numberOfFilters);
  // Tags and filters can be added as long as it fits in a ValueBlock.
  static_assert(k_bitsForTag + k_bitsForFilter <=
                OMG::BitHelper::numberOfBitsIn<ValueBlock>());
  constexpr static uint8_t k_tagMask = (1 << k_bitsForTag) - 1;
  constexpr static uint8_t k_filterMask = (1 << k_bitsForFilter) - 1;

  constexpr static uint8_t NodeToValue(const Node n) {
    return static_cast<uint8_t>(*(n.block()->next()));
  }
  constexpr static Tag ValueToTag(uint8_t value) {
    return static_cast<Tag>((value >> k_bitsForFilter) & k_tagMask);
  }
  constexpr static Filter ValueToFilter(uint8_t value) {
    return static_cast<Filter>(value & k_filterMask);
  }
};

}  // namespace PoincareJ

#endif
