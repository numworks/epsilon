#ifndef POINCARE_MEMORY_NODE_CONSTRUCTOR_H
#define POINCARE_MEMORY_NODE_CONSTRUCTOR_H

#include <omg/bit_helper.h>
#include <omg/enums.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/sign.h>
#include <poincare_junior/src/layout/code_point_layout.h>

#include "value_block.h"

namespace PoincareJ {

class NodeConstructor final {
 public:
  template <BlockType blockType, typename... Types>
  constexpr static bool CreateBlockAtIndexForType(Block* block,
                                                  size_t blockIndex,
                                                  Types... args) {
    if (blockIndex == 0) {
      *block = TypeBlock(blockType);
      return TypeBlock::NumberOfMetaBlocks(blockType) == 1;
    }
    return SpecializedCreateBlockAtIndexForType<blockType>(block, blockIndex,
                                                           args...);
  }

 private:
  template <typename... Types>
  constexpr static bool CreateBlockAtIndexForNthBlocksNode(Block* block,
                                                           size_t index,
                                                           BlockType type,
                                                           Types... args) {
    constexpr int size = sizeof...(args);
    uint8_t values[size] = {static_cast<uint8_t>(args)...};
    assert(index <= size);
    *block = ValueBlock(values[index - 1]);
    return index >= size;
  }

  template <BlockType blockType, typename... Types>
  constexpr static bool SpecializedCreateBlockAtIndexForType(Block* block,
                                                             size_t blockIndex,
                                                             Types... args) {
    static_assert(
        blockType != BlockType::Constant &&
            blockType != BlockType::SingleFloat &&
            blockType != BlockType::DoubleFloat &&
            blockType != BlockType::UserSymbol &&
            blockType != BlockType::IntegerPosBig &&
            blockType != BlockType::IntegerNegBig,
        "BlockType associated with specific specialized creators shouldn't end "
        "up in the default SpecializedCreateBlockAtIndexForType");
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, blockType,
                                              args...);
  }

  constexpr static bool CreateIntegerBlockAtIndexForType(Block* block,
                                                         size_t blockIndex,
                                                         BlockType type,
                                                         uint64_t value) {
    static_assert(TypeBlock::NumberOfMetaBlocks(BlockType::IntegerPosBig) ==
                  TypeBlock::NumberOfMetaBlocks(BlockType::IntegerNegBig));
    size_t numberOfMetaBlocks =
        TypeBlock::NumberOfMetaBlocks(BlockType::IntegerPosBig);
    uint8_t numberOfDigits = Integer::NumberOfDigits(value);
    if (blockIndex < numberOfMetaBlocks) {
      assert(blockIndex == 1);
      *block = ValueBlock(numberOfDigits);
      return numberOfDigits == 0;
    }
    *block = ValueBlock(
        Integer::DigitAtIndex(value, blockIndex - numberOfMetaBlocks));
    return blockIndex + 1 >= numberOfMetaBlocks + numberOfDigits;
  }
};

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::Constant>(
    Block* block, size_t blockIndex, char16_t name) {
  assert(Constant::Type(name) != Constant::Type::Undefined);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::Constant, Constant::Type(name));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::Matrix>(
    Block* block, size_t blockIndex, int rows, int cols) {
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex,
                                            BlockType::Matrix, rows, cols);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::UserSymbol>(
    Block* block, size_t blockIndex, const char* name, size_t nameLength) {
  size_t numberOfMetaBlocks =
      TypeBlock::NumberOfMetaBlocks(BlockType::UserSymbol);
  if (blockIndex < numberOfMetaBlocks) {
    assert(blockIndex == 1);
    *block = ValueBlock(nameLength);
    return nameLength == 0;
  }
  *block = ValueBlock(name[blockIndex - numberOfMetaBlocks]);
  return blockIndex - numberOfMetaBlocks >= nameLength - 1;
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::SingleFloat>(
    Block* block, size_t blockIndex, float value) {
  static_assert(sizeof(float) / sizeof(uint8_t) == 4);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::SingleFloat,
      Float::SubFloatAtIndex(value, 0), Float::SubFloatAtIndex(value, 1),
      Float::SubFloatAtIndex(value, 2), Float::SubFloatAtIndex(value, 3));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::DoubleFloat>(
    Block* block, size_t blockIndex, double value) {
  static_assert(sizeof(double) / sizeof(uint8_t) == 8);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::DoubleFloat,
      Float::SubFloatAtIndex(value, 0), Float::SubFloatAtIndex(value, 1),
      Float::SubFloatAtIndex(value, 2), Float::SubFloatAtIndex(value, 3),
      Float::SubFloatAtIndex(value, 4), Float::SubFloatAtIndex(value, 5),
      Float::SubFloatAtIndex(value, 6), Float::SubFloatAtIndex(value, 7));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    BlockType::CodePointLayout>(Block* block, size_t blockIndex,
                                CodePoint value) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::CodePointLayout,
      CodePointLayout::SubCodePointLayoutAtIndex(value, 0),
      CodePointLayout::SubCodePointLayoutAtIndex(value, 1),
      CodePointLayout::SubCodePointLayoutAtIndex(value, 2),
      CodePointLayout::SubCodePointLayoutAtIndex(value, 3));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    BlockType::CombinedCodePointsLayout>(Block* block, size_t blockIndex,
                                         CodePoint first, CodePoint second) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::CodePointLayout,
      CodePointLayout::SubCodePointLayoutAtIndex(first, 0),
      CodePointLayout::SubCodePointLayoutAtIndex(first, 1),
      CodePointLayout::SubCodePointLayoutAtIndex(first, 2),
      CodePointLayout::SubCodePointLayoutAtIndex(first, 3),
      CodePointLayout::SubCodePointLayoutAtIndex(second, 0),
      CodePointLayout::SubCodePointLayoutAtIndex(second, 1),
      CodePointLayout::SubCodePointLayoutAtIndex(second, 2),
      CodePointLayout::SubCodePointLayoutAtIndex(second, 3));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    BlockType::ParenthesisLayout>(Block* block, size_t blockIndex,
                                  bool leftIsTemporary, bool rightIsTemporary) {
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::CodePointLayout,
      leftIsTemporary | (0b10 && rightIsTemporary));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::Variable>(
    Block* block, size_t blockIndex, uint8_t id, ComplexSign sign) {
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, BlockType::Variable, id, sign.getValue());
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::IntegerPosBig>(
    Block* block, size_t blockIndex, uint64_t value) {
  return CreateIntegerBlockAtIndexForType(block, blockIndex,
                                          BlockType::IntegerPosBig, value);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::IntegerNegBig>(
    Block* block, size_t blockIndex, uint64_t value) {
  return CreateIntegerBlockAtIndexForType(block, blockIndex,
                                          BlockType::IntegerNegBig, value);
}
}  // namespace PoincareJ

#endif
