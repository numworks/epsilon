#include "sequence_title_cell.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell() :
  Shared::FunctionTitleCell(Orientation::VerticalIndicator),
  m_titleTextView(k_verticalOrientationHorizontalAlignment, k_horizontalOrientationAlignment)
{
  m_titleTextView.setRightMargin(3);
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

void SequenceTitleCell::layoutSubviews(bool force) {
  if (m_orientation == Orientation::VerticalIndicator) {
    m_titleTextView.setAlignment(k_verticalOrientationHorizontalAlignment, verticalAlignment());
  }
  m_titleTextView.setFrame(subviewFrame(), force);
}

float SequenceTitleCell::verticalAlignmentGivenExpressionBaselineAndRowHeight(KDCoordinate expressionBaseline, KDCoordinate rowHeight) const {
  assert(m_orientation == Orientation::VerticalIndicator);
  Layout l = layout();
  return ((float)(expressionBaseline - l.baseline()))/((float)rowHeight-l.layoutSize().height());
}

}
