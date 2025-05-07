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

  static bool ParameterText(UnicodeDecoder& varDecoder, size_t* parameterStart,
                            size_t* parameterLength);
  static bool ParameterText(LayoutSpanDecoder* varDecoder,
                            const Layout** parameterStart,
                            size_t* parameterLength);
  constexpr static int k_indexOfMainExpression1D = 0;
  constexpr static int k_indexOfParameter1D = 1;
};

}  // namespace Poincare::Internal
#endif
