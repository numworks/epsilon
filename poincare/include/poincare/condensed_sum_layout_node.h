#ifndef POINCARE_CONDENSED_SUM_LAYOUT_NODE_H
#define POINCARE_CONDENSED_SUM_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/layout_node.h>
#include <poincare/layout_engine.h>
#include <poincare/layout_reference.h>

namespace Poincare {

class CondensedSumLayoutNode : public LayoutNode {
public:
  using LayoutNode::LayoutNode;

  /* CondensedSumLayout is only used in apps/shared/sum_graph_controller.cpp, in
   * a view with no cursor. */
  void moveCursorLeft(LayoutCursor * cursor, bool * shouldRecomputeLayout) override { assert(false); }
  void moveCursorRight(LayoutCursor * cursor, bool * shouldRecomputeLayout) override { assert(false); }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return LayoutEngine::writePrefixSerializableRefTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "sum");
  }

  LayoutNode * layoutToPointWhenInserting() override {
    assert(false);
    return nullptr;
  }

  // TreeNode
  size_t size() const override { return sizeof(CondensedSumLayoutNode); }
  int numberOfChildren() const override { return 3; }
#if TREE_LOG
  const char * description() const override {
    return "CondensedSumLayout";
  }
#endif

protected:
  // LayoutNode
  KDSize computeSize() override;
  KDCoordinate computeBaseline() override;
  KDPoint positionOfChild(LayoutNode * child) override;
private:
  void render(KDContext * ctx, KDPoint p, KDColor expressionColor, KDColor backgroundColor) override {}
  LayoutNode * baseLayout() { return childAtIndex(0); }
  LayoutNode * subscriptLayout() { return childAtIndex(1); }
  LayoutNode * superscriptLayout() { return childAtIndex(2); }
};

class CondensedSumLayoutRef : public LayoutReference {
public:
  CondensedSumLayoutRef(LayoutRef base, LayoutRef subscript, LayoutRef superscript) :
    CondensedSumLayoutRef()
  {
    addChildAtIndex(base, 0, 0);
    addChildAtIndex(subscript, 1, 1);
    addChildAtIndex(superscript, 2, 2);
  }

  CondensedSumLayoutRef(TreeNode * n) : LayoutReference(n) {}
private:
  CondensedSumLayoutRef() : LayoutReference() {
    TreeNode * node = TreePool::sharedPool()->createTreeNode<CondensedSumLayoutNode>();
    m_identifier = node->identifier();
  }
};

}

#endif
