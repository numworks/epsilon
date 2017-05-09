#ifndef POINCARE_STRING_LAYOUT_H
#define POINCARE_STRING_LAYOUT_H

#include <poincare/expression_layout.h>
#include <string.h>

namespace Poincare {

class StringLayout : public ExpressionLayout {
public:
  // Here the inverse is a uint8_t instead of a bool, because the size of a bool is
  // not standardized, thus since we call a foreign C function with this value we want to be
  // sure about compatibility.
  StringLayout(const char * string, size_t length, KDText::FontSize fontSize = KDText::FontSize::Large);
  ~StringLayout();
  StringLayout(const StringLayout& other) = delete;
  StringLayout(StringLayout&& other) = delete;
  StringLayout& operator=(const StringLayout& other) = delete;
  StringLayout& operator=(StringLayout&& other) = delete;
  char * text();
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDSize computeSize() override;
  ExpressionLayout * child(uint16_t index) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
private:
  char * m_string;
  KDText::FontSize m_fontSize;
};

}

#endif
