#include <escher/metric.h>
#include <ion/display.h>
#include <poincare/code_point_layout.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/horizontal_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare/layout_selection.h>
#include <poincare/matrix_layout.h>
#include <poincare/vertical_offset_layout.h>

namespace Poincare {

#define Layout OLayout

bool LayoutNode::isIdenticalTo(Layout l, bool makeEditable) {
  if (makeEditable) {
    return Layout(this).clone().makeEditable().isIdenticalTo(
        l.clone().makeEditable(), false);
  }
  if (l.isUninitialized() || type() != l.type()) {
    return false;
  }
  if (identifier() == l.identifier()) {
    return true;
  }
  return protectedIsIdenticalTo(l);
}

KDPoint LayoutNode::absoluteOriginWithMargin(KDFont::Size font) {
  LayoutNode *p = parent();
  if (!m_flags.m_positioned || m_flags.m_positionFontSize != font) {
    if (p != nullptr) {
      assert(!SumOverflowsKDCoordinate(p->absoluteOrigin(font).x(),
                                       p->positionOfChild(this, font).x()));
      assert(!SumOverflowsKDCoordinate(p->absoluteOrigin(font).y(),
                                       p->positionOfChild(this, font).y()));
      m_frame.setOrigin(
          p->absoluteOrigin(font).translatedBy(p->positionOfChild(this, font)));
    } else {
      m_frame.setOrigin(KDPointZero);
    }
    m_flags.m_positioned = true;
    m_flags.m_positionFontSize = font;
  }
  return m_frame.origin();
}

KDSize LayoutNode::layoutSize(KDFont::Size font) {
  if (!m_flags.m_sized || m_flags.m_sizeFontSize != font) {
    KDSize size = computeSize(font);

    /* This method will raise an exception if the size of the layout that is
     * passed is beyond k_maxLayoutSize.
     * The purpose of this hack is to avoid overflowing KDCoordinate when
     * drawing a layout.
     *
     * Currently, the only layouts that can overflow KDCoordinate without
     * overflowing the pool are:
     *  - the derivative layouts (if multiple derivative layouts are nested
     *    inside the variable layout or the order layout)
     *  - the horizontal layouts (when a very long list is generated through a
     *    sequence and each child is large).
     * This two sepific cases are handled in their own computeSize methods but
     * we still do this check for other layouts.
     *
     * Raising an exception might not be the best option though. We could just
     * handle the max size better and simply crop the layout (which is not that
     * easy to implement), instead of raising an exception.
     *
     * The following solutions were also explored but deemed too complicated for
     * the current state of the issue:
     *  - Make all KDCoordinate int32 and convert at the last moment into int16,
     * only when stored as m_variable.
     *  - Rewrite KDCoordinate::operator+ so that KDCOORDINATE_MAX is returned
     * if the + overflows.
     *  - Forbid insertion of a large layout as the child of another layout.
     *  - Check for an overflow before each translation of p in the render
     * methods*/
    if (size.height() >= k_maxLayoutSize || size.width() >= k_maxLayoutSize) {
      ExceptionCheckpoint::Raise();
    }

    m_frame.setSize(size);
    m_flags.m_sized = true;
    m_flags.m_sizeFontSize = font;
  }
  return m_frame.size();
}

KDCoordinate LayoutNode::baseline(KDFont::Size font) {
  if (!m_flags.m_baselined || m_flags.m_baselineFontSize != font) {
    m_baseline = computeBaseline(font);
    m_flags.m_baselined = true;
    m_flags.m_baselineFontSize = font;
  }
  return m_baseline;
}

void LayoutNode::invalidAllSizesPositionsAndBaselines() {
  m_flags.m_sized = false;
  m_flags.m_positioned = false;
  m_flags.m_baselined = false;
  for (LayoutNode *l : children()) {
    l->invalidAllSizesPositionsAndBaselines();
  }
}

int LayoutNode::indexAfterHorizontalCursorMove(
    OMG::HorizontalDirection direction, int currentIndex,
    bool *shouldRedrawLayout) {
  int nChildren = numberOfChildren();
  if (nChildren == 0) {
    assert(currentIndex == k_outsideIndex);
    return k_outsideIndex;
  }
  if (nChildren == 1) {
    assert(currentIndex == k_outsideIndex || currentIndex == 0);
    return currentIndex == k_outsideIndex ? 0 : k_outsideIndex;
  }
  assert(false);
  return k_cantMoveIndex;
}

int LayoutNode::indexAfterVerticalCursorMove(
    OMG::VerticalDirection direction, int currentIndex,
    PositionInLayout positionAtCurrentIndex, bool *shouldRedrawLayout) {
  assert(currentIndex < numberOfChildren());
  assert(currentIndex != k_outsideIndex ||
         positionAtCurrentIndex != PositionInLayout::Middle);
  return k_cantMoveIndex;
}

LayoutNode::DeletionMethod LayoutNode::deletionMethodForCursorLeftOfChild(
    int childIndex) const {
  assert((childIndex >= 0 || childIndex == k_outsideIndex) &&
         childIndex < numberOfChildren());
  return childIndex == k_outsideIndex ? DeletionMethod::DeleteLayout
                                      : DeletionMethod::MoveLeft;
}

LayoutNode::DeletionMethod
LayoutNode::StandardDeletionMethodForLayoutContainingArgument(
    int childIndex, int argumentIndex) {
  return childIndex == argumentIndex ? DeletionMethod::DeleteParent
                                     : DeletionMethod::MoveLeft;
}

int LayoutNode::indexOfChildToPointToWhenInserting() {
  return numberOfChildren() > 0 ? 0 : k_outsideIndex;
}

Layout LayoutNode::makeEditable() {
  /* We visit children if reverse order to avoid visiting the codepoints they
   * might have inserted after them. */
  for (int i = numberOfChildren() - 1; i >= 0; i--) {
    childAtIndex(i)->makeEditable();
  }
  return Layout(this);
}

// Other
bool LayoutNode::canBeOmittedMultiplicationLeftFactor() const {
  if (type() == LayoutNode::Type::CodePointLayout &&
      static_cast<const CodePointLayoutNode *>(this)
          ->isMultiplicationCodePoint()) {
    return false;
  }
  /* WARNING: canBeOmittedMultiplicationLeftFactor is true when and only when
   * isCollapsable is true too. If isCollapsable changes, it might not be the
   * case anymore so make sure to modify this function if needed. */
  int numberOfOpenParentheses = 0;
  return isCollapsable(&numberOfOpenParentheses, OMG::Direction::Left());
}

bool LayoutNode::createGraySquaresAfterEnteringGrid(Layout layoutToExclude) {
  return changeGraySquaresOfAllGridRelatives(true, true, layoutToExclude);
}

bool LayoutNode::deleteGraySquaresBeforeLeavingGrid(Layout layoutToExclude) {
  return changeGraySquaresOfAllGridRelatives(false, true, layoutToExclude);
}

// Protected and private

bool LayoutNode::protectedIsIdenticalTo(Layout l) {
  if (numberOfChildren() != l.numberOfChildren()) {
    return false;
  }
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    if (!childAtIndex(i)->isIdenticalTo(l.childAtIndex(i))) {
      return false;
    }
  }
  return true;
}

bool addRemoveGraySquaresInLayoutIfNeeded(bool add, Layout *l) {
  if (!GridLayoutNode::IsGridLayoutType(l->type())) {
    return false;
  }
  if (add) {
    static_cast<GridLayoutNode *>(l->node())->startEditing();
  } else {
    static_cast<GridLayoutNode *>(l->node())->stopEditing();
  }
  return true;
}

bool LayoutNode::changeGraySquaresOfAllGridRelatives(bool add, bool ancestors,
                                                     Layout layoutToExclude) {
  bool changedSquares = false;
  if (!ancestors) {
    // If in children, we also change the squares for this
    {
      Layout thisLayout = Layout(this);
      if ((layoutToExclude.isUninitialized() ||
           !layoutToExclude.hasAncestor(thisLayout, false)) &&
          addRemoveGraySquaresInLayoutIfNeeded(add, &thisLayout)) {
        changedSquares = true;
      }
    }
    int childrenNumber = numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      /* We cannot use "for l : children()", as the node addresses might change,
       * especially the iterator stopping address. */
      changedSquares = changedSquares ||
                       childAtIndex(i)->changeGraySquaresOfAllGridRelatives(
                           add, false, layoutToExclude);
    }
  } else {
    Layout currentAncestor = Layout(parent());
    while (!currentAncestor.isUninitialized()) {
      if (!layoutToExclude.isUninitialized() &&
          layoutToExclude.hasAncestor(currentAncestor, false)) {
        break;
      }
      if (addRemoveGraySquaresInLayoutIfNeeded(add, &currentAncestor)) {
        changedSquares = true;
      }
      currentAncestor = currentAncestor.parent();
    }
  }
  return changedSquares;
}

}  // namespace Poincare
