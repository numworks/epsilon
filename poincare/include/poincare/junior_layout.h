#ifndef POINCARE_JUNIOR_LAYOUT_NODE_H
#define POINCARE_JUNIOR_LAYOUT_NODE_H

#include <poincare/old_layout.h>
#include <poincare_junior/src/memory/block.h>

namespace PoincareJ {
class Block;
class Tree;
class LayoutCursor;
struct ContextTrees;
}  // namespace PoincareJ

namespace Poincare {

class JuniorLayoutNode final : public LayoutNode {
  friend class JuniorLayout;

 private:
  JuniorLayoutNode(const PoincareJ::Tree* tree, size_t treeSize);

  // TreeNode
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "JuniorLayout";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) const override;
  KDCoordinate computeBaseline(KDFont::Size font) const override;

  bool protectedIsIdenticalTo(OLayout l) const override;

  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            PoincareJ::LayoutCursor* cursor, KDColor selectionColor) const;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) const override;

  const PoincareJ::Tree* tree() const;
  PoincareJ::Tree* tree();
  PoincareJ::Block m_blocks[0];
};

class JuniorLayout final : public OLayout {
 public:
  JuniorLayout() {}

  JuniorLayout(const PoincareJ::Tree* tree) {
    // TODO is copy-elimination guaranteed here ?
    *this = Builder(tree);
  }

  template <class C>
  JuniorLayout(C c) : JuniorLayout(static_cast<const PoincareJ::Tree*>(c)) {
    static_assert(c.type().isRackLayout());
  }

  JuniorLayout clone() const {
    OLayout clone = OLayout::clone();
    return static_cast<JuniorLayout&>(clone);
  }

  static JuniorLayout Create(const PoincareJ::Tree* structure,
                             PoincareJ::ContextTrees ctx);
  operator const PoincareJ::Tree*() { return tree(); }

  static JuniorLayout CodePoint(CodePoint cp);
  static JuniorLayout String(const char* str, int length = -1);

  static JuniorLayout Builder(const PoincareJ::Tree* tree);
  // Eat the tree
  static JuniorLayout Builder(PoincareJ::Tree* tree);
  PoincareJ::Tree* tree() const {
    return const_cast<JuniorLayout*>(this)->node()->tree();
  }

  JuniorLayout cloneWithoutMargins();
  JuniorLayout makeEditable() { return cloneWithoutMargins(); }

  bool isEmpty() const;

  // Render
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            PoincareJ::LayoutCursor* cursor,
            KDColor selectionColor = Escher::Palette::Select);
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style);

  JuniorLayoutNode* node() {
    return static_cast<JuniorLayoutNode*>(OLayout::node());
  }

  // True if rack with only code points in it
  bool isCodePointsString() const;
};

}  // namespace Poincare

#endif
