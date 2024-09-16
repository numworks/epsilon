#include <poincare/layout.h>
#include <poincare/old/piecewise_operator.h>
#include <poincare/old/serialization_helper.h>

namespace Poincare {

// SerializableNode
size_t PiecewiseOperatorLayoutNode::serialize(
    char *buffer, size_t bufferSize,
    Preferences::PrintFloatMode floatDisplayMode,
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

// OLayout

PiecewiseOperatorLayout PiecewiseOperatorLayout::Builder() {
  PoolHandle result = PoolHandle::NAryBuilder<PiecewiseOperatorLayout,
                                              PiecewiseOperatorLayoutNode>({});
  PiecewiseOperatorLayout layout =
      static_cast<PiecewiseOperatorLayout &>(result);
  layout.setDimensions(0, 2);
  return layout;
}

void PiecewiseOperatorLayout::addRow(OLayout leftLayout, OLayout rightLayout) {
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
