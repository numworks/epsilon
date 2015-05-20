#include <poincare/number.h>
#include <kandinsky/text.h>
#include <string.h>

/*Number::Number(int v) : m_value(v) {
  for (int i=0; i<16; i++) {
    m_stringValue[i] = 0;
  }

  int value = v;
  for (int i=0; i<15; i++) {
    int digit = value - 10*(value/10);
    m_stringValue[i] = '0' + digit;
    value = value/10;
    if (value == 0) {
      break;
    }
  }
}*/

Number::Number(char * string) {
  // FIXME: use strdup
  memset(m_stringValue, 0, 16);
  for (int i=0;i<15;i++) {
    if (string[i] == 0) {
      break;
    }
    m_stringValue[i] = string[i];
  }
}


Expression ** Number::children() {
  return NULL;
}

void Number::layout() {
  m_frame.size = KDStringSize(m_stringValue);
}

void Number::draw() {
  KDDrawString(m_stringValue, KDPOINT(0,0));
}
