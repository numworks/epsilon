#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Expression::Type Addition::type() const {
  return Expression::Type::Addition;
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  return new Addition(newOperands, numberOfOperands, cloneOperands);
}

float Addition::operateApproximatevelyOn(float a, float b) const {
  return a + b;
}

char Addition::operatorChar() const {
  return '+';
}
