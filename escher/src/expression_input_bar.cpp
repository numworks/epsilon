#include <escher/expression_input_bar.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

ExpressionInputBar::ExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate, KDContext::k_alignLeft, KDContext::k_alignCenter)
{
  m_textField.setMargins(0, k_horizontalMargin, 0, k_horizontalMargin);
  m_layoutField.setMargins(k_verticalMargin, k_horizontalMargin, k_verticalMargin, k_horizontalMargin);
}

void ExpressionInputBar::layoutSubviews(bool force) {
  KDRect inputViewFrame(0, k_separatorThickness, bounds().width(), bounds().height() - k_separatorThickness);
  layoutSubviewsInRect(inputViewFrame, force);
}

void ExpressionInputBar::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GrayMiddle);
}

KDSize ExpressionInputBar::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight() + k_separatorThickness);
}

KDCoordinate ExpressionInputBar::inputViewHeight() const {
  return (editionIsInTextField() ? k_minimalHeight :
           std::min(k_maximalHeight,
             std::max(k_minimalHeight, m_layoutField.minimalSizeForOptimalDisplay().height())));
}

}
