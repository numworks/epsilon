#include <poincare/bracket_pair_layout.h>
#include <poincare/curly_brace_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/piecewise_operator.h>
#include <poincare/piecewise_operator_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

// SerializableNode
int PiecewiseOperatorLayoutNode::serialize(
    char *buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode,
    int numberOfSignificantDigits) const {
  assert(numberOfColumns() == 2);
  int lastChildIndex = numberOfChildren() - 1;
  if (isEditing()) {
    lastChildIndex -= 2;
  }
  if (childAtIndex(lastChildIndex)->isEmpty()) {
    lastChildIndex--;
  }
  return SerializationHelper::Prefix(
      this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits,
      PiecewiseOperator::s_functionHelper.aliasesList().mainAlias(),
      SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

void PiecewiseOperatorLayoutNode::startEditing() {
  if (!isEditing()) {
    // Show last condition if empty
    makeLastConditionVisibleIfEmpty(true);
    addEmptyRow(EmptyRectangle::Color::Gray);
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
  LayoutNode *firstElementOfLastRow = childAtIndex(numberOfChildren() - 2);
  return firstElementOfLastRow->isEmpty() &&
         (static_cast<HorizontalLayoutNode *>(firstElementOfLastRow))
                 ->emptyColor() == EmptyRectangle::Color::Gray;
}

KDSize PiecewiseOperatorLayoutNode::computeSize(KDFont::Size font) {
  assert(numberOfColumns() == 2);
  KDSize sizeWithoutBrace = gridSize(font);
  if (numberOfChildren() == 2 && !isEditing() && childAtIndex(1)->isEmpty()) {
    // If there is only 1 row and the condition is empty, shrink the size
    sizeWithoutBrace = KDSize(columnWidth(0, font), sizeWithoutBrace.height());
  }
  KDSize sizeWithBrace = KDSize(
      sizeWithoutBrace.width() +
          2 * CurlyBraceLayoutNode::k_curlyBraceWidth,  // Add a right margin of
                                                        // size
                                                        // k_curlyBraceWidth
      CurlyBraceLayoutNode::HeightGivenChildHeight(sizeWithoutBrace.height()));
  return sizeWithBrace;
}

KDPoint PiecewiseOperatorLayoutNode::positionOfChild(LayoutNode *l,
                                                     KDFont::Size font) {
  assert(indexOfChild(l) >= 0);
  return GridLayoutNode::positionOfChild(l, font).translatedBy(
      KDPoint(CurlyBraceLayoutNode::k_curlyBraceWidth,
              CurlyBraceLayoutNode::k_lineThickness));
}

KDCoordinate PiecewiseOperatorLayoutNode::computeBaseline(KDFont::Size font) {
  return GridLayoutNode::computeBaseline(font) +
         CurlyBraceLayoutNode::k_lineThickness;
}

void PiecewiseOperatorLayoutNode::render(KDContext *ctx, KDPoint p,
                                         KDFont::Size font,
                                         KDColor expressionColor,
                                         KDColor backgroundColor) {
  assert(numberOfColumns() == 2);
  bool cursorIsInsideOperator = isEditing();

  /* Set the right color for the condition if empty.
   * The last condition must be grayed if empty.
   * Other conditions are yellow if empty.
   * Every color should be already correctly set except for the last
   * condition which is yellow instead of gray, and the penultimate
   * condition which could have been previously set to gray here and should
   * be set to yellow. */
  int lastRealRowIndex =
      m_numberOfRows - 1 - static_cast<int>(cursorIsInsideOperator);
  LayoutNode *lastRealCondition = childAtIndex(lastRealRowIndex * 2 + 1);
  if (lastRealCondition->isEmpty()) {
    static_cast<HorizontalLayoutNode *>(lastRealCondition)
        ->setEmptyColor(EmptyRectangle::Color::Gray);
  }
  if (numberOfChildren() > 2 + 2 * static_cast<int>(cursorIsInsideOperator)) {
    LayoutNode *conditionAboveLast = childAtIndex(lastRealRowIndex * 2 - 1);
    if (conditionAboveLast->isEmpty()) {
      static_cast<HorizontalLayoutNode *>(conditionAboveLast)
          ->setEmptyColor(EmptyRectangle::Color::Gray);
    }
  }

  // Draw the grid and the {
  CurlyBraceLayoutNode::RenderWithChildHeight(
      true, gridSize(font).height(), ctx, p, expressionColor, backgroundColor);

  // Draw the commas
  KDCoordinate commaAbscissa = CurlyBraceLayoutNode::k_curlyBraceWidth +
                               columnWidth(0, font) + k_gridEntryMargin;
  for (int i = 0; i < numberOfRows(); i++) {
    LayoutNode *leftChild = childAtIndex(i * 2);
    LayoutNode *rightChild = childAtIndex(1 + i * 2);
    if (!cursorIsInsideOperator && i == numberOfRows() - 1 &&
        rightChild->isEmpty()) {
      // Last empty condition should be invisible when out of the layout
      assert(
          static_cast<HorizontalLayoutNode *>(rightChild)->emptyVisibility() ==
          EmptyRectangle::State::Hidden);
      continue;  // Do not draw the comma
    }
    KDPoint leftChildPosition = positionOfChild(leftChild, font);
    KDPoint commaPosition = KDPoint(
        commaAbscissa, leftChildPosition.y() + leftChild->baseline(font) -
                           KDFont::GlyphHeight(font) / 2);
    bool drawInGray =
        rightChild->isEmpty() &&
        static_cast<HorizontalLayoutNode *>(rightChild)->emptyColor() ==
            EmptyRectangle::Color::Gray;
    ctx->drawString(",", commaPosition.translatedBy(p), font,
                    drawInGray ? Escher::Palette::GrayDark : expressionColor,
                    backgroundColor);
  }
}

void PiecewiseOperatorLayoutNode::makeLastConditionVisibleIfEmpty(
    bool visible) {
  assert(numberOfColumns() == 2);
  LayoutNode *lastCondition = childAtIndex(numberOfChildren() - 1);
  if (lastCondition->isHorizontal()) {
    static_cast<HorizontalLayoutNode *>(lastCondition)
        ->setEmptyVisibility(visible ? EmptyRectangle::State::Visible
                                     : EmptyRectangle::State::Hidden);
  }
}

// Layout

PiecewiseOperatorLayout PiecewiseOperatorLayout::Builder() {
  TreeHandle result = TreeHandle::NAryBuilder<PiecewiseOperatorLayout,
                                              PiecewiseOperatorLayoutNode>({});
  PiecewiseOperatorLayout layout =
      static_cast<PiecewiseOperatorLayout &>(result);
  layout.setDimensions(0, 2);
  return layout;
}

void PiecewiseOperatorLayout::addRow(Layout leftLayout, Layout rightLayout) {
  assert(numberOfColumns() == 2);
  if (rightLayout.isUninitialized()) {
    rightLayout = HorizontalLayout::Builder();
    static_cast<HorizontalLayout &>(rightLayout)
        .setEmptyColor(EmptyRectangle::Color::Gray);
    static_cast<HorizontalLayout &>(rightLayout)
        .setEmptyVisibility(EmptyRectangle::State::Hidden);
  }
  int nRows = numberOfRows();
  assert(!leftLayout.isUninitialized());
  int nChildren = numberOfChildren();
  addChildAtIndexInPlace(leftLayout, nChildren, nChildren);
  nChildren = numberOfChildren();
  addChildAtIndexInPlace(rightLayout, nChildren, nChildren);
  // Array::didChangeNumberOfChildren will set the number of rows back to 1
  setDimensions(nRows + 1, 2);
}

}  // namespace Poincare
