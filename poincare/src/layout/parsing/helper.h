#ifndef POINCARE_LAYOUT_PARSING_HELPER_H
#define POINCARE_LAYOUT_PARSING_HELPER_H

#include "../layout_span_decoder.h"
#include "token.h"

namespace Poincare::Internal {

class Builtin;

class ParsingHelper {
 public:
  static bool IsLogicalOperator(LayoutSpan name, Token::Type* returnType);
  static bool ExtractInteger(const Tree* e, int* value);
  static const Builtin* GetInverseFunction(const Builtin* builtin);
  static bool IsPowerableFunction(const Builtin* builtin);
};
}  // namespace Poincare::Internal
#endif
