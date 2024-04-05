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
  template <Type blockType, typename... Types>
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
                                                           Type type,
                                                           Types... args) {
    constexpr int size = sizeof...(args);
    uint8_t values[size] = {static_cast<uint8_t>(args)...};
    assert(index <= size);
    *block = ValueBlock(values[index - 1]);
    return index >= size;
  }

  template <Type blockType, typename... Types>
  constexpr static bool SpecializedCreateBlockAtIndexForType(Block* block,
                                                             size_t blockIndex,
                                                             Types... args) {
    static_assert(
        blockType != Type::PhysicalConstant && blockType != Type::SingleFloat &&
            blockType != Type::DoubleFloat && blockType != Type::UserSymbol &&
            blockType != Type::IntegerPosBig && blockType != Type::RackLayout &&
            blockType != Type::IntegerNegBig,
        "Type associated with specific specialized creators shouldn't end "
        "up in the default SpecializedCreateBlockAtIndexForType");
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, blockType,
                                              args...);
  }

  constexpr static bool CreateIntegerBlockAtIndexForType(Block* block,
                                                         size_t blockIndex,
                                                         Type type,
                                                         uint64_t value) {
    static_assert(TypeBlock::NumberOfMetaBlocks(Type::IntegerPosBig) ==
                  TypeBlock::NumberOfMetaBlocks(Type::IntegerNegBig));
    size_t numberOfMetaBlocks =
        TypeBlock::NumberOfMetaBlocks(Type::IntegerPosBig);
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
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::PhysicalConstant>(
    Block* block, size_t blockIndex, uint8_t index) {
  assert(index < Constant::k_numberOfConstants);
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex,
                                            Type::PhysicalConstant, index);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::Matrix>(
    Block* block, size_t blockIndex, int rows, int cols) {
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, Type::Matrix,
                                            rows, cols);
}

constexpr bool CreateBlockAtIndexForUserType(Type type, Block* block,
                                             size_t blockIndex,
                                             const char* name,
                                             size_t nameSize) {
  size_t numberOfMetaBlocks = TypeBlock::NumberOfMetaBlocks(type);
  if (blockIndex < numberOfMetaBlocks) {
    assert(blockIndex == 1);
    *block = ValueBlock(nameSize);
    return nameSize == 0;
  }
  *block = ValueBlock(name[blockIndex - numberOfMetaBlocks]);
  return blockIndex - numberOfMetaBlocks >= nameSize - 1;
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::UserFunction>(
    Block* block, size_t blockIndex, const char* name, size_t nameSize) {
  return CreateBlockAtIndexForUserType(Type::UserFunction, block, blockIndex,
                                       name, nameSize);
}
template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::UserSequence>(
    Block* block, size_t blockIndex, const char* name, size_t nameSize) {
  return CreateBlockAtIndexForUserType(Type::UserSequence, block, blockIndex,
                                       name, nameSize);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::UserSymbol>(
    Block* block, size_t blockIndex, const char* name, size_t nameSize) {
  return CreateBlockAtIndexForUserType(Type::UserSymbol, block, blockIndex,
                                       name, nameSize);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::SingleFloat>(
    Block* block, size_t blockIndex, float value) {
  static_assert(sizeof(float) / sizeof(uint8_t) == 4);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::SingleFloat,
      FloatNode::SubFloatAtIndex(value, 0),
      FloatNode::SubFloatAtIndex(value, 1),
      FloatNode::SubFloatAtIndex(value, 2),
      FloatNode::SubFloatAtIndex(value, 3));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::DoubleFloat>(
    Block* block, size_t blockIndex, double value) {
  static_assert(sizeof(double) / sizeof(uint8_t) == 8);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::DoubleFloat,
      FloatNode::SubFloatAtIndex(value, 0),
      FloatNode::SubFloatAtIndex(value, 1),
      FloatNode::SubFloatAtIndex(value, 2),
      FloatNode::SubFloatAtIndex(value, 3),
      FloatNode::SubFloatAtIndex(value, 4),
      FloatNode::SubFloatAtIndex(value, 5),
      FloatNode::SubFloatAtIndex(value, 6),
      FloatNode::SubFloatAtIndex(value, 7));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    Type::AsciiCodePointLayout>(Block* block, size_t blockIndex,
                                CodePoint value) {
  assert(value < 128);
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex,
                                            Type::AsciiCodePointLayout,
                                            Bit::getByteAtIndex(value, 0));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    Type::UnicodeCodePointLayout>(Block* block, size_t blockIndex,
                                  CodePoint value) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  // assert(value >= 128);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::UnicodeCodePointLayout,
      Bit::getByteAtIndex(value, 0), Bit::getByteAtIndex(value, 1),
      Bit::getByteAtIndex(value, 2), Bit::getByteAtIndex(value, 3));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    Type::CombinedCodePointsLayout>(Block* block, size_t blockIndex,
                                    CodePoint first, CodePoint second) {
  static_assert(sizeof(CodePoint) / sizeof(uint8_t) == 4);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::CombinedCodePointsLayout,
      Bit::getByteAtIndex(first, 0), Bit::getByteAtIndex(first, 1),
      Bit::getByteAtIndex(first, 2), Bit::getByteAtIndex(first, 3),
      Bit::getByteAtIndex(second, 0), Bit::getByteAtIndex(second, 1),
      Bit::getByteAtIndex(second, 2), Bit::getByteAtIndex(second, 3));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::RackLayout>(
    Block* block, size_t blockIndex, int nbChildren) {
  assert(nbChildren < UINT16_MAX);
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, Type::RackLayout,
                                            nbChildren % 256, nbChildren / 256);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::ParenthesisLayout>(
    Block* block, size_t blockIndex, bool leftIsTemporary,
    bool rightIsTemporary) {
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::ParenthesisLayout,
      leftIsTemporary | (0b10 && rightIsTemporary));
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    Type::VerticalOffsetLayout>(Block* block, size_t blockIndex,
                                bool isSubscript, bool isPrefix) {
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex,
                                            Type::VerticalOffsetLayout,
                                            isSubscript | (0b10 && isPrefix));
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::Variable>(
    Block* block, size_t blockIndex, uint8_t id, ComplexSign sign) {
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, Type::Variable,
                                            id, sign.getValue());
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::IntegerPosBig>(
    Block* block, size_t blockIndex, uint64_t value) {
  return CreateIntegerBlockAtIndexForType(block, blockIndex,
                                          Type::IntegerPosBig, value);
}

template <>
constexpr bool
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::IntegerNegBig>(
    Block* block, size_t blockIndex, uint64_t value) {
  return CreateIntegerBlockAtIndexForType(block, blockIndex,
                                          Type::IntegerNegBig, value);
}
}  // namespace PoincareJ

#endif
