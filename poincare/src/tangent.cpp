#include <poincare/tangent.h>
#include "layout/horizontal_layout.h"

Expression * Tangent::clone() {
  return new Tangent(m_arg, true);
}

Expression::Type Tangent::type() {
  return Expression::Type::Tangent;
}

float Tangent::approximate(Context& context) {
  // FIXME: use tangent obviously.
  return m_arg->approximate(context);
}

#ifdef DEBUG
int Tangent::getPrintableVersion(char* txt) {
  const char* printable = "tan";
  for(int i=0; printable[i]!='\0'; i++) {
    txt[i] = printable[i];
  }
  return 3;
}
#endif
