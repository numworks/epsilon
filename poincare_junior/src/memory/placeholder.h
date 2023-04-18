#ifndef POINCARE_EXPRESSION_PLACEHOLDER_H
#define POINCARE_EXPRESSION_PLACEHOLDER_H

#include <omg/bit_helper.h>
#include <omgpj/bit.h>
#include <poincare_junior/src/memory/node.h>
#include <poincare_junior/src/memory/value_block.h>

namespace PoincareJ {

/* Note : This class could use an union to hide bit manipulation */
class Placeholder {
 public:
  // Using plain enum for tag to simplify PatternMatching Context usage.
  enum Tag : uint8_t { A = 0, B, C, numberOfTags };

  enum class MatchFilter : uint8_t {
    // Match any node
    None = 0,
    // Match Addition nodes
    Addition,
    // Match Multiplication nodes
    Multiplication,
    numberOfFilters
  };

  enum class CreateFilter : uint8_t {
    // Replaces with matched node
    None = 0,
    // Replace with the first child of matched nAry
    FirstChild,
    // Replace with all but first children of matched nAry
    ExcludeFirstChild,
    numberOfFilters
  };

  consteval static uint8_t ParamsToValue(Tag tag, MatchFilter filter) {
    return ParamsToValue(tag, static_cast<uint8_t>(filter));
  }
  consteval static uint8_t ParamsToValue(Tag tag, CreateFilter filter) {
    return ParamsToValue(tag, static_cast<uint8_t>(filter));
  }
  constexpr static Tag NodeToTag(const Node n) {
    return ValueToTag(NodeToValue(n));
  }

  constexpr static uint8_t NodeToFilter(const Node n) {
    return ValueToFilter(NodeToValue(n));
  }
  constexpr static MatchFilter NodeToMatchFilter(const Node n) {
    return static_cast<MatchFilter>(NodeToFilter(n));
  }
  constexpr static CreateFilter NodeToCreateFilter(const Node n) {
    return static_cast<CreateFilter>(NodeToFilter(n));
  }

 private:
  constexpr static size_t k_bitsForTag =
      OMG::BitHelper::numberOfBitsToCountUpTo(Tag::numberOfTags);
  constexpr static size_t k_bitsForMatchFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(MatchFilter::numberOfFilters));
  constexpr static size_t k_bitsForCreateFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(CreateFilter::numberOfFilters));
  // Tags and filters can be added as long as it fits in a ValueBlock.
  static_assert(k_bitsForTag + k_bitsForMatchFilter <=
                OMG::BitHelper::numberOfBitsIn<ValueBlock>());
  static_assert(k_bitsForTag + k_bitsForCreateFilter <=
                OMG::BitHelper::numberOfBitsIn<ValueBlock>());
  // No filter is equivalent during Create and Match
  static_assert(static_cast<CreateFilter>(MatchFilter::None) ==
                CreateFilter::None);
  // Taking advantage of both filters taking the same space with k_bitsForFilter
  static_assert(k_bitsForMatchFilter == k_bitsForCreateFilter);
  constexpr static size_t k_bitsForFilter = k_bitsForMatchFilter;

  consteval static uint8_t ParamsToValue(Tag tag, uint8_t filter) {
    return tag | (filter << k_bitsForTag);
  }
  constexpr static uint8_t NodeToValue(const Node n) {
    return static_cast<uint8_t>(*(n.block()->next()));
  }
  constexpr static Tag ValueToTag(uint8_t value) {
    return static_cast<Tag>(Bit::getBitRange(value, k_bitsForTag - 1, 0));
  }
  constexpr static uint8_t ValueToFilter(uint8_t value) {
    return Bit::getBitRange(value, k_bitsForTag + k_bitsForFilter - 1,
                            k_bitsForTag);
  }
};

}  // namespace PoincareJ

#endif
