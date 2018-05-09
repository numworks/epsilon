#ifndef POINCARE_EXPRESSION_LAYOUT_H
#define POINCARE_EXPRESSION_LAYOUT_H

#include <kandinsky.h>
#include <poincare/print_float.h>
#include <poincare/expression_layout_cursor.h>


namespace Poincare {

class ExpressionLayout {
public:
  enum class VerticalDirection {
    Up,
    Down
  };
  enum class HorizontalDirection {
    Left,
    Right
  };

  /* Constructor & Destructor */
  ExpressionLayout();
  virtual  ~ExpressionLayout() = default;
  virtual ExpressionLayout * clone() const = 0;

  /* Rendering */
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  KDPoint origin();
  KDPoint absoluteOrigin();
  KDSize size();
  KDCoordinate baseline();
  virtual void invalidAllSizesPositionsAndBaselines();

  /* Hierarchy */

  // Children
  virtual const ExpressionLayout * const * children() const = 0;
  const ExpressionLayout * child(int i) const;
  ExpressionLayout * editableChild(int i) { return const_cast<ExpressionLayout *>(child(i)); }
  virtual int numberOfChildren() const = 0;
  bool hasChild(const ExpressionLayout * child) const;
  int indexOfChild(const ExpressionLayout * child) const;

  // Parent
  void setParent(ExpressionLayout * parent) { m_parent = parent; }
  const ExpressionLayout * parent() const { return m_parent; }
  ExpressionLayout * editableParent() { return m_parent; }
  bool hasAncestor(const ExpressionLayout * e, bool includeSelf = false) const;
  ExpressionLayout * editableRoot();

  // Sibling
  bool hasSibling(const ExpressionLayout * e) const;

  /* Dynamic Layout */

  // Collapse
  virtual void collapseSiblingsAndMoveCursor(ExpressionLayoutCursor * cursor) {}

  // Add
  virtual bool addChildAtIndex(ExpressionLayout * child, int index) { return false; }
  void addSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling);
  void addSiblingAndMoveCursor(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling);

  // Replace
  virtual ExpressionLayout * replaceWith(ExpressionLayout * newChild, bool deleteAfterReplace);
  ExpressionLayout * replaceWithAndMoveCursor(ExpressionLayout * newChild, bool deleteAfterReplace, ExpressionLayoutCursor * cursor);
  virtual void replaceChild(const ExpressionLayout * oldChild, ExpressionLayout * newChild, bool deleteOldChild);
  virtual void replaceChildAndMoveCursor(
      const ExpressionLayout * oldChild,
      ExpressionLayout * newChild,
      bool deleteOldChild,
      ExpressionLayoutCursor * cursor);

  // Detach
  void detachChild(const ExpressionLayout * e); // Detach a child WITHOUT deleting it
  void detachChildren(); // Detach all children WITHOUT deleting them

  // Remove
  virtual void removeChildAtIndex(int index, bool deleteAfterRemoval);
  virtual void removePointedChildAtIndexAndMoveCursor(int index, bool deleteAfterRemoval, ExpressionLayoutCursor * cursor);

  // User input
  bool insertLayoutAtCursor(ExpressionLayout * newChild, ExpressionLayoutCursor * cursor);
  virtual void deleteBeforeCursor(ExpressionLayoutCursor * cursor);

  /* Tree navigation */
  virtual ExpressionLayoutCursor cursorLeftOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) = 0;
  virtual ExpressionLayoutCursor cursorRightOf(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout) = 0;
  virtual ExpressionLayoutCursor cursorAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false);
  virtual ExpressionLayoutCursor cursorUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false);
  ExpressionLayoutCursor cursorInDescendantsAbove(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout);
  ExpressionLayoutCursor cursorInDescendantsUnder(ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout);

  /* Expression Engine */
  virtual int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const = 0;

  /* Cursor */
  virtual ExpressionLayoutCursor equivalentCursor(ExpressionLayoutCursor cursor);

  /* Other */
  virtual ExpressionLayout * layoutToPointWhenInserting();
  bool addGreySquaresToAllMatrixAncestors();
  bool removeGreySquaresFromAllMatrixAncestors();
  bool hasText() const;
  virtual bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return true; }
  /* isCollapsable is used when adding a sibling fraction: should the layout be
   * inserted in the numerator (or denominator)? For instance, 1+2|3-4 should
   * become 1+ 2/3 - 4 when pressing "Divide": a CharLayout is collapsable if
   * its char is not +, -, or *. */
  bool canBeOmittedMultiplicationLeftFactor() const;
  virtual bool canBeOmittedMultiplicationRightFactor() const;
  /* canBeOmittedMultiplicationLeftFactor and RightFactor return true if the
   * layout, next to another layout, might be the factor of a multiplication
   * with an omitted multiplication sign. For instance, an absolute value layout
   * returns true, because |3|2 means |3|*2. A '+' CharLayout returns false,
   * because +'something' nevers means +*'something'. */
  virtual bool mustHaveLeftSibling() const { return false; }
  virtual bool isVerticalOffset() const { return false; }
  /* For now, mustHaveLeftSibling and isVerticalOffset behave the same, but code
   * is clearer with different names. */
  virtual bool isHorizontal() const { return false; }
  virtual bool isLeftParenthesis() const { return false; }
  virtual bool isRightParenthesis() const { return false; }
  virtual bool isLeftBracket() const { return false; }
  virtual bool isRightBracket() const { return false; }
  virtual bool isEmpty() const { return false; }
  virtual bool isMatrix() const { return false; }
  virtual bool hasUpperLeftIndex() const { return false; }
  virtual char XNTChar() const;

protected:
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
  virtual KDSize computeSize() = 0;
  virtual void computeBaseline() = 0;
  virtual KDPoint positionOfChild(ExpressionLayout * child) = 0;
  void scoreCursorInDescendantsVerticalOf (
    VerticalDirection direction,
    ExpressionLayoutCursor cursor,
    bool * shouldRecomputeLayout,
    ExpressionLayout ** childResult,
    void * resultPosition,
    int * resultScore);
  virtual void privateAddSibling(ExpressionLayoutCursor * cursor, ExpressionLayout * sibling, bool moveCursor);
  void collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex);
  virtual ExpressionLayoutCursor cursorVerticalOf(VerticalDirection direction, ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited);
  ExpressionLayout * m_parent;
  KDCoordinate m_baseline;
  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  bool m_sized;
  bool m_baselined;
  bool m_positioned;
private:
  void detachChildAtIndex(int i);
  ExpressionLayoutCursor cursorInDescendantsVerticalOf(VerticalDirection direction, ExpressionLayoutCursor cursor, bool * shouldRecomputeLayout);
  ExpressionLayout * replaceWithJuxtapositionOf(ExpressionLayout * leftChild, ExpressionLayout * rightChild, bool deleteAfterReplace);
  bool changeGreySquaresOfAllMatrixAncestors(bool add);
  KDRect m_frame;
};

}

#endif
