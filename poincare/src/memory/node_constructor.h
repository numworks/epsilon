#ifndef POINCARE_MEMORY_NODE_CONSTRUCTOR_H
#define POINCARE_MEMORY_NODE_CONSTRUCTOR_H

#include <omg/bit_helper.h>
#include <poincare/src/expression/float.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/layout/code_point_layout.h>

#include "value_block.h"

namespace Poincare::Internal {

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
            blockType != Type::UserSequence &&
            blockType != Type::UserFunction &&
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
NodeConstructor::SpecializedCreateBlockAtIndexForType<Type::Matrix>(
    Block* block, size_t blockIndex, int rows, int cols) {
  return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, Type::Matrix,
                                            rows, cols);
}

template <>
constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<
    Type::AsciiCodePointLayout>(Block* block, size_t blockIndex,
                                CodePoint value) {
  assert(value < 128);
  return CreateBlockAtIndexForNthBlocksNode(
      block, blockIndex, Type::AsciiCodePointLayout,
      OMG::BitHelper::getByteAtIndex(value, 0));
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
}  // namespace Poincare::Internal

#endif
