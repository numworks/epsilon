#ifndef APPS_VARIABLE_BOX_LEAF_CELL_H
#define APPS_VARIABLE_BOX_LEAF_CELL_H

#include <escher.h>

class VariableBoxLeafCell : public TableViewCell {
public:
  VariableBoxLeafCell();
  void reloadCell() override;
  void setLabel(const char * text);
  void setSubtitle(const char * text);
  void setExpression(ExpressionLayout * expressionLayout);
  void drawRect(KDContext * ctx, KDRect rect) const override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  PointerTextView m_labelView;
  BufferTextView m_subtitleView;
  ExpressionView m_expressionView;
};

#endif
