#ifndef POINCARE_CONTEXT_H
#define POINCARE_CONTEXT_H

#include <poincare/expression.h>

//TODO: We should probably make a COPY of the expressions we store

class Context {
  public:
    Context();
    ~Context();
    Expression * operator[](const char * symbol_name) const;
    void setExpressionForSymbolName(Expression * expression, const char * symbol_name);
    /*
    Expression const& operator[](const char * symbol_name) const;
    Expression& operator[](const char * symbol_name);
    */
  private:
    struct ContextPair {
      const char * name;
      Expression * expression;
    };
    ContextPair * m_pairs;
    const uint16_t kMaxContextPairs = 10;
};

#endif
