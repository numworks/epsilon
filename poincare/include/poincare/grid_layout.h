#ifndef POINCARE_GRID_LAYOUT_NODE_H
#define POINCARE_GRID_LAYOUT_NODE_H

#include <poincare/array.h>
#include <poincare/empty_layout.h>
#include <poincare/layout.h>
#include <poincare/layout_cursor.h>

namespace Poincare {

class GridLayout;
class MatrixLayoutNode;

class GridLayoutNode : public Array, public LayoutNode {
  friend class MatrixLayoutNode;
  friend class BinomialCoefficientLayoutNode;
  friend class BinomialCoefficientLayout;
  friend class GridLayout;
public:
  GridLayoutNode() :
    Array(),
    LayoutNode()
  {}

  // Layout
  Type type() const override { return Type::GridLayout; }

  KDSize gridSize() const { return KDSize(width(), height()); }

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorUp(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;
  void moveCursorDown(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool equivalentPositionVisited = false, bool forSelection = false) override;

  // SerializableNode
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    assert(false);
    return 0;
  }

  // TreeNode
  size_t size() const override { return sizeof(GridLayoutNode); }
  void didChangeArity(int newNumberOfChildren) override { return Array::didChangeNumberOfChildren(newNumberOfChildren); }
  int numberOfChildren() const override { return m_numberOfRows * m_numberOfColumns; }
  void eraseNumberOfChildren() override {
    m_numberOfRows = 0;
    m_numberOfColumns = 0;
  }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "GridLayout";
  }
#endif

protected:
  // GridLayoutNode
  virtual void addEmptyRow(EmptyLayoutNode::Color color);
  virtual void addEmptyColumn(EmptyLayoutNode::Color color);
  virtual void deleteRowAtIndex(int index);
  virtual void deleteColumnAtIndex(int index);
  bool childIsRightOfGrid(int index) const;
  bool childIsLeftOfGrid(int index) const;
  bool childIsTopOfGrid(int index) const;
  bool childIsBottomOfGrid(int index) const;
  int rowAtChildIndex(int index) const;
  int columnAtChildIndex(int index) const;
  int indexAtRowColumn(int rowIndex, int columnIndex) const;

  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * l) override;

private:
  // GridLayoutNode
  constexpr static KDCoordinate k_gridEntryMargin = 6;
  KDCoordinate rowBaseline(int i);
  KDCoordinate rowHeight(int i) const;
  KDCoordinate height() const;
  KDCoordinate columnWidth(int j) const;
  KDCoordinate width() const;
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) override {}
};

class GridLayout : public Layout {
public:
  GridLayout(const GridLayoutNode * n) : Layout(n) {}
  static GridLayout Builder() { return TreeHandle::NAryBuilder<GridLayout,GridLayoutNode>(); }

  void setDimensions(int rows, int columns);
  using Layout::addChildAtIndex;
  int numberOfRows() const { return node()->numberOfRows(); }
  int numberOfColumns() const { return node()->numberOfColumns(); }
private:
  virtual GridLayoutNode * node() const { return static_cast<GridLayoutNode *>(Layout::node()); }
  void setNumberOfRows(int rows) { node()->setNumberOfRows(rows); }
  void setNumberOfColumns(int columns) { node()->setNumberOfColumns(columns); }
};

}

#endif
