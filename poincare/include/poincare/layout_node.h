#ifndef POINCARE_LAYOUT_NODE_H
#define POINCARE_LAYOUT_NODE_H

#include <poincare/tree_node.h>
#include <kandinsky.h>

namespace Poincare {

class LayoutCursor;

class LayoutNode : public TreeNode {
public:

  // Constructor
  LayoutNode() :
    TreeNode(),
    m_baseline(0),
    m_frame(KDRectZero),
    m_baselined(false),
    m_positioned(false),
    m_sized(false)
  {
  }

  virtual bool hasText() const { return false; } //TODO
  virtual char XNTChar() const { return 'x'; }
  virtual bool isHorizontal() const { return false; }
  virtual bool isEmpty() const { return false; }
  virtual bool isLeftParenthesis() const { return false; }
  virtual bool isVerticalOffset() const { return false; }
  virtual bool mustHaveLeftSibling() const { return false; }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite);
  KDPoint origin();
  KDPoint absoluteOrigin();
  KDSize layoutSize();
  KDCoordinate baseline();
  virtual void invalidAllSizesPositionsAndBaselines();

  // TreeNode
  static TreeNode * FailedAllocationStaticNode();
  TreeNode * failedAllocationStaticNode() override { return FailedAllocationStaticNode(); }
  static int AllocationFailureNodeIdentifier() {
    return FailedAllocationStaticNode()->identifier();
  }
  int allocationFailureNodeIdentifier() override {
    return AllocationFailureNodeIdentifier();
  }

  // Hierarchy
  LayoutNode * parent() const { return static_cast<LayoutNode *>(parentTree()); }
  LayoutNode * childAtIndex(int i) { return static_cast<LayoutNode *>(childTreeAtIndex(i)); }
  LayoutNode * root() { return static_cast<LayoutNode *>(rootTree()); }

  // Tree navigation
  virtual void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) {}
  virtual void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) {}
  virtual void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {}
  virtual void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) {}
  virtual LayoutCursor equivalentCursor(LayoutCursor * cursor); //TODO

  // Tree modification
  void addSibling(LayoutCursor * cursor, LayoutNode * sibling);
  void addSiblingAndMoveCursor(LayoutCursor * cursor, LayoutNode * sibling);
  void collapseSiblingsAndMoveCursor(LayoutCursor * cursor) {} //TODO
  bool removeGreySquaresFromAllMatrixAncestors() { return false; } //TODO
  bool addGreySquaresToAllMatrixAncestors() { return false; } //TODO
  virtual LayoutNode * layoutToPointWhenInserting() { return this; } //TODO
  LayoutNode * replaceWithJuxtapositionOf(LayoutNode * leftChild, LayoutNode * rightChild) {return nullptr; } //TODO
protected:
  // Iterators
  class Iterator {
    public:
    Iterator(LayoutNode * node) : m_node(node) {}
    LayoutNode * operator*() { return m_node; }
    bool operator!=(const Iterator& it) const { return (m_node != it.m_node); }
  protected:
    LayoutNode * m_node;
  };
  class DirectChildren {
  public:
    DirectChildren(LayoutNode * node) : m_node(node) {}
    class Iterator : public LayoutNode::Iterator {
    public:
      using LayoutNode::Iterator::Iterator;
      Iterator & operator++() {
        m_node = static_cast<LayoutNode *>(m_node->nextSibling());
        return *this;
      }
    };
    Iterator begin() const { return Iterator(static_cast<LayoutNode *>(m_node->next())); }
    Iterator end() const { return Iterator(static_cast<LayoutNode *>(m_node->nextSibling())); }
  private:
    LayoutNode * m_node;
  };
  DirectChildren children() { return DirectChildren(this); }

  // Sizing and positioning
  virtual void computeSize() = 0;
  virtual void computeBaseline() = 0;
  virtual KDPoint positionOfChild(LayoutNode * child) = 0;

  /* m_baseline is the signed vertical distance from the top of the layout to
   * the fraction bar of an hypothetical fraction sibling layout. If the top of
   * the layout is under that bar, the baseline is negative. */
  KDCoordinate m_baseline;
  KDRect m_frame;
  bool m_baselined;
  bool m_positioned;
  bool m_sized;
private:
  virtual void privateAddSibling(LayoutCursor * cursor, LayoutNode * sibling, bool moveCursor);
  virtual void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) = 0;
};

}

#endif
