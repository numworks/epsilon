#ifndef POINCARE_MEMORY_NODE_CONSTRUCTOR_H
#define POINCARE_MEMORY_NODE_CONSTRUCTOR_H

#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <utils/bit.h>
#include "value_block.h"

namespace PoincareJ {

class NodeConstructor final {
public:
  template <BlockType blockType, typename... Types>
  constexpr static bool CreateBlockAtIndexForType(Block * block, size_t blockIndex, Types... args) {
    if (blockIndex == 0) {
      *block = TypeBlock(blockType);
      return TypeBlock::NumberOfMetaBlocks(blockType) == 1;
    }
    return SpecializedCreateBlockAtIndexForType<blockType>(block, blockIndex, args...);
  }

private:
  template<typename... Types>
  constexpr static bool CreateBlockAtIndexForNthBlocksNode(Block * block, size_t index, BlockType type, Types... args) {
    constexpr int size = sizeof...(args);
    uint8_t values[size] = {static_cast<uint8_t>(args)...};
    if (size + 1 == index) {
      *block = TypeBlock(type);
      return true;
    }
    *block = ValueBlock(values[index - 1]);
    return false;
  }

  template <BlockType blockType, typename... Types>
  constexpr static bool SpecializedCreateBlockAtIndexForType(Block * block, size_t blockIndex, Types... args) {
    static_assert(blockType != BlockType::Constant &&
                  blockType != BlockType::Float &&
                  blockType != BlockType::UserSymbol &&
                  blockType != BlockType::IntegerPosBig &&
                  blockType != BlockType::IntegerNegBig,
                  "BlockType associated with specific specialized creators shouldn't end up in the default SpecializedCreateBlockAtIndexForType");
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, blockType, args...);
  }

  constexpr static bool CreateIntegerBlockAtIndexForType(Block * block, size_t blockIndex, BlockType type, uint64_t value) {
    uint8_t numberOfDigits = Integer::NumberOfDigits(value);
    assert(TypeBlock::NumberOfMetaBlocks(BlockType::IntegerPosBig) == TypeBlock::NumberOfMetaBlocks(BlockType::IntegerNegBig));
    uint8_t numberOfBlocks = TypeBlock::NumberOfMetaBlocks(BlockType::IntegerPosBig) + numberOfDigits;
    if (blockIndex == numberOfBlocks - 1) {
      *block = TypeBlock(type);
      return true;
    }
    if (blockIndex == 1 || blockIndex == numberOfBlocks - 2) {
      *block = ValueBlock(numberOfDigits);
      return false;
    }
    *block = ValueBlock(Integer::DigitAtIndex(value, blockIndex - 2));
    return false;
  }

  template <>
  constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::Constant>(Block * block, size_t blockIndex, char16_t name) {
    assert(Constant::Type(name) != Constant::Type::Undefined);
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, BlockType::Constant, Constant::Type(name));
  }

  template <>
  constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::UserSymbol>(Block * block, size_t blockIndex, const char * name, size_t nameLength) {
    size_t numberOfBlocks = TypeBlock::NumberOfMetaBlocks(BlockType::UserSymbol) + nameLength;
    if (blockIndex == numberOfBlocks - 1) {
      *block = TypeBlock(BlockType::UserSymbol);
      return true;
    }
    if (blockIndex == 1 || blockIndex == numberOfBlocks - 2) {
      *block = ValueBlock(nameLength);
      return false;
    }
    *block = ValueBlock(name[blockIndex - 2]);
    return false;
  }

  template <>
  constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::Float>(Block * block, size_t blockIndex, float value) {
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, BlockType::Float, Float::SubFloatAtIndex(value, 0), Float::SubFloatAtIndex(value, 1), Float::SubFloatAtIndex(value, 2), Float::SubFloatAtIndex(value, 3));
  }

  template <>
  constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::IntegerPosBig>(Block * block, size_t blockIndex, uint64_t value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerPosBig, value);
  }

  template <>
  constexpr bool NodeConstructor::SpecializedCreateBlockAtIndexForType<BlockType::IntegerNegBig>(Block * block, size_t blockIndex, uint64_t value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerNegBig, value);
  }
}

#endif
