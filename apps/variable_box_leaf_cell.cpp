#include "variable_box_leaf_cell.h"
#include <assert.h>

VariableBoxLeafCell::VariableBoxLeafCell() :
  TableViewCell(),
  m_labelView(BufferTextView(0, 0.5, KDColorBlack, Palette::CellBackgroundColor)),
  m_subtitleView(BufferTextView(0, 0.5, Palette::DesactiveTextColor, Palette::CellBackgroundColor)),
  m_displayExpression(false)
{
}

void VariableBoxLeafCell::displayExpression(bool displayExpression) {
  m_displayExpression = displayExpression;
}

int VariableBoxLeafCell::numberOfSubviews() const {
  if (m_displayExpression) {
    return 3;
  }
  return 2;
}

View * VariableBoxLeafCell::subviewAtIndex(int index) {
  if (index == 0) {
    return &m_labelView;
  }
  if (index == 1) {
    return &m_subtitleView;
  }
  assert(numberOfSubviews() == 3 && index == 2);
  return &m_expressionView;
}

void VariableBoxLeafCell::layoutSubviews() {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (numberOfSubviews() == 3) {
    m_labelView.setFrame(KDRect(1, 1, width/2-1, height/2 - 1));
    m_subtitleView.setFrame(KDRect(1, height/2, width/2-1, height/2 - 1));
    m_subtitleView.setAlignment(0.0f, 0.5f);
    m_expressionView.setFrame(KDRect(width/2, 1, width/2-1, height-2));
    return;
  }
  m_labelView.setFrame(KDRect(1, 1, width/2-1, height-2));
  m_subtitleView.setFrame(KDRect(width/2, 1, width/2-1, height-2));
  m_subtitleView.setAlignment(1.0f, 0.5f);
  return;
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
