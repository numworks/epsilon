#include "sequence_cell.h"

using namespace Escher;

namespace Sequence {

KDSize AbstractSequenceCell::minimalSizeForOptimalDisplay() const {
  KDCoordinate height =
      std::max(m_sequenceTitleCell.minimalSizeForOptimalDisplay().height(),
               expressionCell()->minimalSizeForOptimalDisplay().height());
  return KDSize(bounds().width(), height);
}

View* AbstractSequenceCell::subviewAtIndex(int index) {
  switch (index) {
    case 0:
      return &m_sequenceTitleCell;
    default:
      assert(index == 1);
      return expressionCell();
  }
}

void AbstractSequenceCell::drawRect(KDContext* ctx, KDRect rect) const {
  // Color the main background
  ctx->fillRect(bounds(), m_expressionBackground);
}

void AbstractSequenceCell::layoutSubviews(bool force) {
  m_sequenceTitleCell.setFrame(
      KDRect(0, 0, k_titlesColmunWidth, bounds().height()), force);
  expressionCell()->setFrame(
      KDRect(k_titlesColmunWidth, 0, bounds().width() - k_titlesColmunWidth,
             bounds().height()),
      force);
}

void AbstractSequenceCell::setEven(bool even) {
  m_sequenceTitleCell.setEven(even);
  EvenOddCell::setEven(even);
}

void SequenceCell::updateSubviewsBackgroundAfterChangingState() {
  KDColor defaultColor = m_even ? KDColorWhite : Palette::WallScreen;
  // If not highlighted, selectedColor is defaultColor
  KDColor selectedColor = backgroundColor();
  m_sequenceTitleCell.setHighlighted(isHighlighted() && m_parameterSelected);
  m_expressionBackground = m_parameterSelected ? defaultColor : selectedColor;
  expressionCell()->setHighlighted(isHighlighted() && !m_parameterSelected);
  expressionCell()->setBackgroundColor(m_expressionBackground);
}

void AbstractSequenceCell::setParameterSelected(bool selected) {
  if (selected != m_parameterSelected) {
    m_parameterSelected = selected;
    updateSubviewsBackgroundAfterChangingState();
  }
}

void SequenceCell::setEven(bool even) {
  m_expressionCell.setEven(even);
  AbstractSequenceCell::setEven(even);
}

}  // namespace Sequence
