#include "sequence_cell.h"

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
  ctx->fillRect(bounds(), m_expressionBackground);
  // Draw the color indicator
  ctx->fillRect(
      KDRect(0, 0, k_verticalColorIndicatorThickness, bounds().height()),
      m_functionColor);
  // Color the ellipsis view
  KDCoordinate ellipsisWidth = displayEllipsis() ? k_ellipsisWidth : 0;
  ctx->fillRect(KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
                       bounds().height()),
                m_ellipsisBackground);
}

void AbstractSequenceCell::layoutSubviews(bool force) {
  KDCoordinate ellipsisWidth = displayEllipsis() ? k_ellipsisWidth : 0;
  setChildFrame(
      mainCell(),
      KDRect(k_verticalColorIndicatorThickness + k_expressionMargin, 0,
             bounds().width() - k_verticalColorIndicatorThickness -
                 k_expressionMargin - ellipsisWidth,
             bounds().height()),
      force);
  setChildFrame(&m_ellipsisView,
                KDRect(bounds().width() - ellipsisWidth, 0, ellipsisWidth,
                       bounds().height()),
                force);
}

void AbstractSequenceCell::setEven(bool even) { EvenOddCell::setEven(even); }

void SequenceCell::updateSubviewsBackgroundAfterChangingState() {
  KDColor defaultColor = m_even ? KDColorWhite : Palette::WallScreen;
  // If not highlighted, selectedColor is defaultColor
  KDColor selectedColor = backgroundColor();
  m_expressionBackground = m_parameterSelected ? defaultColor : selectedColor;
  m_ellipsisBackground = m_parameterSelected ? selectedColor : defaultColor;
  expressionCell()->setHighlighted(isHighlighted() && !m_parameterSelected);
}

void AbstractSequenceCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
    updateSubviewsBackgroundAfterChangingState();
  }
}

void SequenceCell::setEven(bool even) {
  expressionCell()->setEven(even);
  AbstractSequenceCell::setEven(even);
}

}  // namespace Sequence
