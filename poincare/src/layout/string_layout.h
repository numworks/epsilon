#ifndef POINCARE_STRING_LAYOUT_H
#define POINCARE_STRING_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <string.h>

namespace Poincare {

class StringLayout : public StaticLayoutHierarchy<0> {
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
  ExpressionLayout * clone() const override;

  char * text() { return m_string; }
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  KDSize computeSize() override;
  char * m_string;
  KDText::FontSize m_fontSize;
};

}

#endif
