#include <assert.h>
#include <stdlib.h>
#include "string_layout.h"

StringLayout::StringLayout(const char * string, size_t length, uint8_t inverse) :
ExpressionLayout() {
  assert(string[length] == 0); // Assert NULL-termination
  m_string = (char *)malloc(sizeof(char)*(length+1));
  memcpy(m_string, string, (length+1));
  m_inverse = inverse;
  // Height of the font.
  m_baseline = KDStringSize(" ").height;
}

StringLayout::~StringLayout() {
  free(m_string);
}

ExpressionLayout * StringLayout::child(uint16_t index) {
  return nullptr;
}

void StringLayout::render(KDPoint point) {
  KDDrawString(m_string, point, m_inverse);
}

KDPoint StringLayout::positionOfChild(ExpressionLayout * child) {
  assert(false); // We should never be here
  return KDPointZero;
}

KDSize StringLayout::computeSize() {
  return KDStringSize(m_string);
}
