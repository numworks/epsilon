#ifndef POINCARE_SINUS_H
#define POINCARE_SINUS_H

#include <poincare/function.h>

class Sinus : public Function {
  public:
    Sinus(Expression * arg): Function(arg, (char*) "sin") {}
    float approximate(Context& context) override;
    Type type() override;
};

#endif
