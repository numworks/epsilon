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
  enum Tag : uint8_t { A = 0, B, C, D, NumberOfTags };

  enum class MatchFilter : uint8_t {
    // Match any node
    None = 0,
    // Match Addition nodes
    Addition,
    // Match Multiplication nodes
    Multiplication,
    NumberOfFilters
  };

  enum class CreateFilter : uint8_t {
    // Replaces with matched node
    None = 0,
    // Replace with the first child of matched nAry
    FirstChild,
    // Replace with non-first children of matched nAry
    NonFirstChild,
    NumberOfFilters
  };

  consteval static uint8_t ParamsToValue(Tag tag, MatchFilter filter) {
    return ParamsToValue(tag, static_cast<uint8_t>(filter));
  }
  consteval static uint8_t ParamsToValue(Tag tag, CreateFilter filter) {
    return ParamsToValue(tag, static_cast<uint8_t>(filter));
  }
  constexpr static Tag NodeToTag(const Node placeholder) {
    return ValueToTag(NodeToValue(placeholder));
  }

  constexpr static uint8_t NodeToFilter(const Node placeholder) {
    return ValueToFilter(NodeToValue(placeholder));
  }
  constexpr static MatchFilter NodeToMatchFilter(const Node placeholder) {
    return static_cast<MatchFilter>(NodeToFilter(placeholder));
  }
  constexpr static CreateFilter NodeToCreateFilter(const Node placeholder) {
    return static_cast<CreateFilter>(NodeToFilter(placeholder));
  }
  constexpr static bool MatchesNode(const Node placeholder, const Node n) {
    Placeholder::MatchFilter filter = NodeToMatchFilter(placeholder);
    return filter == MatchFilter::None ||
           (filter == MatchFilter::Addition &&
            n.type() == BlockType::Addition) ||
           (filter == MatchFilter::Multiplication &&
            n.type() == BlockType::Multiplication);
  }

 private:
  constexpr static size_t k_bitsForTag =
      OMG::BitHelper::numberOfBitsToCountUpTo(Tag::NumberOfTags);
  constexpr static size_t k_bitsForMatchFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(MatchFilter::NumberOfFilters));
  constexpr static size_t k_bitsForCreateFilter =
      OMG::BitHelper::numberOfBitsToCountUpTo(
          static_cast<uint8_t>(CreateFilter::NumberOfFilters));
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
  constexpr static uint8_t NodeToValue(const Node placeholder) {
    assert(placeholder.type() == BlockType::Placeholder);
    return static_cast<uint8_t>(*(placeholder.block()->next()));
  }
  constexpr static Tag ValueToTag(uint8_t value) {
    return static_cast<Tag>(Bit::getBitRange(value, k_bitsForTag - 1, 0));
  }
  constexpr static uint8_t ValueToFilter(uint8_t value) {
    return Bit::getBitRange(value, k_bitsForTag + k_bitsForFilter - 1,
                            k_bitsForTag);
  }
};

namespace Placeholders {
static constexpr Placeholder::Tag A = Placeholder::Tag::A;
static constexpr Placeholder::Tag B = Placeholder::Tag::B;
static constexpr Placeholder::Tag C = Placeholder::Tag::C;
static constexpr Placeholder::Tag D = Placeholder::Tag::D;

static constexpr Placeholder::MatchFilter FilterAddition =
    Placeholder::MatchFilter::Addition;
static constexpr Placeholder::MatchFilter FilterMultiplication =
    Placeholder::MatchFilter::Multiplication;

static constexpr Placeholder::CreateFilter FilterFirstChild =
    Placeholder::CreateFilter::FirstChild;
static constexpr Placeholder::CreateFilter FilterNonFirstChild =
    Placeholder::CreateFilter::NonFirstChild;
}  // namespace Placeholders

}  // namespace PoincareJ

#endif
