#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <escher/palette.h>
#include <kandinsky/context.h>
#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>
#include <poincare/src/layout/layout_memoization.h>
#include <poincare/src/memory/block.h>
#include <poincare/src/memory/k_tree_concept.h>

namespace Poincare::Internal {
class Block;
class Tree;
class LayoutCursor;
struct ContextTrees;
}  // namespace Poincare::Internal

namespace Poincare {

class Layout;

class LayoutObject final : public PoolObject,
                               public Internal::LayoutMemoization {
  friend class Layout;

 private:
  LayoutObject(const Internal::Tree* tree, size_t treeSize);

  // PoolObject
  size_t size() const override;
  int numberOfChildren() const override { return 0; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Layout";
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

  bool isIdenticalTo(Layout l, bool makeEditable) const;

  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            Internal::LayoutCursor* cursor, KDColor selectionColor) const;
  void render(KDContext* ctx, KDPoint p, KDGlyph::Style style) const;

  const Internal::Tree* tree() const;
  Internal::Tree* tree();
  Internal::Block m_blocks[0];
};

class Layout final : public PoolHandle {
 public:
  Layout() {}

  Layout(const Internal::Tree* tree) {
    // TODO is copy-elimination guaranteed here ?
    *this = Builder(tree);
  }

  template <Internal::KTrees::KTreeConcept T>
  Layout(T t) : Layout(static_cast<const Internal::Tree*>(t)) {
    static_assert(t.type().isRackLayout());
  }

  const LayoutObject* operator->() const {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<const LayoutObject*>(PoolHandle::object());
  }

  LayoutObject* operator->() {
    assert(isUninitialized() ||
           (PoolHandle::object() && !PoolHandle::object()->isGhost()));
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  bool isIdenticalTo(Layout l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : (*this)->isIdenticalTo(l, makeEditable);
  }

  static Layout Create(const Internal::Tree* structure,
                             Internal::ContextTrees ctx);
  operator const Internal::Tree*() { return tree(); }

  static Layout CodePoint(CodePoint cp);
  static Layout String(const char* str, int length = -1);
  static Layout Parse(const char* string);

  static Layout Builder(const Internal::Tree* tree);
  // Eat the tree
  static Layout Builder(Internal::Tree* tree);
  static Layout Concatenate(Layout layout1, Layout layout2);

  Internal::Tree* tree() const {
    return const_cast<Layout*>(this)->node()->tree();
  }

  Layout childAtIndex(int i) const {
    // Layout cannot have parents or children Layouts.
    assert(false);
    return Layout();
  }

  // Serialization
  size_t serialize(char* buffer, size_t bufferSize) const {
    return (*this)->serialize(buffer, bufferSize);
  }

  // Clone
  Layout clone() const;
  Layout cloneWithoutMargins();
  // KRackL(KAbsL("x"_l)) -> KRackL(KAbsL(""_l))
  Layout cloneWithoutChildrenRacks();

  Layout makeEditable() { return cloneWithoutMargins(); }

  bool isEmpty() const;

  // Render
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
            Internal::LayoutCursor* cursor,
            KDColor selectionColor = Escher::Palette::Select);
  void draw(KDContext* ctx, KDPoint p, KDGlyph::Style style);

  LayoutObject* node() {
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  const LayoutObject* node() const {
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  // True if rack with only code points in it
  bool isCodePointsString() const;
};

}  // namespace Poincare

#endif
