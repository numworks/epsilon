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
  if (hasGraySquares()) {
    lastChildIndex -= 2;
  }
  if (childAtIndex(lastChildIndex)->isEmpty()) {
    lastChildIndex--;
  }
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, PiecewiseOperator::s_functionHelper.aliasesList().mainAlias(), SerializationHelper::ParenthesisType::Classic, lastChildIndex);
}

// Private

KDSize PiecewiseOperatorLayoutNode::computeSize(KDFont::Size font) {
  assert(numberOfColumns() == 2);
  KDSize sizeWithoutBrace = gridSize(font);
  if (!hasGraySquares()) {
    /* Cursor is outside of the operator.
     * Check if all the right cells are empty. If it's the case, set the width
     * to the width of the left column. */
    bool allRightChildrenAreEmpty = true;
    for (int i = 0; i < numberOfRows(); i++) {
      if (childAtIndex(1 + i * 2)->type() != Type::EmptyLayout) {
        allRightChildrenAreEmpty = false;
        break;
      }
    }
    if (allRightChildrenAreEmpty) {
      sizeWithoutBrace = KDSize(columnWidth(0, font), sizeWithoutBrace.height());
    }
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
  // Draw the grid and the {
  CurlyBraceLayoutNode::RenderWithChildHeight(true, gridSize(font).height(), ctx, p, expressionColor, backgroundColor);

  bool cursorIsOutsideOfOperator = !hasGraySquares();

  // Draw the commas and hide the empty layouts if needed
  assert(numberOfColumns() == 2);
  KDCoordinate commaAbscissa = CurlyBraceLayoutNode::k_curlyBraceWidth + columnWidth(0, font) + k_gridEntryMargin;
  for (int i = 0; i < numberOfRows(); i++) {
    LayoutNode * leftChild = childAtIndex(i * 2);
    LayoutNode * rightChild = childAtIndex(1 + i * 2);
    if (rightChild->type() == Type::EmptyLayout) {
      if (cursorIsOutsideOfOperator) {
        static_cast<EmptyLayoutNode *>(rightChild)->setVisible(false);
        continue; // Do not draw the comma
      } else {
        // TODO: Find a way to properly show the empty layouts again
        //static_cast<EmptyLayoutNode *>(rightChild)->setVisible(true);
      }
    }
    KDPoint leftChildPosition = positionOfChild(leftChild, font);
    KDPoint commaPosition = KDPoint(commaAbscissa, leftChildPosition.y() + leftChild->baseline(font) - KDFont::GlyphHeight(font) / 2);
    bool drawInGray =  rightChild->type() == Type::EmptyLayout && static_cast<EmptyLayoutNode *>(rightChild)->color() == EmptyLayoutNode::Color::Gray;
    ctx->drawString(",", commaPosition.translatedBy(p), font, drawInGray ? Escher::Palette::GrayDark : expressionColor, backgroundColor);
  }
}

PiecewiseOperatorLayout PiecewiseOperatorLayout::EmptyPiecewiseOperatorBuilder() {
  PiecewiseOperatorLayout result = TreeHandle::NAryBuilder<PiecewiseOperatorLayout, PiecewiseOperatorLayoutNode>({
    EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow),
    EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow),
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
    rightLayout = EmptyLayout::Builder(EmptyLayoutNode::Color::Yellow);
  }
  assert(!leftLayout.isUninitialized());
  addChildAtIndexInPlace(leftLayout, numberOfChildren(), numberOfChildren());
  addChildAtIndexInPlace(rightLayout, numberOfChildren(), numberOfChildren());
}

}
