#include <escher/expression_input_bar.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

ExpressionInputBar::ExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, LayoutFieldDelegate * layoutFieldDelegate) :
  ExpressionField(parentResponder, inputEventHandler, layoutFieldDelegate, KDContext::k_alignLeft, KDContext::k_alignCenter),
  m_line(Palette::GrayMiddle)
{
  setMargins(k_margin);
}

void ExpressionInputBar::layoutSubviews(bool force) {
  ExpressionField::layoutSubviews(force);
  m_line.setFrame(KDRect(0, 0, bounds().width(), k_separatorThickness), force);
}

View * ExpressionInputBar::subviewAtIndex(int index) {
  if (index == ExpressionField::numberOfSubviews()) {
    return &m_line;
  }
  return ExpressionField::subviewAtIndex(index);
}

KDSize ExpressionInputBar::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight() + k_separatorThickness);
}

KDCoordinate ExpressionInputBar::inputViewHeight() const {
  return std::min(ExpressionField::inputViewHeight(), k_maximalHeight);
}

}
