#ifndef POINCARE_COSINUS_H
#define POINCARE_COSINUS_H

#include <poincare/function.h>

class Cosinus : public Function {
  public:
    Cosinus(Expression * arg): Function(arg, (char*) "cos") {}
    float approximate(Context& context) override;
    Type type() override;
};

#endif
