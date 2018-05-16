#include "variable_box_leaf_cell.h"
#include <assert.h>

using namespace Poincare;

VariableBoxLeafCell::VariableBoxLeafCell() :
  HighlightCell(),
  m_labelView(KDText::FontSize::Small, 0, 0.5, KDColorBlack, KDColorWhite),
  m_subtitleView(KDText::FontSize::Small, 0, 0.5, Palette::GreyDark, KDColorWhite),
  m_expressionView(1.0f, 0.5f),
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
  KDSize labelSize = m_labelView.minimalSizeForOptimalDisplay();
  if (numberOfSubviews() == 3) {
    KDSize subtitleSize = m_subtitleView.minimalSizeForOptimalDisplay();
    m_labelView.setFrame(KDRect(k_separatorThickness+k_widthMargin, k_separatorThickness, labelSize.width(), height/2 - k_separatorThickness));
    m_subtitleView.setFrame(KDRect(k_separatorThickness+k_widthMargin, height/2, subtitleSize.width(), height/2-k_separatorThickness));
    m_subtitleView.setAlignment(0.0f, 0.5f);
    KDCoordinate maxTextSize = subtitleSize.width() > labelSize.width() ? subtitleSize.width() : labelSize.width();
    KDCoordinate expressionAvailableSize = width-k_separatorThickness-3*k_widthMargin-maxTextSize;
    m_expressionView.setFrame(KDRect(width - expressionAvailableSize - k_widthMargin, k_separatorThickness, expressionAvailableSize, height-2*k_separatorThickness));
    if (m_expressionView.minimalSizeForOptimalDisplay().width() > expressionAvailableSize) {
      m_expressionView.setAlignment(0.0f, 0.5f);
    } else {
      m_expressionView.setAlignment(1.0f, 0.5f);
    }
    return;
  }
  m_labelView.setFrame(KDRect(k_separatorThickness+k_widthMargin, k_separatorThickness, labelSize.width(), height-2*k_separatorThickness));
  m_subtitleView.setFrame(KDRect(k_separatorThickness+k_widthMargin+labelSize.width(), k_separatorThickness, width-k_separatorThickness-2*k_widthMargin-labelSize.width(), height-2*k_separatorThickness));
  m_subtitleView.setAlignment(1.0f, 0.5f);
  return;
}

void VariableBoxLeafCell::reloadCell() {
  HighlightCell::reloadCell();
  KDColor backgroundColor = isHighlighted()? Palette::Select : KDColorWhite;
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

void VariableBoxLeafCell::setExpressionLayout(ExpressionLayout * expressionLayout) {
  m_expressionView.setExpressionLayout(expressionLayout);
}

void VariableBoxLeafCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  KDColor backgroundColor = isHighlighted() ? Palette::Select : KDColorWhite;
  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness, width-2*k_separatorThickness, height-2*k_separatorThickness), backgroundColor);
  ctx->fillRect(KDRect(0, height-k_separatorThickness, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, 0, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, 0, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
 }
