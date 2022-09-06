#ifndef POINCARE_PARSING_CONTEXT_H
#define POINCARE_PARSING_CONTEXT_H

#include <poincare/context.h>

namespace Poincare {

class ParsingContext {
public:
  /* We do not always understand identifiers like m or abc the same way.
   * This enum values are used by the IdentifierTokenizer to know how to
   * handle some ambiguous cases. */
  enum class ParsingMethod {
    Classic,
    Assignment, // f(x) is understood as a function and not f*(x). abc is understood as abc and not a*b*c
    UnitConversion, // 3m is understood as 3meters even if there is a value stored in the variable m
    ImplicitAdditionBetweenUnits // 4h40min30s is understood as 4h+40min+30s and not 4*h40*mi*n30*s
  };

  ParsingContext(Context * context, ParsingContext::ParsingMethod parsingMethod) : m_context(context), m_parsingMethod(parsingMethod) {}

  Context * context() { return m_context; }
  void setContext(Context * context) { m_context = context; }

  ParsingMethod parsingMethod() { return m_parsingMethod; }
  void setParsingMethod(ParsingMethod method) { m_parsingMethod = method; }

 private:
  Context * m_context;
  ParsingMethod m_parsingMethod;
};

}

#endif