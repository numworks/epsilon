#ifndef POINCARE_MEMORY_NODE_CONSTRUCTOR_H
#define POINCARE_MEMORY_NODE_CONSTRUCTOR_H

#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/float.h>
#include <poincare_junior/src/expression/integer.h>
#include <utils/bit.h>
#include "value_block.h"

namespace Poincare {

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
                  blockType != BlockType::IntegerPosBig &&
                  blockType != BlockType::IntegerNegBig,
                  "BlockType associated with specific specialized creators shouldn't end up in the default SpecializedCreateBlockAtIndexForType");
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, blockType, args...);
  }

  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::Constant>(Block * block, size_t blockIndex, char16_t name) {
    assert(Constant::Type(name) != Constant::Type::Undefined);
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, BlockType::Constant, Constant::Type(name));
  }

  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::Float>(Block * block, size_t blockIndex, float value) {
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, BlockType::Float, Float::SubFloatAtIndex(value, 0), Float::SubFloatAtIndex(value, 1), Float::SubFloatAtIndex(value, 2), Float::SubFloatAtIndex(value, 3));
  }

  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::IntegerPosBig>(Block * block, size_t blockIndex, uint64_t value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerPosBig, value);
  }
  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::IntegerNegBig>(Block * block, size_t blockIndex, uint64_t value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerNegBig, value);
  }

  constexpr static bool CreateIntegerBlockAtIndexForType(Block * block, size_t blockIndex, BlockType type, uint64_t value) {
    uint8_t numberOfDigits = Integer::NumberOfDigits(value);
    switch (numberOfDigits) {
      case 1:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, Integer::DigitAtIndex(value, 0), numberOfDigits);
      case 2:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, Integer::DigitAtIndex(value, 0), Integer::DigitAtIndex(value, 1), numberOfDigits);
      case 3:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, Integer::DigitAtIndex(value, 0), Integer::DigitAtIndex(value, 1), Integer::DigitAtIndex(value, 2), numberOfDigits);
      case 4:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, Integer::DigitAtIndex(value, 0), Integer::DigitAtIndex(value, 1), Integer::DigitAtIndex(value, 2), Integer::DigitAtIndex(value, 3), numberOfDigits);
      default:
        assert(false);
        // TODO: handle case for numberOfDigits == 5, 6, 7, 8 when they occur
        return false;
    }
  }
};


}

#endif
