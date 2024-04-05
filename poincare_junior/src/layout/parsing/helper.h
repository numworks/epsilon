#ifndef POINCARE_JUNIOR_LAYOUT_PARSING_HELPER_H
#define POINCARE_JUNIOR_LAYOUT_PARSING_HELPER_H

#include "token.h"

namespace PoincareJ {

class ParsingHelper {
 public:
  static bool IsLogicalOperator(const CPL* name, size_t nameLength,
                                Token::Type* returnType);
};
}  // namespace PoincareJ
#endif
