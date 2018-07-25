#ifndef POINCARE_GRID_LAYOUT_NODE_H
#define POINCARE_GRID_LAYOUT_NODE_H

#include <poincare/layout_reference.h>
#include <poincare/layout_node.h>
#include <poincare/layout_cursor.h>
#include <poincare/empty_layout_node.h>

namespace Poincare {

class GridLayoutRef;

class GridLayoutNode : public LayoutNode {
  friend class BinomialCoefficientLayoutNode;
  friend class BinomialCoefficientLayoutRef;
  friend class LayoutReference<GridLayoutNode>;
public:
  GridLayoutNode() :
    LayoutNode(),
    m_numberOfRows(0),
    m_numberOfColumns(0)
  {}

  void setNumberOfRows(int numberOfRows) { m_numberOfRows = numberOfRows; }
  void setNumberOfColumns(int numberOfColumns) { m_numberOfColumns = numberOfColumns; }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false) override;

  // SerializableNode
  int writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
  }

  // TreeNode
  size_t size() const override { return sizeof(GridLayoutNode); }
  int numberOfChildren() const override { return m_numberOfRows * m_numberOfColumns; }
  void eraseNumberOfChildren() override {
    m_numberOfRows = 0;
    m_numberOfColumns = 0;
  }
#if TREE_LOG
  const char * description() const override {
    return "GridLayout";
  }
#endif

protected:
  // GridLayoutNode
  void addEmptyRow(EmptyLayoutNode::Color color);
  void addEmptyColumn(EmptyLayoutNode::Color color);
  void deleteRowAtIndex(int index);
  void deleteColumnAtIndex(int index);
  bool childIsRightOfGrid(int index) const;
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  int rowAtChildIndex(int index) const;
  int columnAtChildIndex(int index) const;
  int indexAtRowColumn(int rowIndex, int columnIndex) const;
  int m_numberOfRows;
  int m_numberOfColumns;

  // LayoutNode
  void computeSize() override;
  void computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  // GridLayoutNode
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i);
  KDCoordinate height();
  KDCoordinate columnWidth(int j);
  KDCoordinate width();
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
};

class GridLayoutRef : public LayoutReference<GridLayoutNode> {
public:
  GridLayoutRef(LayoutRef l1, LayoutRef l2) :
    LayoutReference<GridLayoutNode>()
  {
    if (!(node()->isAllocationFailure())) {
      typedNode()->setNumberOfRows(2);
      typedNode()->setNumberOfColumns(1);
    }
    addChildTreeAtIndex(l1, 0);
    addChildTreeAtIndex(l2, 1);
  }
  GridLayoutRef(TreeNode * t) : LayoutReference<GridLayoutNode>(t) {}
};

}

#endif
