#include <escher/expression_input_bar.h>
#include <poincare/preferences.h>
#include <assert.h>
#include <algorithm>

namespace Escher {

ExpressionInputBar::ExpressionInputBar(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandler, TextFieldDelegate * textFieldDelegate, LayoutFieldDelegate * layoutFieldDelegate) :
  ExpressionField(parentResponder, inputEventHandler, textFieldDelegate, layoutFieldDelegate, KDContext::k_alignLeft, KDContext::k_alignCenter)
{
  setMargins(k_verticalMargin, k_horizontalMargin, k_verticalMargin, k_horizontalMargin);
}

void ExpressionInputBar::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the separator
  ctx->fillRect(KDRect(0, 0, bounds().width(), k_separatorThickness), Palette::GrayMiddle);
}

KDSize ExpressionInputBar::minimalSizeForOptimalDisplay() const {
  return KDSize(0, inputViewHeight() + k_separatorThickness);
}

KDCoordinate ExpressionInputBar::inputViewHeight() const {
  return std::clamp(LayoutField::minimalSizeForOptimalDisplay().height(), k_minimalHeight, k_maximalHeight);
}

}
