#include <poincare/layout_helper.h>
#include <poincare/matrix_layout.h>
#include <poincare/serialization_helper.h>
#include <poincare/square_bracket_pair_layout.h>

#include <algorithm>

namespace Poincare {

// SerializableNode
int MatrixLayoutNode::serialize(char *buffer, int bufferSize,
                                Preferences::PrintFloatMode floatDisplayMode,
                                int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  buffer[bufferSize - 1] = 0;
  if (bufferSize == 1) {
    return bufferSize - 1;
  }

  // Write the opening bracket
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '[');
  if (numberOfChar >= bufferSize - 1) {
    return bufferSize - 1;
  }

  /* Do not serialize the outmost lines if they are empty: compute the first and
   * last lines to serialize. */
  int minRowIndex = 0;
  bool matrixIsEmpty = true;
  for (int i = 0; i < m_numberOfRows; i++) {
    if (!isRowEmpty(i)) {
      minRowIndex = i;
      matrixIsEmpty = false;
      break;
    }
  }
  assert(m_numberOfRows > 0);
  int maxRowIndex = m_numberOfRows - 1;
  if (!matrixIsEmpty) {
    for (int i = m_numberOfRows - 1; i >= 0; i--) {
      if (!isRowEmpty(i)) {
        maxRowIndex = i;
        break;
      }
    }
  }

  // Serialize the vectors
  int maxColumnIndex =
      isEditing() ? m_numberOfColumns - 2 : m_numberOfColumns - 1;
  for (int i = minRowIndex; i <= maxRowIndex; i++) {
    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, '[');
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }

    numberOfChar += SerializationHelper::Infix(
        this, buffer + numberOfChar, bufferSize - numberOfChar,
        floatDisplayMode, numberOfSignificantDigits, ",", i * m_numberOfColumns,
        i * m_numberOfColumns + maxColumnIndex);
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }

    numberOfChar += SerializationHelper::CodePoint(
        buffer + numberOfChar, bufferSize - numberOfChar, ']');
    if (numberOfChar >= bufferSize - 1) {
      return bufferSize - 1;
    }
  }

  // Write the final closing bracket
  numberOfChar += SerializationHelper::CodePoint(
      buffer + numberOfChar, bufferSize - numberOfChar, ']');
  return std::min(numberOfChar, bufferSize - 1);
}

void MatrixLayoutNode::startEditing() {
  if (!isEditing()) {
    addEmptyRow(EmptyRectangle::Color::Gray);
    addEmptyColumn(EmptyRectangle::Color::Gray);
  }
}

void MatrixLayoutNode::stopEditing() {
  if (isEditing()) {
    deleteRowAtIndex(m_numberOfRows - 1);
    deleteColumnAtIndex(m_numberOfColumns - 1);
  }
}

// Private

bool MatrixLayoutNode::isEditing() const {
  if (numberOfChildren() == 0) {
    return false;
  }
  LayoutNode *lastChild = childAtIndex(m_numberOfRows * m_numberOfColumns - 1);
  return lastChild->isEmpty() &&
         static_cast<HorizontalLayoutNode *>(lastChild)->emptyColor() ==
             EmptyRectangle::Color::Gray;
}

KDSize MatrixLayoutNode::computeSize(KDFont::Size font) {
  return SquareBracketPairLayoutNode::SizeGivenChildSize(gridSize(font));
}

KDPoint MatrixLayoutNode::positionOfChild(LayoutNode *l, KDFont::Size font) {
  assert(indexOfChild(l) >= 0);
  return GridLayoutNode::positionOfChild(l, font).translatedBy(
      SquareBracketPairLayoutNode::ChildOffset());
}

KDCoordinate MatrixLayoutNode::computeBaseline(KDFont::Size font) {
  return GridLayoutNode::computeBaseline(font) +
         BracketPairLayoutNode::k_lineThickness;
}

void MatrixLayoutNode::render(KDContext *ctx, KDPoint p, KDGlyph::Style style) {
  KDSize s = gridSize(style.font);
  SquareBracketPairLayoutNode::RenderWithChildSize(
      true, s.height(), ctx, p, style.glyphColor, style.backgroundColor);
  KDCoordinate rightOffset =
      SquareBracketPairLayoutNode::ChildOffset().x() + s.width();
  SquareBracketPairLayoutNode::RenderWithChildSize(
      false, s.height(), ctx, p.translatedBy(KDPoint(rightOffset, 0)),
      style.glyphColor, style.backgroundColor);
}

MatrixLayout MatrixLayout::Builder(Layout l1) {
  MatrixLayout m =
      TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>({l1});
  m.setDimensions(1, 1);
  return m;
}

MatrixLayout MatrixLayout::Builder(Layout l1, Layout l2, Layout l3, Layout l4) {
  MatrixLayout m =
      TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>({l1, l2, l3, l4});
  m.setDimensions(2, 2);
  return m;
}

}  // namespace Poincare
