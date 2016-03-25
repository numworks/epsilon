#ifndef POINCARE_SINUS_H
#define POINCARE_SINUS_H

#include <poincare/function.h>

class Sinus : public Function {
  public:
    Sinus(Expression * arg, bool clone_arg=true): Function(arg, (char*) "sin", clone_arg) {}
    float approximate(Context& context) override;
    Type type() override;
    Expression * clone() override;
};

#endif
