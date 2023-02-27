#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare_junior/src/memory/edition_reference.h>
#include "aliases_list.h"

namespace PoincareJ {

class Builtin : public std::pair<BlockType, AliasesList> {
public:
  using pair::pair;
  const BlockType blockType() const {
    return first;
  }
  const AliasesList * aliasesList() const {
    return &second;
  }
};

class Builtins {
public:
  constexpr static AliasesList Name(BlockType type);
  static AliasesList Name(const Node block);
  static bool HasReservedFunction(UnicodeDecoder * name);
  static const Builtin * GetReservedFunction(UnicodeDecoder * name);
  static uint8_t MinNumberOfParameters(BlockType type);
  static uint8_t MaxNumberOfParameters(BlockType type);
  static EditionReference Build(BlockType type, EditionReference parameters);
};

}

#endif
