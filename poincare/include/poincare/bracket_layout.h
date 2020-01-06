#ifndef POINCARE_BRACKET_LAYOUT_NODE_H
#define POINCARE_BRACKET_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout.h>

namespace Poincare {

class BracketLayoutNode : public LayoutNode {
public:
  BracketLayoutNode() :
    LayoutNode(),
    m_childHeightComputed(false),
    m_childHeight(0)
  {}

  // LayoutNode
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout, bool forSelection) override;
  void invalidAllSizesPositionsAndBaselines() override;

  // TreeNode
  size_t size() const override { return sizeof(BracketLayoutNode); }
  int numberOfChildren() const override { return 0; }
#if TREE_LOG
  const char * description() const override { return "BracketLayout"; }
#endif

protected:
  // LayoutNode
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
  KDCoordinate childHeight();
  KDCoordinate computeChildHeight();
  bool m_childHeightComputed;
  uint16_t m_childHeight;
};

}

#endif
