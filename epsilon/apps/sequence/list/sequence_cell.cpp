#include "sequence_cell.h"

#include <escher/theme/color_picker.h>
#include <escher/theme/highlight.h>

using namespace Escher;

namespace Sequence {

View* AbstractSequenceCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return mainCell();
    default:
      assert(index == 1);
      return &m_ellipsisView;
  }
}

void AbstractSequenceCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the main background
  ctx->fillRect(bounds(), ColorPicker::Resolve(Highlight(
                              backgroundColor(), expressionIsHighlighted())));
  // Draw the color indicator
  ctx->fillRect(
      KDRect(0, 0, k_verticalColorIndicatorThickness, bounds().height()),
      ColorPicker::Resolve(m_functionColor));
  // Color the ellipsis view
  KDCoordinate ellipsisWidth = displayEllipsis() ? k_ellipsisWidth : 0;
  ctx->fillRect(KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
                       bounds().height()),
                ColorPicker::Resolve(
                    Highlight(backgroundColor(), ellipsisIsHighlighted())));
}

void AbstractSequenceCell::layoutSubviews(bool force) {
  KDCoordinate ellipsisWidth = displayEllipsis() ? k_ellipsisWidth : 0;
  setChildFrame(mainCell(),
                KDRect(k_verticalColorIndicatorThickness + k_margin, 0,
                       bounds().width() - k_verticalColorIndicatorThickness -
                           k_margin - ellipsisWidth,
                       bounds().height()),
                force);
  setChildFrame(&m_ellipsisView,
                KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
                       bounds().height()),
                force);
}

void AbstractSequenceCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  mainCell()->setHighlighted(expressionIsHighlighted());
  m_ellipsisView.setHighlighted(ellipsisIsHighlighted());
}

void AbstractSequenceCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
  }
}

void SequenceCell::setEven(bool even) {
  expressionCell()->setEven(even);
  AbstractSequenceCell::setEven(even);
  expressionCell()->setBackgroundColor(backgroundColor());
}

}  // namespace Sequence
