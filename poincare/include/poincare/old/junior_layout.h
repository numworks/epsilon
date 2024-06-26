#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare/src/memory/block.h>

#include "old_layout.h"

namespace Poincare::Internal {
class Block;
class Tree;
class LayoutCursor;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class JuniorLayoutNode final : public LayoutNode {
  friend class JuniorLayout;

 private:
  JuniorLayoutNode(const Internal::Tree* tree, size_t treeSize);

  // PoolObject
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
            Internal::LayoutCursor* cursor, KDColor selectionColor) const;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) const override;

  const Internal::Tree* tree() const;
  Internal::Tree* tree();
  Internal::Block m_blocks[0];
};

class JuniorLayout final : public OLayout {
 public:
  JuniorLayout() {}

  JuniorLayout(const Internal::Tree* tree) {
    // TODO is copy-elimination guaranteed here ?
    *this = Builder(tree);
  }

  template <class C>
  JuniorLayout(C c) : JuniorLayout(static_cast<const Internal::Tree*>(c)) {
    static_assert(c.type().isRackLayout());
  }

  JuniorLayout clone() const {
    OLayout clone = OLayout::clone();
    return static_cast<JuniorLayout&>(clone);
  }

  static JuniorLayout Create(const Internal::Tree* structure,
                             Internal::ContextTrees ctx);
  operator const Internal::Tree*() { return tree(); }

  static JuniorLayout CodePoint(CodePoint cp);
  static JuniorLayout String(const char* str, int length = -1);
  static JuniorLayout Parse(const char* string);

  static JuniorLayout Builder(const Internal::Tree* tree);
  // Eat the tree
  static JuniorLayout Builder(Internal::Tree* tree);
  Internal::Tree* tree() const {
    return const_cast<JuniorLayout*>(this)->node()->tree();
  }

  JuniorLayout cloneWithoutMargins();

  // KRackL(KAbsL("x"_l)) -> KRackL(KAbsL(""_l))
  JuniorLayout cloneWithoutChildrenRacks();

  JuniorLayout makeEditable() { return cloneWithoutMargins(); }

  bool isEmpty() const;

  // Render
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            Internal::LayoutCursor* cursor,
            KDColor selectionColor = Escher::Palette::Select);
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style);

  JuniorLayoutNode* node() {
    return static_cast<JuniorLayoutNode*>(OLayout::object());
  }

  // True if rack with only code points in it
  bool isCodePointsString() const;
};

}  // namespace Poincare

#endif
