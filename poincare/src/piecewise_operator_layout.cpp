#include <poincare/piecewise_operator_layout.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/empty_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/piecewise_operator.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

// SerializableNode
int PiecewiseOperatorLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  assert(numberOfColumns() == 2);
  int lastChildIndex = numberOfChildren() - 1;
  if (isEditing()) {
    lastChildIndex -= 2;
  }
  if (childAtIndex(lastChildIndex)->isEmpty()) {
    lastChildIndex--;
  }
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PiecewiseOperator::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

void PiecewiseOperatorLayoutNode::startEditing() {
  if (!isEditing()) {
    // Show last condition if empty
    makeLastConditionVisibleIfEmpty(true);
    addEmptyRow(EmptyLayoutNode::Color::Gray);
  }
}

void PiecewiseOperatorLayoutNode::stopEditing() {
  if (isEditing()) {
    deleteRowAtIndex(m_numberOfRows - 1);
    // Hide last condition if empty
    makeLastConditionVisibleIfEmpty(false);
  }
}

// Private

bool PiecewiseOperatorLayoutNode::isEditing() const {
  assert(numberOfColumns() == 2);
  LayoutNode * firstElementOfLastRow = const_cast<PiecewiseOperatorLayoutNode *>(this)->childAtIndex(numberOfChildren() - 2);
  return firstElementOfLastRow->type() == Type::EmptyLayout && (static_cast<EmptyLayoutNode *>(firstElementOfLastRow))->color() == EmptyLayoutNode::Color::Gray;
}

KDSize PiecewiseOperatorLayoutNode::computeSize(KDFont::Size font) {
  assert(numberOfColumns() == 2);
  KDSize sizeWithoutBrace = gridSize(font);
  if (numberOfChildren() == 2 && !isEditing() && childAtIndex(1)->type() == Type::EmptyLayout) {
    // If there is only 1 row and the condition is empty, shrink the size
    sizeWithoutBrace = KDSize(columnWidth(0, font), sizeWithoutBrace.height());
  }
  KDSize sizeWithBrace = KDSize(
      sizeWithoutBrace.width() + 2 * CurlyBraceLayoutNode::k_curlyBraceWidth, // Add a right margin of size k_curlyBraceWidth
      CurlyBraceLayoutNode::HeightGivenChildHeight(sizeWithoutBrace.height()));
  return sizeWithBrace;
}

KDPoint PiecewiseOperatorLayoutNode::positionOfChild(LayoutNode * l, KDFont::Size font) {
  assert(indexOfChild(l) >= 0);
  return GridLayoutNode::positionOfChild(l, font).translatedBy(KDPoint(KDPoint(CurlyBraceLayoutNode::k_curlyBraceWidth, CurlyBraceLayoutNode::k_lineThickness)));
}

KDCoordinate PiecewiseOperatorLayoutNode::computeBaseline(KDFont::Size font) {
  return GridLayoutNode::computeBaseline(font) + CurlyBraceLayoutNode::k_lineThickness;
}

void PiecewiseOperatorLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart, Layout * selectionEnd, KDColor selectionColor) {
  assert(numberOfColumns() == 2);
  bool cursorIsInsideOperator = isEditing();

  /* Set the right color for the condition if empty.
   * The last condition must be grayed if empty.
   * Other conditions are yellow if empty. */
  int lastRealRowIndex = m_numberOfRows - 1 - static_cast<int>(cursorIsInsideOperator);
  LayoutNode * lastRealCondition = childAtIndex(lastRealRowIndex * 2 + 1);
  if (lastRealCondition->type() == Type::EmptyLayout) {
    static_cast<EmptyLayoutNode *>(lastRealCondition)->setColor(EmptyLayoutNode::Color::Gray);
  }
  if (numberOfChildren() > 2 + 2 * static_cast<int>(cursorIsInsideOperator)) {
    LayoutNode * conditionAboveLast = childAtIndex(lastRealRowIndex * 2 - 1);
    if (conditionAboveLast->type() == Type::EmptyLayout) {
      static_cast<EmptyLayoutNode *>(conditionAboveLast)->setColor(EmptyLayoutNode::Color::Yellow);
    }
  }

  // Draw the grid and the {
  CurlyBraceLayoutNode::RenderWithChildHeight(true, gridSize(font).height(), ctx, p, expressionColor, backgroundColor);

  // Draw the commas
  KDCoordinate commaAbscissa = CurlyBraceLayoutNode::k_curlyBraceWidth + columnWidth(0, font) + k_gridEntryMargin;
  for (int i = 0; i < numberOfRows(); i++) {
    LayoutNode * leftChild = childAtIndex(i * 2);
    LayoutNode * rightChild = childAtIndex(1 + i * 2);
    if (!cursorIsInsideOperator && i == numberOfRows() - 1 && rightChild->type() == Type::EmptyLayout) {
      // Last empty condition should be invisible when out of the layout
      assert(static_cast<EmptyLayoutNode *>(rightChild)->isVisible() == false);
      continue; // Do not draw the comma
    }
    KDPoint leftChildPosition = positionOfChild(leftChild, font);
    KDPoint commaPosition = KDPoint(commaAbscissa, leftChildPosition.y() + leftChild->baseline(font) - KDFont::GlyphHeight(font) / 2);
    bool drawInGray =  rightChild->type() == Type::EmptyLayout && static_cast<EmptyLayoutNode *>(rightChild)->color() == EmptyLayoutNode::Color::Gray;
    ctx->drawString(",", commaPosition.translatedBy(p), font, drawInGray ? Escher::Palette::GrayDark : expressionColor, backgroundColor);
  }
}

void PiecewiseOperatorLayoutNode::makeLastConditionVisibleIfEmpty(bool visible) {
  assert(numberOfColumns() == 2);
  LayoutNode * lastCondition = childAtIndex(numberOfChildren() - 1);
  if (lastCondition->type() == Type::EmptyLayout) {
    static_cast<EmptyLayoutNode *>(lastCondition)->setVisible(visible);
  }
}

// Layout

PiecewiseOperatorLayout PiecewiseOperatorLayout::EmptyPiecewiseOperatorBuilder() {
  PiecewiseOperatorLayout result = TreeHandle::NAryBuilder<PiecewiseOperatorLayout, PiecewiseOperatorLayoutNode>({
    EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow),
    EmptyLayout::Builder(EmptyLayoutNode::Color::Gray),
    EmptyLayout::Builder(EmptyLayoutNode::Color::Gray),
    EmptyLayout::Builder(EmptyLayoutNode::Color::Gray)
    });
  result.setDimensions(2, 2);
  return result;
}

PiecewiseOperatorLayout PiecewiseOperatorLayout::Builder() {
  return TreeHandle::NAryBuilder<PiecewiseOperatorLayout, PiecewiseOperatorLayoutNode>({});
}

void PiecewiseOperatorLayout::addRow(Layout leftLayout, Layout rightLayout) {
  if (rightLayout.isUninitialized()) {
    rightLayout = EmptyLayout::Builder(EmptyLayoutNode::Color::Gray);
    static_cast<EmptyLayout&>(rightLayout).setVisible(false);
  }
  assert(!leftLayout.isUninitialized());
  addChildAtIndexInPlace(leftLayout, numberOfChildren(), numberOfChildren());
  addChildAtIndexInPlace(rightLayout, numberOfChildren(), numberOfChildren());
}

}
