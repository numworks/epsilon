#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare/src/memory/block.h>
#include <poincare/src/memory/k_tree_concept.h>

#include "old_layout.h"

namespace Poincare::Internal {
class Block;
class Tree;
class LayoutCursor;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class JuniorLayout;

class JuniorLayoutNode final : public PoolObject, public LayoutMemoization {
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
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const override;

  // LayoutNode
  KDSize computeSize(KDFont::Size font) const override;
  KDCoordinate computeBaseline(KDFont::Size font) const override;

  bool isIdenticalTo(JuniorLayout l, bool makeEditable) const;

  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            Internal::LayoutCursor* cursor, KDColor selectionColor) const;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) const;

  const Internal::Tree* tree() const;
  Internal::Tree* tree();
  Internal::Block m_blocks[0];
};

class JuniorLayout final : public PoolHandle {
 public:
  JuniorLayout() {}

  JuniorLayout(const Internal::Tree* tree) {
    // TODO is copy-elimination guaranteed here ?
    *this = Builder(tree);
  }

  template <Internal::KTrees::KTreeConcept T>
  JuniorLayout(T t) : JuniorLayout(static_cast<const Internal::Tree*>(t)) {
    static_assert(t.type().isRackLayout());
  }

  const JuniorLayoutNode* operator->() const {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<const JuniorLayoutNode*>(PoolHandle::object());
  }

  JuniorLayoutNode* operator->() {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<JuniorLayoutNode*>(PoolHandle::object());
  }

  bool isIdenticalTo(JuniorLayout l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : (*this)->isIdenticalTo(l, makeEditable);
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
  static JuniorLayout Concatenate(JuniorLayout layout1, JuniorLayout layout2);

  Internal::Tree* tree() const {
    return const_cast<JuniorLayout*>(this)->node()->tree();
  }

  JuniorLayout childAtIndex(int i) const {
    // JuniorLayout cannot have parents or children JuniorLayouts.
    assert(false);
    return JuniorLayout();
  }

  // Serialization
  size_t serialize(char* buffer, size_t bufferSize) const {
    return (*this)->serialize(buffer, bufferSize);
  }

  // Clone
  JuniorLayout clone() const;
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
    return static_cast<JuniorLayoutNode*>(PoolHandle::object());
  }

  const JuniorLayoutNode* node() const {
    return static_cast<JuniorLayoutNode*>(PoolHandle::object());
  }

  // True if rack with only code points in it
  bool isCodePointsString() const;
};

}  // namespace Poincare

#endif
