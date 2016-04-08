#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Expression::Type Addition::type() {
  return Expression::Type::Addition;
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) {
  return new Addition(newOperands, numberOfOperands, cloneOperands);
}

float Addition::operateApproximatevelyOn(float a, float b) {
  return a + b;
}

char Addition::operatorChar() {
  return '+';
}
