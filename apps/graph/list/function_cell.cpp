#include "function_cell.h"

#include <escher/palette.h>

using namespace Escher;

namespace Graph {

/* Function cell has the folowing layout :
 *  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
 * |####|  _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _                   |              |
 * |####|  |                             |                   |              |
 * |####|  | MainCell                    |                   |              |
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
 * - ExpressionCell is cropped in width, but can take significant height
 * - If inactive, both color indicator and all texts are set to gray
 */

AbstractFunctionCell::AbstractFunctionCell()
    : EvenOddCell(),
      m_messageTextView((I18n::Message)0,
                        {.style = {.glyphColor = Palette::GrayDark,
                                   .font = KDFont::Size::Small},
                         .verticalAlignment = KDGlyph::k_alignTop}),
      m_functionColor(KDColorBlack),
      m_expressionBackground(KDColorWhite),
      m_ellipsisBackground(KDColorWhite) {}

void AbstractFunctionCell::drawRect(KDContext* ctx, KDRect rect) const {
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

KDSize AbstractFunctionCell::minimalSizeForOptimalDisplay() const {
  KDCoordinate minimalHeight =
      mainCell()->minimalSizeForOptimalDisplay().height() + 2 * k_margin;
  if (displayFunctionType()) {
    KDCoordinate messageHeight =
        m_messageTextView.minimalSizeForOptimalDisplay().height();
    minimalHeight += k_messageMargin + messageHeight;
  }
  return KDSize(bounds().width(), minimalHeight);
}

bool AbstractFunctionCell::displayFunctionType() const {
  return !Poincare::Preferences::sharedPreferences->examMode()
              .forbidGraphDetails();
}

View* AbstractFunctionCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return mainCell();
    case 1:
      return &m_ellipsisView;
    default:
      assert(index == 2 && displayFunctionType());
      return &m_messageTextView;
  }
}

void AbstractFunctionCell::layoutSubviews(bool force) {
  setChildFrame(&m_ellipsisView,
                KDRect(bounds().width() - k_parametersColumnWidth, 0,
                       k_parametersColumnWidth, bounds().height()),
                force);
  KDCoordinate leftMargin = k_colorIndicatorThickness + k_margin;
  KDCoordinate rightMargin = k_margin + k_parametersColumnWidth;
  KDCoordinate availableWidth = bounds().width() - leftMargin - rightMargin;

  KDCoordinate totalMessageHeight = 0;
  if (displayFunctionType()) {
    KDCoordinate messageHeight =
        m_messageTextView.minimalSizeForOptimalDisplay().height();
    setChildFrame(
        &m_messageTextView,
        KDRect(leftMargin, bounds().height() - k_margin - messageHeight,
               availableWidth, messageHeight),
        force);
    totalMessageHeight = messageHeight + k_messageMargin;
  }

  KDCoordinate expressionHeight =
      mainCell()->minimalSizeForOptimalDisplay().height();
  KDCoordinate availableHeight =
      bounds().height() - totalMessageHeight - 2 * k_margin;
  setChildFrame(
      mainCell(),
      KDRect(leftMargin, k_margin + (availableHeight - expressionHeight) / 2,
             availableWidth, expressionHeight),
      force);
}

void FunctionCell::updateSubviewsBackgroundAfterChangingState() {
  KDColor defaultColor = m_even ? KDColorWhite : Palette::WallScreen;
  // If not highlighted, selectedColor is defaultColor
  KDColor selectedColor = backgroundColor();
  m_ellipsisBackground = m_parameterSelected ? selectedColor : defaultColor;
  m_expressionBackground = m_parameterSelected ? defaultColor : selectedColor;
  // Expression View and Message Text View share the same background
  if (displayFunctionType()) {
    m_messageTextView.setBackgroundColor(m_expressionBackground);
  }
  expressionCell()->setBackgroundColor(m_expressionBackground);
}

void FunctionCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
    updateSubviewsBackgroundAfterChangingState();
  }
}

}  // namespace Graph
