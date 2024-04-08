#ifndef POINCARE_LAYOUT_PARSING_HELPER_H
#define POINCARE_LAYOUT_PARSING_HELPER_H

#include "token.h"

namespace PoincareJ {

class ParsingHelper {
 public:
  static bool IsLogicalOperator(const CPL* name, size_t nameLength,
                                Token::Type* returnType);
};
}  // namespace PoincareJ
#endif
