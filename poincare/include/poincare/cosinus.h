#ifndef POINCARE_COSINUS_H
#define POINCARE_COSINUS_H

#include <poincare/function.h>

class Cosinus : public Function {
  public:
    Cosinus(Expression * arg, bool clone_arg=true): Function(arg, (char*) "cos", clone_arg) {}
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
};

#endif
