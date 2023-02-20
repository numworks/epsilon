#include <poincare/vertical_offset_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parenthesis_layout.h>
#include <poincare/serialization_helper.h>
#include <string.h>
#include <assert.h>
#include <algorithm>

namespace Poincare {

LayoutNode::DeletionMethod VerticalOffsetLayoutNode::deletionMethodForCursorLeftOfChild(int childIndex) const {
  assert(childIndex == 0 || childIndex == k_outsideIndex);
  return childIndex == 0 && childAtIndex(0)->isEmpty() ? DeletionMethod::DeleteLayout : DeletionMethod::MoveLeft;
}

int VerticalOffsetLayoutNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  if (verticalPosition() == VerticalPosition::Subscript || horizontalPosition() == HorizontalPosition::Prefix) {
    if (bufferSize == 0) {
      return bufferSize-1;
    }
    buffer[bufferSize-1] = 0;
    if (bufferSize == 1) {
      return bufferSize-1;
    }

    /* If the layout is a subscript or a prefix, write "\x14{indice\x14}".
     * System braces are used to avoid confusion with lists. */
    int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, UCodePointSystem);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '{');
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(buffer + numberOfChar, bufferSize - numberOfChar, floatDisplayMode, numberOfSignificantDigits);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }

    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, UCodePointSystem);
    if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
    numberOfChar += SerializationHelper::CodePoint(buffer + numberOfChar, bufferSize - numberOfChar, '}');
    return std::min(numberOfChar, bufferSize-1);
  }

  assert(verticalPosition() == VerticalPosition::Superscript);
  // If the layout is a superscript, write: '^' 'System(' indice 'System)'
  int numberOfChar = SerializationHelper::CodePoint(buffer, bufferSize, '^');
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, UCodePointLeftSystemParenthesis);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += const_cast<VerticalOffsetLayoutNode *>(this)->indiceLayout()->serialize(buffer+numberOfChar, bufferSize-numberOfChar, floatDisplayMode, numberOfSignificantDigits);
  if (numberOfChar >= bufferSize-1) { return bufferSize-1; }
  numberOfChar += SerializationHelper::CodePoint(buffer+numberOfChar, bufferSize-numberOfChar, UCodePointRightSystemParenthesis);
  return std::min(numberOfChar, bufferSize-1);
}

int VerticalOffsetLayoutNode::indexAfterVerticalCursorMove(OMG::VerticalDirection direction, int currentIndex, PositionInLayout positionAtCurrentIndex, bool * shouldRedrawLayout) {
  if (currentIndex == k_outsideIndex && ((direction.isUp() && m_verticalPosition == VerticalPosition::Superscript) || (direction.isDown() && m_verticalPosition == VerticalPosition::Subscript))) {
    return 0;
  }
  if (currentIndex == 0 && ((direction.isDown() && m_verticalPosition == VerticalPosition::Superscript) || (direction.isUp() && m_verticalPosition == VerticalPosition::Subscript)) && positionAtCurrentIndex != PositionInLayout::Middle) {
    return k_outsideIndex;
  }
  return k_cantMoveIndex;
}

bool VerticalOffsetLayoutNode::setEmptyVisibility(EmptyRectangle::State state) {
  if (m_emptyBaseVisibility == state) {
    return false;
  }
  m_emptyBaseVisibility = state;
  return baseLayout() == nullptr; // Return true if empty is displayed
}

KDSize VerticalOffsetLayoutNode::computeSize(KDFont::Size font) {
  KDSize indiceSize = indiceLayout()->layoutSize(font);
  KDCoordinate width = indiceSize.width();
  if (verticalPosition() == VerticalPosition::Superscript) {
    LayoutNode * parentNode = parent();
    assert(parentNode != nullptr);
    assert(parentNode->isHorizontal());
    int idxInParent = parentNode->indexOfChild(this);
    if (idxInParent < parentNode->numberOfChildren() - 1 && parentNode->childAtIndex(idxInParent + 1)->hasUpperLeftIndex()) {
      width += k_separationMargin;
    }
  }
  if (m_emptyBaseVisibility == EmptyRectangle::State::Visible && baseLayout() == nullptr) {
    width += EmptyRectangle::RectangleSize(font).width();
  }
  KDCoordinate height = baseSize(font).height() - k_indiceHeight + indiceLayout()->layoutSize(font).height();
  return KDSize(width, height);
}

KDCoordinate VerticalOffsetLayoutNode::computeBaseline(KDFont::Size font) {
  if (verticalPosition() == VerticalPosition::Subscript) {
    return baseBaseline(font);
  }
  assert(verticalPosition() == VerticalPosition::Superscript);
  return indiceLayout()->layoutSize(font).height() - k_indiceHeight + baseBaseline(font);
}

KDPoint VerticalOffsetLayoutNode::positionOfChild(LayoutNode * child, KDFont::Size font) {
  assert(child == indiceLayout());
  KDCoordinate shiftForEmpty = baseLayout() == nullptr && horizontalPosition() == HorizontalPosition::Suffix ? baseSize(font).width() : 0;
  if (verticalPosition() == VerticalPosition::Superscript) {
    return KDPoint(shiftForEmpty, 0);
  }
  assert(verticalPosition() == VerticalPosition::Subscript);
  return KDPoint(shiftForEmpty, baseSize(font).height() - k_indiceHeight);
}

LayoutNode * VerticalOffsetLayoutNode::baseLayout() {
  LayoutNode * parentNode = parent();
  assert(parentNode != nullptr);
  if (parentNode->type() != Type::HorizontalLayout) {
    return nullptr;
  }
  int idxInParent = parentNode->indexOfChild(this);
  assert(idxInParent >= 0);
  int baseIndex = idxInParent + baseOffsetInParent();
  if (baseIndex < 0 || baseIndex >= parent()->numberOfChildren()) {
    return nullptr;
  }
  LayoutNode * result = parentNode->childAtIndex(baseIndex);
  if (result->type() == Type::VerticalOffsetLayout && static_cast<VerticalOffsetLayoutNode *>(result)->horizontalPosition() != horizontalPosition()) {
    /* If two vertical offset layouts, one prefix and one suffix, are next to
     * each other, the size of each one depends on the other one so thay can't
     * rely on their base to draw themselves. */
    return nullptr;
  }
  return result;
}

KDSize VerticalOffsetLayoutNode::baseSize(KDFont::Size font) {
  return baseLayout() ? baseLayout()->layoutSize(font) : (m_emptyBaseVisibility == EmptyRectangle::State::Visible ? EmptyRectangle::RectangleSize(font) : KDSize(0, EmptyRectangle::RectangleSize(font).height()));
}

KDCoordinate VerticalOffsetLayoutNode::baseBaseline(KDFont::Size font) {
  return baseLayout() ? baseLayout()->baseline(font) : EmptyRectangle::RectangleBaseLine(font);
}

bool VerticalOffsetLayoutNode::protectedIsIdenticalTo(Layout l) {
  assert(l.type() == Type::VerticalOffsetLayout);
  VerticalOffsetLayoutNode * n = static_cast<VerticalOffsetLayoutNode *>(l.node());
  return verticalPosition() == n->verticalPosition() && horizontalPosition() == n->horizontalPosition() && LayoutNode::protectedIsIdenticalTo(l);
}

void VerticalOffsetLayoutNode::render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  if (baseLayout() || m_emptyBaseVisibility == EmptyRectangle::State::Hidden) {
    return;
  }
  /* FIXME: If a prefix and a suffix vertical offset are consecutive, they
   * will both draw an empty rectangle, when only one should be displayed.
   * This is a fringe case though. */
  KDCoordinate emptyRectangleHorizontalOrigin;
  if (horizontalPosition() == HorizontalPosition::Suffix) {
    emptyRectangleHorizontalOrigin = 0;
  } else {
    assert(horizontalPosition() == HorizontalPosition::Prefix);
    KDCoordinate totalWidth = layoutSize(font).width();
    emptyRectangleHorizontalOrigin = totalWidth - EmptyRectangle::RectangleSize(font).width();
  }
  KDPoint emptyRectangleOrigin = p.translatedBy(KDPoint(emptyRectangleHorizontalOrigin, baseline(font) - baseBaseline(font)));
  EmptyRectangle::DrawEmptyRectangle(ctx, emptyRectangleOrigin, font, EmptyRectangle::Color::Yellow);
}

VerticalOffsetLayout VerticalOffsetLayout::Builder(Layout l, VerticalOffsetLayoutNode::VerticalPosition verticalPosition, VerticalOffsetLayoutNode::HorizontalPosition horizontalPosition) {
  void * bufferNode = TreePool::sharedPool->alloc(sizeof(VerticalOffsetLayoutNode));
  VerticalOffsetLayoutNode * node = new (bufferNode) VerticalOffsetLayoutNode(verticalPosition, horizontalPosition);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  h.replaceChildAtIndexInPlace(0, l);
  return static_cast<VerticalOffsetLayout &>(h);
}

}
