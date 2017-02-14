#include <assert.h>
#include <stdlib.h>
#include "string_layout.h"

namespace Poincare {

StringLayout::StringLayout(const char * string, size_t length, KDText::FontSize fontSize) :
  ExpressionLayout(),
  m_fontSize(fontSize)
{
  m_string = (char *)malloc(sizeof(char)*(length+1));
  memcpy(m_string, string, length);
  m_string[length] = 0;
  // Height of the font.
  m_baseline = KDText::stringSize(" ", m_fontSize).height();
}

StringLayout::~StringLayout() {
  free(m_string);
}

ExpressionLayout * StringLayout::child(uint16_t index) {
  return nullptr;
}

void StringLayout::render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) {
  ctx->drawString(m_string, p, m_fontSize, expressionColor, backgroundColor);
}

KDPoint StringLayout::positionOfChild(ExpressionLayout * child) {
  assert(false); // We should never be here
  return KDPointZero;
}

KDSize StringLayout::computeSize() {
  return KDText::stringSize(m_string, m_fontSize);
}

}
