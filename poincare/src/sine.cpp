#include <poincare/sine.h>
#include "layout/horizontal_layout.h"

Expression * Sine::clone() {
  return new Sine(m_arg, true);
}

Expression::Type Sine::type() {
  return Expression::Type::Sine;
}

float Sine::approximate(Context& context) {
  // FIXME: use sine obviously.
  return m_arg->approximate(context);
}

#ifdef DEBUG
int Sine::getPrintableVersion(char* txt) {
  const char* printable = "sin";
  for(int i=0; printable[i]!='\0'; i++) {
    txt[i] = printable[i];
  }
  return 3;
}
#endif
