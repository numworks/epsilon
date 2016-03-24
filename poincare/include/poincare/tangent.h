#ifndef POINCARE_TANGENT_H
#define POINCARE_TANGENT_H

#include <poincare/function.h>

class Tangent : public Function {
  public:
    Tangent(Expression * arg): Function(arg, (char*) "tan") {}
    float approximate(Context& context) override;
    Type type() override;
};

#endif
