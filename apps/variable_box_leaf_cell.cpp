#include "variable_box_leaf_cell.h"
#include <assert.h>

VariableBoxLeafCell::VariableBoxLeafCell() :
  TableViewCell(),
  m_labelView(PointerTextView(nullptr, 0, 0.5, KDColorBlack, Palette::CellBackgroundColor)),
  m_subtitleView(BufferTextView(0, 0.5, Palette::DesactiveTextColor, Palette::CellBackgroundColor))  
{
}

int VariableBoxLeafCell::numberOfSubviews() const {
  if (strlen(m_subtitleView.text()) > 0) {
    return 3;
  }
  return 2;
}

View * VariableBoxLeafCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_labelView;
  }
  if (index == 1) {
    return &m_expressionView;
  }
  assert(numberOfSubviews() == 3 && index == 2);
  return &m_subtitleView;
}

void VariableBoxLeafCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  m_expressionView.setFrame(KDRect(width/2, 1, width/2-1, height-2));
  if (numberOfSubviews() == 3) {
    m_labelView.setFrame(KDRect(1, 1, width/2-1, height/2 - 1));
    m_subtitleView.setFrame(KDRect(1, height/2, width/2-1, height/2 - 1));
    return;
  }
  m_labelView.setFrame(KDRect(1, 1, width/2-1, height-2));
}

void VariableBoxLeafCell::reloadCell() {
  TableViewCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  m_labelView.setBackgroundColor(backgroundColor);
  m_subtitleView.setBackgroundColor(backgroundColor);
  m_expressionView.setBackgroundColor(backgroundColor);
}

void VariableBoxLeafCell::setLabel(const char * text) {
  m_labelView.setText(text);
}

void VariableBoxLeafCell::setSubtitle(const char * text) {
  m_subtitleView.setText(text);
  layoutSubviews();
}

void VariableBoxLeafCell::setExpression(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpression(expressionLayout);
}

void VariableBoxLeafCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::FocusCellBackgroundColor : Palette::CellBackgroundColor;
  ctx->fillRect(KDRect(1, 0, width-2, height-1), backgroundColor);
  ctx->fillRect(KDRect(0, height-1, width, 1), Palette::LineColor);
  ctx->fillRect(KDRect(0, 0, 1, height-1), Palette::LineColor);
  ctx->fillRect(KDRect(width-1, 0, 1, height-1), Palette::LineColor);
 }
