#include <poincare/addition.h>
#include "layout/horizontal_layout.h"

Expression::Type Addition::type() {
  return Expression::Type::Addition;
}

Expression * Addition::clone() {
  return new Addition(m_operands, m_numberOfOperands, true);
}

float Addition::operateApproximatevelyOn(float a, float b) {
  return a + b;
}

char Addition::operatorChar() {
  return '+';
}

#ifdef DEBUG
int Addition::getPrintableVersion(char* txt) {
  txt[0] = '+';
  return 1;
}
#endif
