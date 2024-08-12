#ifndef POINCARE_LAYOUT_PARSING_HELPER_H
#define POINCARE_LAYOUT_PARSING_HELPER_H

#include "../layout_span_decoder.h"
#include "token.h"

namespace Poincare::Internal {

class ParsingHelper {
 public:
  static bool IsLogicalOperator(LayoutSpan name, Token::Type* returnType);
  static bool ExtractInteger(const Tree* e, int* value);
};
}  // namespace Poincare::Internal
#endif
