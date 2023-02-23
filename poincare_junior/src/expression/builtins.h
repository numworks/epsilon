#ifndef POINCARE_EXPRESSION_BUILTINS_H
#define POINCARE_EXPRESSION_BUILTINS_H

#include <poincare_junior/src/memory/edition_reference.h>
#include "aliases_list.h"

namespace PoincareJ {

class Builtins {
public:
  constexpr static AliasesList Name(BlockType type);
  static AliasesList Name(const Node block);
  static bool HasReservedFunction(UnicodeDecoder * name);
};

}

#endif
