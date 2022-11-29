#ifndef POINCARE_MEMORY_NODE_CONSTRUCTOR_H
#define POINCARE_MEMORY_NODE_CONSTRUCTOR_H

#include <poincare_junior/src/expression/constant.h>
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
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, blockType, args...);
  }

  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::Constant>(Block * block, size_t blockIndex, char16_t name) {
    Constant::Type type = name == 'e' ? Constant::Type::E : name == u'π' ? Constant::Type::Pi : Constant::Type::Undefined;
    assert(type != Constant::Type::Undefined);
    return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, BlockType::Constant, type);
  }

  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::IntegerPosBig>(Block * block, size_t blockIndex, unsigned int value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerPosBig, value);
  }
  template <>
  constexpr bool SpecializedCreateBlockAtIndexForType<BlockType::IntegerNegBig>(Block * block, size_t blockIndex, unsigned int value) {
    return CreateIntegerBlockAtIndexForType(block, blockIndex, BlockType::IntegerNegBig, value);
  }

  // TODO move?
  constexpr static uint8_t NumberOfDigits(unsigned int value) {
    uint8_t numberOfDigits = 0;
    while (value && numberOfDigits < 4) {
      value = value >> 8;
      numberOfDigits++;
    }
    return numberOfDigits;
  }

  // TODO move
  constexpr static uint8_t DigitAtIndex(unsigned int value, int index) {
    int maskOffset = index * 8;
    assert(maskOffset <= sizeof(int)/sizeof(uint8_t) * 8);
    return value & (0xFF << maskOffset);
  }

  constexpr static bool CreateIntegerBlockAtIndexForType(Block * block, size_t blockIndex, BlockType type, unsigned int value) {
    uint8_t numberOfDigits = NumberOfDigits(value);
    assert(numberOfDigits > 1);
    switch (numberOfDigits) {
      case 2:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, DigitAtIndex(value, 0), DigitAtIndex(value, 1), numberOfDigits);
      case 3:
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, DigitAtIndex(value, 0), DigitAtIndex(value, 1), DigitAtIndex(value, 2), numberOfDigits);
      default:
        assert(numberOfDigits == 4);
        return CreateBlockAtIndexForNthBlocksNode(block, blockIndex, type, numberOfDigits, DigitAtIndex(value, 0), DigitAtIndex(value, 1), DigitAtIndex(value, 2), DigitAtIndex(value, 3), numberOfDigits);
    }
  }
};


}

#endif
