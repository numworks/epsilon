#ifndef POINCARE_STRING_LAYOUT_H
#define POINCARE_STRING_LAYOUT_H

#include <poincare/static_layout_hierarchy.h>
#include <poincare/layout_engine.h>
#include <string.h>

namespace Poincare {

class StringLayout : public StaticLayoutHierarchy<0> {
public:
  StringLayout(const char * string, size_t length, KDText::FontSize fontSize = KDText::FontSize::Large);
  ~StringLayout();
  StringLayout(const StringLayout& other) = delete;
  StringLayout(StringLayout&& other) = delete;
  StringLayout& operator=(const StringLayout& other) = delete;
  StringLayout& operator=(StringLayout&& other) = delete;
  ExpressionLayout * clone() const override;
  void backspaceAtCursor(ExpressionLayoutCursor * cursor) override;

  char * text() { return m_string; }
  KDText::FontSize fontSize() { return m_fontSize; }
  bool moveLeft(ExpressionLayoutCursor * cursor) override;
  bool moveRight(ExpressionLayoutCursor * cursor) override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
protected:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override;
  KDPoint positionOfChild(ExpressionLayout * child) override;
  KDSize computeSize() override;
  void computeBaseline() override;
  char * m_string;
  KDText::FontSize m_fontSize;
};

}

#endif
