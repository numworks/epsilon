#ifndef POINCARE_EXPRESSION_PLACEHOLDER_H
#define POINCARE_EXPRESSION_PLACEHOLDER_H

#include <omg/bit_helper.h>
#include <poincare_junior/src/memory/node.h>
#include <poincare_junior/src/memory/value_block.h>

namespace PoincareJ {

class Placeholder {
 public:
  enum Tag : uint8_t { A = 0, B, C, numberOfTags };

  consteval static uint8_t ParamsToValue(Tag tag) { return tag; }
  constexpr static Tag NodeToTag(const Node n) {
    return ValueToTag(NodeToValue(n));
  }

 private:
  constexpr static size_t k_bitsForTag =
      OMG::BitHelper::numberOfBitsToCountUpTo(Tag::numberOfTags);
  // Tags can be added as long as it fits in a ValueBlock.
  static_assert(k_bitsForTag <= OMG::BitHelper::numberOfBitsIn<ValueBlock>());
  constexpr static uint8_t k_tagMask = (1 << k_bitsForTag) - 1;

  constexpr static uint8_t NodeToValue(const Node n) {
    return static_cast<uint8_t>(*(n.block()->next()));
  }
  constexpr static Tag ValueToTag(uint8_t value) {
    return static_cast<Tag>(value & k_tagMask);
  }
};

}  // namespace PoincareJ

#endif
