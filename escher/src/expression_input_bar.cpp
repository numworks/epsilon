#include <escher/expression_input_bar.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

AbstractExpressionInputBar::AbstractExpressionInputBar() :
  m_line(Palette::GrayMiddle)
{
}

void AbstractExpressionInputBar::layoutSubviews(bool force) {
  m_line.setFrame(KDRect(0, 0, bounds().width(), k_separatorThickness), force);
  expressionField()->setFrame(KDRect(0, k_separatorThickness, bounds().width(), bounds().height() - k_separatorThickness), force);
}

View * AbstractExpressionInputBar::subviewAtIndex(int index) {
  if (index == 0) {
    return expressionField();
  }
  assert(index == 1);
  return &m_line;
}

KDSize AbstractExpressionInputBar::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight() + k_separatorThickness);
}

KDCoordinate AbstractExpressionInputBar::inputViewHeight() const {
  return std::clamp(expressionField()->inputViewHeight(), k_minimalHeight, k_maximalHeight);
}

}
