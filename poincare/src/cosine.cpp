#include <poincare/cosine.h>
#include "layout/horizontal_layout.h"

Expression * Cosine::clone() {
  return new Cosine(m_arg, true);
}

Expression::Type Cosine::type() {
  return Expression::Type::Cosine;
}

float Cosine::approximate(Context& context) {
  // FIXME: use cosine obviously.
  return m_arg->approximate(context);
}

#ifdef DEBUG
int Cosine::getPrintableVersion(char* txt) {
  const char* printable = "cos";
  for(int i=0; printable[i]!='\0'; i++) {
    txt[i] = printable[i];
  }
  return 3;
}
#endif
