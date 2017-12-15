#ifndef EXPRESSION_EDITOR_EXPRESSION_VIEW_WITH_CURSOR_H
#define EXPRESSION_EDITOR_EXPRESSION_VIEW_WITH_CURSOR_H

#include <escher.h>
#include <kandinsky/point.h>
#include <poincare/expression_layout.h>
#include <poincare/expression_layout_cursor.h>

namespace ExpressionEditor {

class ExpressionViewWithCursor : public View {
public:
  ExpressionViewWithCursor(Poincare::ExpressionLayout * expressionLayout, Poincare::ExpressionLayoutCursor * cursor);
  void cursorPositionChanged();
  KDRect cursorRect();
  const ExpressionView * expressionView() const { return &m_expressionView; }
  /* View */
  KDSize minimalSizeForOptimalDisplay() const override;
private:
  enum class Position {
    Top,
    Bottom
  };
  int numberOfSubviews() const override { return 2; }
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void layoutCursorSubview();
  Poincare::ExpressionLayoutCursor * m_cursor;
  ExpressionView m_expressionView;
  TextCursorView m_cursorView;
};

}

#endif
