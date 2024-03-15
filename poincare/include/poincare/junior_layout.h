#ifndef POINCARE_JUNIOR_LAYOUT_NODE_H
#define POINCARE_JUNIOR_LAYOUT_NODE_H

#include <poincare/layout_cursor.h>
#include <poincare/old_layout.h>
#include <poincare_junior/src/layout/k_tree.h>
#include <poincare_junior/src/layout/layout_selection.h>
#include <poincare_junior/src/memory/tree.h>

namespace PoincareJ {
class LayoutCursor;
}

namespace Poincare {

class JuniorLayoutNode final : public LayoutNode {
  friend class JuniorLayout;

 public:
  JuniorLayoutNode(const PoincareJ::Tree* tree, size_t treeSize) {
    memcpy(m_blocks, tree->block(), treeSize);
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
  void logAttributes(std::ostream& stream) const override {
#if POINCARE_MEMORY_TREE_LOG
    stream << '\n';
    tree()->log(stream);
#endif
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
  OLayout makeEditable() override;

  bool protectedIsIdenticalTo(OLayout l) override;

  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            PoincareJ::LayoutCursor* cursor, KDColor selectionColor);
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) override;

  const PoincareJ::Tree* tree() const {
    return PoincareJ::Tree::FromBlocks(m_blocks);
  }
  PoincareJ::Tree* tree() { return PoincareJ::Tree::FromBlocks(m_blocks); }
  PoincareJ::Block m_blocks[0];
};

class JuniorLayout final
    : public LayoutNoChildren<JuniorLayout, JuniorLayoutNode> {
 public:
  JuniorLayout() {}
  JuniorLayout(const OLayout& other) { *this = other; }

  static JuniorLayout Builder(const PoincareJ::Tree* tree);
  // Eat the tree
  static JuniorLayout Builder(PoincareJ::Tree* tree);
  static JuniorLayout Juniorize(OLayout l);
  static OLayout UnJuniorize(JuniorLayout l);
  PoincareJ::Tree* tree() const {
    return const_cast<JuniorLayout*>(this)->node()->tree();
  }

  JuniorLayout operator=(OLayout&& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorLayout operator=(const OLayout& other) {
    *this = Juniorize(other);
    return *this;
  }

  JuniorLayout cloneWithoutMargins();

  bool isEmpty() const { return tree()->numberOfChildren() == 0; }

  // Render
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            PoincareJ::LayoutCursor* cursor,
            KDColor selectionColor = Escher::Palette::Select);
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style);

  JuniorLayoutNode* node() {
    return static_cast<JuniorLayoutNode*>(OLayout::node());
  }
};

}  // namespace Poincare

#endif
