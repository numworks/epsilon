#ifndef POINCARE_LAYOUT_PARSING_HELPER_H
#define POINCARE_LAYOUT_PARSING_HELPER_H

#include "token.h"

namespace Poincare::Internal {

class ParsingHelper {
 public:
  static bool IsLogicalOperator(const CPL* name, size_t nameLength,
                                Token::Type* returnType);
};
}  // namespace Poincare::Internal
#endif
