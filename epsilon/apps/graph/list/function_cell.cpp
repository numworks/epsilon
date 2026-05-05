#include "function_cell.h"

#include <apps/exam_mode_manager.h>
#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>

using namespace Escher;

namespace Graph {

/* Function cell has the following layout :
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
                        {.style = {.glyphColor = ThemeComponent::TextGrayDark,
                                   .font = KDFont::Size::Small},
                         .verticalAlignment = KDGlyph::k_alignTop}),
      m_functionColor(ThemeComponent::TextDefault),
      m_hideMessage(false) {}

void AbstractFunctionCell::drawRect(KDContext* ctx, KDRect rect) const {
  KDCoordinate ellipsisWidth = ellipsisViewWidth();
  // Draw the color indicator
  ctx->fillRect(KDRect(0, 0, k_colorIndicatorThickness, bounds().height()),
                ColorPicker::Resolve(m_functionColor));
  // Color the main background
  ctx->fillRect(
      KDRect(k_colorIndicatorThickness, 0,
             bounds().width() - k_colorIndicatorThickness - ellipsisWidth,
             bounds().height()),
      ColorPicker::Resolve(Highlight(
          backgroundColor(), isHighlighted() && expressionIsSelected())));
  // Color the ellipsis view
  ctx->fillRect(
      KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
             bounds().height()),
      ColorPicker::Resolve(Highlight(backgroundColor(),
                                     isHighlighted() && ellipsisIsSelected())));
}

KDSize AbstractFunctionCell::minimalSizeForOptimalDisplay() const {
  KDCoordinate minimalHeight =
      mainCell()->minimalSizeForOptimalDisplay().height() + 2 * k_margin;
  if (displayFunctionType()) {
    KDCoordinate messageHeight = messageTextHeight();
    minimalHeight += k_messageMargin + messageHeight;
  }
  return KDSize(bounds().width(), minimalHeight);
}

bool AbstractFunctionCell::displayFunctionType() const {
  return !ExamModeManager::ExamMode().forbidGraphDetails();
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
  KDCoordinate ellipsisWidth = ellipsisViewWidth();
  setChildFrame(&m_ellipsisView,
                KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
                       bounds().height()),
                force);
  KDCoordinate leftMargin = k_colorIndicatorThickness + k_margin;
  KDCoordinate rightMargin = k_margin + ellipsisWidth;
  KDCoordinate availableWidth = bounds().width() - leftMargin - rightMargin;

  KDCoordinate totalMessageHeight = 0;
  if (displayFunctionType()) {
    KDCoordinate messageHeight = messageTextHeight();
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

KDCoordinate AbstractFunctionCell::messageTextHeight() const {
  if (m_hideMessage) {
    return 0;
  }
  return m_messageTextView.minimalSizeForOptimalDisplay().height();
}

ThemeComponent AbstractFunctionCell::backgroundColor() const {
  return m_even ? ThemeComponent::BackgroundWhite : ThemeComponent::WallScreen;
}

void AbstractFunctionCell::setHighlighted(bool highlight) {
  /* TODO: the order of highlight is a bit counter-intuitive. Simplify the
   * logic so that the EvenOddCell highlight can be set first, then the subview
   * highlights. */
  mainCell()->setHighlighted(highlight && expressionIsSelected());
  m_messageTextView.setHighlighted(highlight && expressionIsSelected());
  EvenOddCell::setHighlighted(highlight);
}

void FunctionCell::updateSubviewsBackgroundAfterChangingState() {
  if (displayFunctionType()) {
    m_messageTextView.setBackgroundColor(backgroundColor());
  }
  expressionCell()->setBackgroundColor(backgroundColor());
}

void FunctionCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
  }
}

}  // namespace Graph
