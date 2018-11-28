#include "sequence_title_cell.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell() :
  Shared::FunctionTitleCell(Orientation::VerticalIndicator),
  m_titleTextView(k_verticalOrientationHorizontalAlignment, k_horizontalOrientationAlignment)
{
}

void SequenceTitleCell::setOrientation(Orientation orientation) {
  if (orientation == Orientation::VerticalIndicator) {
    /* We do not care here about the vertical alignment, it will be set properly
     * in layoutSubviews */
    m_titleTextView.setAlignment(k_verticalOrientationHorizontalAlignment, k_verticalOrientationHorizontalAlignment);
  } else {
    m_titleTextView.setAlignment(k_horizontalOrientationAlignment, k_horizontalOrientationAlignment);
  }
  FunctionTitleCell::setOrientation(orientation);
}

void SequenceTitleCell::setLayout(Poincare::Layout layout) {
  m_titleTextView.setLayout(layout);
}

void SequenceTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_titleTextView.setHighlighted(highlight);
}

void SequenceTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_titleTextView.setEven(even);
}

void SequenceTitleCell::setColor(KDColor color) {
  Shared::FunctionTitleCell::setColor(color);
  m_titleTextView.setTextColor(color);
}

int SequenceTitleCell::numberOfSubviews() const {
  return 1;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_titleTextView;
}

void SequenceTitleCell::layoutSubviews() {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator) {
    KDCoordinate h = bounds().height()-k_separatorThickness;
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness, h);
    /* We try to align the text so that the equal is vertically centered in the
     * cell. This makes the title cell and the definition cell baselines be
     * approximately at the same level for basic sequences definitions (un = 1,
     * un=1/2, ...). */
    float verticalAlignment = 0.5f + 20.0f/((float)h); // 20.0f is a magic value
    m_titleTextView.setAlignment(k_verticalOrientationHorizontalAlignment, verticalAlignment);
  }
  m_titleTextView.setFrame(textFrame);
}

}
