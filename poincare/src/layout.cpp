#include <poincare/layout.h>
#include <poincare/autocompleted_bracket_pair_layout.h>
#include <poincare/bracket_pair_layout.h>
#include <poincare/code_point_layout.h>
#include <poincare/combined_code_points_layout.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_selection.h>
#include <poincare/symbol_abstract.h>

namespace Poincare {

Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  TreeHandle c = TreeHandle::clone();
  Layout cast = Layout(static_cast<LayoutNode *>(c.node()));
  cast.invalidAllSizesPositionsAndBaselines();
  return cast;
}

Layout Layout::LayoutFromAddress(const void * address, size_t size) {
  if (address == nullptr || size == 0) {
    return Layout();
  }
  return Layout(static_cast<LayoutNode *>(TreePool::sharedPool->copyTreeFromAddress(address, size)));
}

void Layout::draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, LayoutSelection selection, KDColor selectionColor) {
  node()->draw(ctx, p, font, expressionColor, backgroundColor, selection, selectionColor);
}

void Layout::draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor) {
  draw(ctx, p, font, expressionColor, backgroundColor, LayoutSelection());
}

int Layout::serializeParsedExpression(char * buffer, int bufferSize, Context * context) const {
  /* This method fixes the following problem:
   * Some layouts have a special serialization so they can be parsed afterwards,
   * such has logBase3(2) that serializes as log_{3}(2). When handling the
   * layout text, we want to use log(2,3) because we might paste the text in a
   * LinearEdition textfield, so what we really want is the parsed expression's
   * serialization. */
  if (bufferSize <= 0) {
    return 0;
  }
  serializeForParsing(buffer, bufferSize);
  Poincare::Expression e = Poincare::Expression::Parse(buffer, context);
  if (e.isUninitialized()) {
    buffer[0] = 0;
    return 0;
  }
  return e.serialize(buffer, bufferSize, Poincare::Preferences::sharedPreferences->displayMode());
}

Layout Layout::recursivelyMatches(LayoutTest test) const {
  TrinaryBoolean testResult = test(*this);
  if (testResult == TrinaryBoolean::True) {
    return *this;
  }
  if (testResult == TrinaryBoolean::False) {
    return Layout();
  }
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    Layout childResult = childAtIndex(i).recursivelyMatches(test);
    if (!childResult.isUninitialized()) {
      return childResult;
    }
  }
  return Layout();
}

Layout Layout::XNTLayout() const {
  Layout xntLayout = const_cast<Layout *>(this)->node()->XNTLayout();
  assert(xntLayout.isUninitialized() || xntLayout.numberOfDescendants(true) >= 0);
  if (!xntLayout.isUninitialized() && static_cast<size_t>(xntLayout.numberOfDescendants(true)) <= SymbolAbstract::k_maxNameSize && xntLayout.recursivelyMatches(
      [](const Layout l) {
        if (l.type() != LayoutNode::Type::CodePointLayout) {
          return l.isHorizontal() ? TrinaryBoolean::Unknown :  TrinaryBoolean::True;
        }
        CodePoint c = static_cast<const CodePointLayout&>(l).codePoint();
        return (c.isDecimalDigit() || c.isLatinLetter() || c == '_') ? TrinaryBoolean::Unknown : TrinaryBoolean::True;
      } ).isUninitialized()) {
    /* Return xnt if :
     * - it is initialized and only contains horizontal layouts and code points
     * - all code points are letters, numbers or _
     * - There are less than k_maxNameSize descendants */
    return xntLayout;
  }
  return Layout();
}

Layout Layout::childAtIndex(int i) const {
  assert(i >= 0 && i < numberOfChildren());
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Layout &>(c);
}

// Tree modification
/*
void Layout::replaceWithJuxtapositionOf(Layout leftChild, Layout rightChild) {
  Layout p = parent();
  assert(!p.isUninitialized());
  if (!p.isHorizontal()) {
    /* One of the children to juxtapose might be "this", so we cannot just call
     * replaceWith. *
    HorizontalLayout horizontalLayoutR = HorizontalLayout::Builder();
    p.replaceChild(*this, horizontalLayoutR);
    horizontalLayoutR.addOrMergeChildAtIndex(leftChild, 0);
    horizontalLayoutR.addOrMergeChildAtIndex(rightChild, horizontalLayoutR.numberOfChildren());
    return;
  }
  /* The parent is an Horizontal layout, so directly add the two juxtaposition
   * children to the parent. *
  int idxInParent = p.indexOfChild(*this);
  HorizontalLayout castedParent = HorizontalLayout(static_cast<HorizontalLayoutNode *>(p.node()));
  p.removeChild(*this, true);
  castedParent.addOrMergeChildAtIndex(rightChild, idxInParent);
  castedParent.addOrMergeChildAtIndex(leftChild, idxInParent);
}

void Layout::addChildAtIndex(Layout l, int index, int currentNumberOfChildren) {
  int newIndex = index;
  int newCurrentNumberOfChildren = currentNumberOfChildren;
  Layout nextPointedLayout;
  LayoutCursor::Position nextPosition = LayoutCursor::Position::Left;
  if (cursor != nullptr) {
    if (newIndex < this->numberOfChildren()) {
      nextPointedLayout = childAtIndex(newIndex);
      nextPosition = LayoutCursor::Position::Left;
    } else {
      nextPointedLayout = *this;
      nextPosition = LayoutCursor::Position::Right;
    }
  }

  addChildAtIndexInPlace(l, newIndex, newCurrentNumberOfChildren);

  if (cursor != nullptr) {
    cursor->setLayout(nextPointedLayout);
    cursor->setPosition(nextPosition);
  }
}

void Layout::addSibling(LayoutCursor * cursor, Layout * sibling, bool moveCursor) {
  if (!node()->willAddSibling(cursor, sibling, moveCursor)) {
    return;
  }
  /* The layout must have a parent, because HorizontalLayout's
   * preprocessAddSibling returns false only an HorizontalLayout can be the
   * root layout. *
  Layout rootLayout = root();
  Layout p = parent();
  assert(!p.isUninitialized());
  if (p.isHorizontal()) {
    int indexInParent = p.indexOfChild(*this);
    int indexOfCursor = p.indexOfChild(cursor->layout());
    /* indexOfCursor == -1 if cursor->layout() is not a child of p. This should
     * never happen, as addSibling is only called from inside
     * LayoutField::handleEventWithText, and LayoutField is supposed to keep
     * its cursor up to date.*
    assert(indexOfCursor >= 0);
    int siblingIndex = cursor->position() == LayoutCursor::Position::Left ? indexOfCursor : indexOfCursor + 1;

    /* Special case: If the neighbour sibling is a VerticalOffsetLayout, let it
     * handle the insertion of the new sibling. Do not enter the special case if
     * "this" is a VerticalOffsetLayout, to avoid an infinite loop. *
    if (type() != LayoutNode::Type::VerticalOffsetLayout) {
      Layout neighbour;
      if (cursor->position() == LayoutCursor::Position::Left && indexInParent > 0) {
        neighbour = p.childAtIndex(indexInParent - 1);
      } else if (cursor->position() == LayoutCursor::Position::Right && indexInParent < p.numberOfChildren() - 1) {
        neighbour = p.childAtIndex(indexInParent + 1);
      }
      if (!neighbour.isUninitialized() && neighbour.type() == LayoutNode::Type::VerticalOffsetLayout) {
        if (moveCursor) {
          cursor->setLayout(neighbour);
          cursor->setPosition(cursor->position() == LayoutCursor::Position::Left ? LayoutCursor::Position::Right : LayoutCursor::Position::Left);
        }
        neighbour.addSibling(cursor, sibling, moveCursor);
        return;
      }
    }

    // Else, let the parent add the sibling.
    HorizontalLayout(static_cast<HorizontalLayoutNode *>(p.node())).addOrMergeChildAtIndex(*sibling, siblingIndex, moveCursor ? cursor : nullptr);
    return;
  }
  if (cursor->position() == LayoutCursor::Position::Left) {
    replaceWithJuxtapositionOf(*sibling, *this, cursor, moveCursor);
  } else {
    assert(cursor->position() == LayoutCursor::Position::Right);
    replaceWithJuxtapositionOf(*this, *sibling, cursor);
  }
}*

void Layout::removeChild(Layout l, LayoutCursor * cursor, bool force) {
  assert(hasChild(l));
  int index = indexOfChild(l);
  removeChildInPlace(l, l.numberOfChildren());
  if (cursor) {
    if (index < numberOfChildren()) {
      Layout newCursorRef = childAtIndex(index);
      cursor->setLayout(newCursorRef);
      cursor->setPosition(LayoutCursor::Position::Left);
    } else {
      int newPointedLayoutIndex = index - 1;
      if (newPointedLayoutIndex >= 0 && newPointedLayoutIndex < numberOfChildren()) {
        cursor->setLayout(childAtIndex(newPointedLayoutIndex));
        cursor->setPosition(LayoutCursor::Position::Right);
      } else {
        cursor->setLayout(*this);
        cursor->setPosition(LayoutCursor::Position::Right);
      }
    }
  }
}

void Layout::removeChildAtIndex(int index, LayoutCursor * cursor, bool force) {
  return removeChild(childAtIndex(index), cursor, force);
}
*
bool Layout::collapseOnDirection(OMG::HorizontalDirection direction, int absorbingChildIndex, LayoutCursor * cursor) {
  Layout p = parent();
  if (p.isUninitialized() || !p.isHorizontal()) {
    return false;
  }
  int idxInParent = p.indexOfChild(*this);
  int numberOfSiblings = p.numberOfChildren();
  int numberOfOpenParenthesis = 0;
  bool canCollapse = true;
  Layout absorbingChild = childAtIndex(absorbingChildIndex);
  if (absorbingChild.isUninitialized() || !absorbingChild.isEmpty()) {
    return false;
  }
  if (!absorbingChild.isHorizontal()) {
    Layout horizontal = HorizontalLayout::Builder();
    replaceChild(absorbingChild, horizontal, cursor, true);
    horizontal.addChildAtIndexInPlace(absorbingChild, 0, 0);
    absorbingChild = horizontal;
  }
  HorizontalLayout horizontalAbsorbingChild = HorizontalLayout(static_cast<HorizontalLayoutNode *>(absorbingChild.node()));
  Layout sibling;
  bool forceCollapse = false;
  while (canCollapse) {
    if (direction.isRight() && idxInParent == numberOfSiblings - 1) {
      break;
    }
    if (direction.isLeft() && idxInParent == 0) {
      break;
    }
    int siblingIndex = direction.isRight() ? idxInParent+1 : idxInParent-1;
    sibling = p.childAtIndex(siblingIndex);
    /* Even if forceCollapse is true, isCollapsable should be called to update
     * the number of open parentheses. *
    bool shouldCollapse = sibling.isCollapsable(&numberOfOpenParenthesis, direction == OMG::HorizontalDirection::Left);
    if (shouldCollapse || forceCollapse) {
      p.removeChild(sibling, nullptr);
      int newIndex = direction == OMG::HorizontalDirection::Right ? absorbingChild.numberOfChildren() : 0;
      horizontalAbsorbingChild.addOrMergeChildAtIndex(sibling, newIndex);
      // removeChildAtIndex may have removed more than one child.
      numberOfSiblings = p.numberOfChildren();
      idxInParent = p.indexOfChild(*this);
    } else {
      break;
    }
  }
  return true;
}

void Layout::collapseSiblings(LayoutCursor * cursor) {
  bool collapsed = false;
  if (node()->shouldCollapseSiblingsOnRight()) {
    collapsed |= collapseOnDirection(OMG::NewDirection::Right(), rightCollapsingAbsorbingChildIndex(), cursor);
  }
  if (node()->shouldCollapseSiblingsOnLeft()) {
    collapsed |= collapseOnDirection(OMG::NewDirection::Left(), leftCollapsingAbsorbingChildIndex(), cursor);
  }
  if (collapsed) {
    node()->didCollapseSiblings(cursor);
  }
}
*/
bool Layout::privateHasTopLevelComparisonSymbol(bool includingNotEqualSymbol) const {
  if (type() != Poincare::LayoutNode::Type::HorizontalLayout) {
    return false;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Poincare::Layout child = childAtIndex(i);
    if ((child.type() == Poincare::LayoutNode::Type::CodePointLayout && static_cast<Poincare::CodePointLayout &>(child).codePoint().isEquationOperator())
     || (includingNotEqualSymbol && child.type() == Poincare::LayoutNode::Type::CombinedCodePointsLayout && static_cast<Poincare::CombinedCodePointsLayout &>(child).node()->isNotEqualOperator())) {
      return true;
    }
  }
  return false;
}

}
