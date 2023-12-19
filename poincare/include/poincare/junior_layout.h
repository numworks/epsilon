#ifndef POINCARE_JUNIOR_LAYOUT_NODE_H
#define POINCARE_JUNIOR_LAYOUT_NODE_H

#include <poincare/layout.h>
#include <poincare/layout_cursor.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/memory/tree.h>

namespace Poincare {

class JuniorLayoutNode final : public LayoutNode {
  friend class JuniorLayout;

 public:
  JuniorLayoutNode(const PoincareJ::Tree* tree, size_t treeSize) {
    memcpy(m_blocks, tree, treeSize);
  }

  // Layout
  Type type() const override { return Type::JuniorLayout; }

  // TreeNode
  size_t size() const override {
    return sizeof(JuniorLayoutNode) + tree()->treeSize();
  }
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "JuniorLayout";
  }
#endif

 protected:
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) override;
  KDCoordinate computeBaseline(KDFont::Size font) override;
  KDPoint positionOfChild(LayoutNode* child, KDFont::Size font) override {
    assert(false);
    return KDPointZero;
  }
  Layout makeEditable() override;

 private:
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  const PoincareJ::Tree* tree() const {
    return PoincareJ::Tree::FromBlocks(m_blocks);
  }
  PoincareJ::Block m_blocks[0];
};

class JuniorLayout final
    : public LayoutNoChildren<JuniorLayout, JuniorLayoutNode> {
 public:
  JuniorLayout() {}
  JuniorLayout(const Layout& other) { *this = other; }

  static JuniorLayout Builder(const PoincareJ::Tree* tree);
  static JuniorLayout Juniorize(Layout l);
  const PoincareJ::Tree* tree() const {
    return const_cast<JuniorLayout*>(this)->node()->tree();
  }

  JuniorLayout operator=(Layout&& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorLayout operator=(const Layout& other) {
    *this = Juniorize(other);
    return *this;
  }

 private:
  using Layout::node;
  JuniorLayoutNode* node() {
    return static_cast<JuniorLayoutNode*>(Layout::node());
  }
};

}  // namespace Poincare

#endif
