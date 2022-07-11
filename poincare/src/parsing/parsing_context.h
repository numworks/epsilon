#ifndef POINCARE_PARSING_CONTEXT_H
#define POINCARE_PARSING_CONTEXT_H

#include <poincare/context.h>

namespace Poincare {

class ParsingContext {
public:
  enum class ParsingMethod {
    Classic,
    Assignment,
    UnitConversion
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