#include "sequence_title_cell.h"
#include <assert.h>

using namespace Poincare;
using namespace Escher;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell(KDFont::Size font) :
  Shared::FunctionTitleCell(),
  m_titleTextView(k_horizontalOrientationAlignment, k_horizontalOrientationAlignment, KDColorBlack, KDColorWhite, font)
{
  m_titleTextView.setRightMargin(3);
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

void SequenceTitleCell::reloadCell() {
  /* When creating a new sequence, the layout has not yet been initialized, but
   * it is needed in layoutSubview to compute the vertical alignment. */
  if (TreeNode::IsValidIdentifier(layout().identifier())) {
    layoutSubviews();
  }
  m_titleTextView.reloadCell();
  FunctionTitleCell::reloadCell();
}

int SequenceTitleCell::numberOfSubviews() const {
  return 1;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_titleTextView;
}

void SequenceTitleCell::layoutSubviews(bool force) {
  assert(TreeNode::IsValidIdentifier(layout().identifier()));
  m_titleTextView.setFrame(subviewFrame(), force);
}

}
