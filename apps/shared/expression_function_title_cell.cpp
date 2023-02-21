#include "expression_function_title_cell.h"

#include <assert.h>

using namespace Poincare;
using namespace Escher;

namespace Shared {

ExpressionFunctionTitleCell::ExpressionFunctionTitleCell(KDFont::Size font)
    : FunctionTitleCell(),
      m_titleTextView(k_alignment, k_alignment, KDColorBlack, KDColorWhite,
                      font) {}

void ExpressionFunctionTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_titleTextView.setHighlighted(highlight);
}

void ExpressionFunctionTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_titleTextView.setEven(even);
}

void ExpressionFunctionTitleCell::setColor(KDColor color) {
  Shared::FunctionTitleCell::setColor(color);
  m_titleTextView.setTextColor(color);
}

void ExpressionFunctionTitleCell::reloadCell() {
  /* When creating a new function, the layout has not yet been initialized, but
   * it is needed in layoutSubview to compute the vertical alignment. */
  if (TreeNode::IsValidIdentifier(layout().identifier())) {
    layoutSubviews();
  }
  m_titleTextView.reloadCell();
  FunctionTitleCell::reloadCell();
}

View* ExpressionFunctionTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_titleTextView;
}

void ExpressionFunctionTitleCell::layoutSubviews(bool force) {
  assert(TreeNode::IsValidIdentifier(layout().identifier()));
  m_titleTextView.setFrame(subviewFrame(), force);
}

}  // namespace Shared
