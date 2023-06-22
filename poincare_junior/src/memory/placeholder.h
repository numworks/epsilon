#ifndef POINCARE_EXPRESSION_PLACEHOLDER_H
#define POINCARE_EXPRESSION_PLACEHOLDER_H

#include <omg/bit_helper.h>
#include <omgpj/bit.h>
#include <poincare_junior/src/memory/node.h>
#include <poincare_junior/src/memory/value_block.h>

namespace PoincareJ {

/* TODO: This class could use an union to hide bit manipulation */
class Placeholder {
 public:
  // Using plain enum for tag to simplify PatternMatching Context usage.
  enum Tag : uint8_t { A = 0, B, C, D, E, F, G, NumberOfTags };

  enum class Filter : uint8_t {
    // Match any tree
    None = 0,
    // Match 0 or any number of consecutive trees
    AnyTrees,
    NumberOfFilters
  };

  consteval static uint8_t ParamsToValue(Tag tag, Filter filter) {
    return ParamsToValue(tag, static_cast<uint8_t>(filter));
  }
  constexpr static Tag NodeToTag(const Node* placeholder) {
    return ValueToTag(NodeToValue(placeholder));
  }

  constexpr static Filter NodeToFilter(const Node* placeholder) {
    return ValueToFilter(NodeToValue(placeholder));
  }

 private:
  constexpr static size_t k_bitsForTag =
      OMG::BitHelper::numberOfBitsToCountUpTo(Tag::NumberOfTags);
  constexpr static size_t k_bitsForFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(Filter::NumberOfFilters));
  // Tags and filters can be added as long as it fits in a ValueBlock.
  static_assert(k_bitsForTag + k_bitsForFilter <=
                OMG::BitHelper::numberOfBitsIn<ValueBlock>());

  consteval static uint8_t ParamsToValue(Tag tag, uint8_t filter) {
    return tag | (filter << k_bitsForTag);
  }
  constexpr static uint8_t NodeToValue(const Node* placeholder) {
    assert(placeholder->type() == BlockType::Placeholder);
    return static_cast<uint8_t>(*(placeholder->block()->next()));
  }
  constexpr static Tag ValueToTag(uint8_t value) {
    return static_cast<Tag>(Bit::getBitRange(value, k_bitsForTag - 1, 0));
  }
  constexpr static Filter ValueToFilter(uint8_t value) {
    return static_cast<Filter>(Bit::getBitRange(
        value, k_bitsForTag + k_bitsForFilter - 1, k_bitsForTag));
  }
};

namespace Placeholders {
static constexpr Placeholder::Tag A = Placeholder::Tag::A;
static constexpr Placeholder::Tag B = Placeholder::Tag::B;
static constexpr Placeholder::Tag C = Placeholder::Tag::C;
static constexpr Placeholder::Tag D = Placeholder::Tag::D;
static constexpr Placeholder::Tag E = Placeholder::Tag::E;
static constexpr Placeholder::Tag F = Placeholder::Tag::F;
static constexpr Placeholder::Tag G = Placeholder::Tag::G;
}  // namespace Placeholders

}  // namespace PoincareJ

#endif
