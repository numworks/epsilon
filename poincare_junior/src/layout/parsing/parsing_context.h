#ifndef POINCARE_JUNIOR_LAYOUT_PARSING_CONTEXT_H
#define POINCARE_JUNIOR_LAYOUT_PARSING_CONTEXT_H

#include <poincare/context.h>

namespace PoincareJ {

class ParsingContext {
 public:
  /* We do not always understand identifiers like m or abc the same way.
   * This enum values are used by the IdentifierTokenizer to know how to
   * handle some ambiguous cases. */
  enum class ParsingMethod {
    Classic,
    Assignment,      // f(x) is understood as a function and not f*(x). abc is
                     // understood as abc and not a*b*c
    UnitConversion,  // 3m is understood as 3meters even if there is a value
                     // stored in the variable m
    ImplicitAdditionBetweenUnits,  // 4h40min30s is understood as 4h+40min+30s
                                   // and not 4*h40*mi*n30*s
    CommaSeparatedList  // 1,2,3 is parsed as system list of 1, 2 and 3
  };

  ParsingContext(
      /*Poincare::Context * context, */ ParsingContext::ParsingMethod
          parsingMethod)
      : m_context(nullptr), m_parsingMethod(parsingMethod) {}

  Poincare::Context* context() { return m_context; }
  void setContext(Poincare::Context* context) { m_context = context; }

  ParsingMethod parsingMethod() { return m_parsingMethod; }
  void setParsingMethod(ParsingMethod method) { m_parsingMethod = method; }

 private:
  Poincare::Context* m_context;
  ParsingMethod m_parsingMethod;
};

}  // namespace PoincareJ

#endif
