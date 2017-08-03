#ifndef APPS_VARIABLE_BOX_LEAF_CELL_H
#define APPS_VARIABLE_BOX_LEAF_CELL_H

#include <escher.h>
#include <poincare.h>

class VariableBoxLeafCell : public HighlightCell {
public:
  VariableBoxLeafCell();
  ~VariableBoxLeafCell();
  void displayExpression(bool displayExpression);
  void reloadCell() override;
  void setLabel(const char * text);
  void setSubtitle(const char * text);
  void setExpression(const Poincare::Expression * expression);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  constexpr static KDCoordinate k_separatorThickness = 1;
  constexpr static KDCoordinate k_widthMargin = 10;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  /* The cell is responsible to delete the layout that may be created by its
   * dynamic method 'setExpression'.*/
  Poincare::ExpressionLayout * m_expressionLayout;
  BufferTextView m_labelView;
  BufferTextView m_subtitleView;
  ExpressionView m_expressionView;
  bool m_displayExpression;
};

#endif
