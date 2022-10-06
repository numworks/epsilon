#include "function_cell.h"
#include <escher/palette.h>

using namespace Escher;

namespace Graph {

/* Function cell has the folowing layout :
  *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
  * |####|  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _                   |              |
  * |####|  |                             |                   |              |
  * |####|  |   ExpressionView            |                   |              |
  * |####|  |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _|                   | EllipsisView |
  * |####|  _ _ _ _ _ _ _ _ _ _                               |              |
  * |####|  | MessageTextView |                               |              |
  * |####|  |_ _ _ _ _ _ _ _ _|                               |              |
  * |####|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ |_ _ _ _ _ _ _ |
  *
  * A few notes :
  * - Leftmost rectangle is the color indicator.
  * - There are no borders to draw
  * - EllipsisView dictates the minimal height of the cell
  * - ExpressionView is cropped in width, but can take significant height
  * - If inactive, both color indicator and all texts are set to gray
*/

FunctionCell::FunctionCell() :
    EvenOddCell(),
    m_messageTextView(KDFont::Size::Small,
                      (I18n::Message)0,
                      0.0f,
                      0.0f,
                      Palette::GrayDark),
    m_functionColor(KDColorBlack),
    m_expressionBackground(KDColorWhite),
    m_ellipsisBackground(KDColorWhite),
    m_parameterSelected(false) {
}

void FunctionCell::drawRect(KDContext * ctx, KDRect rect) const {
  // Draw the color indicator
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()),
                m_functionColor);
  // Color the main background
  ctx->fillRect(KDRect(k_colorIndicatorThickness, 0,
                       bounds().width() - k_colorIndicatorThickness -
                           k_parametersColumnWidth,
                       bounds().height()),
                m_expressionBackground);
  // Color the ellipsis view
  ctx->fillRect(KDRect(bounds().width() - k_parametersColumnWidth, 0,
                       k_parametersColumnWidth, bounds().height()),
                m_ellipsisBackground);

}

KDSize FunctionCell::minimalSizeForOptimalDisplay() const {
  KDCoordinate expressionHeight =
      m_expressionView.minimalSizeForOptimalDisplay().height();
  KDCoordinate minimalHeight = k_margin + expressionHeight + k_margin;
  if (displayPlotType()) {
    KDCoordinate messageHeight = m_messageTextView.minimalSizeForOptimalDisplay().height();
    minimalHeight += k_messageMargin + messageHeight;
  }
  KDCoordinate parameterHeight =
      m_ellipsisView.minimalSizeForOptimalDisplay().height();
  if (parameterHeight > minimalHeight) {
    // Leave enough height for parameter's menu elipsis.
    minimalHeight = parameterHeight;
  }
  return KDSize(bounds().width(), minimalHeight);
}

void FunctionCell::updateSubviewsBackgroundAfterChangingState() {
  KDColor defaultColor = m_even ? KDColorWhite : Palette::WallScreen;
  // If not highlighted, selectedColor is defaultColor
  KDColor selectedColor = backgroundColor();
  m_ellipsisBackground = m_parameterSelected ? selectedColor : defaultColor;
  m_expressionBackground = m_parameterSelected ? defaultColor : selectedColor;
  // Expression View and Message Text View share the same background
  m_expressionView.setBackgroundColor(m_expressionBackground);
  if (displayPlotType()) {
    m_messageTextView.setBackgroundColor(m_expressionBackground);
  }
}

void FunctionCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
    updateSubviewsBackgroundAfterChangingState();
  }
}

View * FunctionCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_expressionView;
    case 1:
      return &m_ellipsisView;
    default:
      assert(index == 2 && displayPlotType());
      return &m_messageTextView;
  }
}

void FunctionCell::layoutSubviews(bool force) {
  m_ellipsisView.setFrame(KDRect(bounds().width() - k_parametersColumnWidth, 0,
                                 k_parametersColumnWidth, bounds().height()),
                          force);
  KDCoordinate leftMargin = k_colorIndicatorThickness + k_margin;
  KDCoordinate rightMargin = k_margin + k_parametersColumnWidth;
  KDCoordinate expressionHeight =
      m_expressionView.minimalSizeForOptimalDisplay().height();
  KDCoordinate availableWidth = bounds().width() - leftMargin - rightMargin;
  m_expressionView.setFrame(
      KDRect(leftMargin, k_margin, availableWidth, expressionHeight), force);
  if (displayPlotType()) {
    KDCoordinate messageHeight = m_messageTextView.minimalSizeForOptimalDisplay().height();
    m_messageTextView.setFrame(
        KDRect(leftMargin, bounds().height() - k_margin - messageHeight,
               availableWidth, messageHeight),
        force);
  }
}

}  // namespace Graph
