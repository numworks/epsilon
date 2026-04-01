#pragma once

#include <kandinsky/context.h>
#include <poincare/k_tree.h>
#include <poincare/layout_style.h>
#include <poincare/pool_handle.h>
#include <poincare/pool_object.h>
#include <poincare/preferences.h>
#include <poincare/src/layout/layout_memoization.h>

#include <span>

namespace Poincare::Internal {
class Block;
class Tree;
class LayoutCursor;
struct SimpleLayoutCursor;
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
#if POINCARE_TREE_LOG
  void logObjectName(std::ostream& stream) const override {
    stream << "Layout";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  size_t serialize(std::span<char> buffer,
                   Preferences::PrintFloatMode floatDisplayMode =
                       Preferences::PrintFloatMode::Decimal,
                   int numberOfSignificantDigits = 0) const;

  // LayoutNode
  KDSize computeSize(KDFont::Size font,
                     const Internal::LayoutCursor* cursor) const override;
  KDCoordinate computeBaseline(
      KDFont::Size font, const Internal::LayoutCursor* cursor) const override;

  bool isIdenticalTo(const Layout& l, bool makeEditable) const;

  void draw(KDContext* ctx, KDPoint p, const LayoutStyle& style,
            const Internal::LayoutCursor* cursor) const;

  const Internal::Tree* tree() const;
  Internal::Tree* tree();
  Internal::Block m_blocks[0];
};

class Layout final : public PoolHandle {
 public:
  static const Poincare::Layout& EmptyLayout();

  Layout() = default;

  // Implicit conversion from a const Tree*
  Layout(const Internal::Tree* tree) {
    // TODO is copy-elimination guaranteed here ?
    *this = Builder(tree);
  }
  template <Internal::KTrees::KTreeConcept T>
  Layout(T t) : Layout(static_cast<const Internal::Tree*>(t)) {
    static_assert(t.type().isRackLayout());
  }

  const LayoutObject* operator->() const {
    assert(isUninitialized() || (PoolHandle::object()));
    return static_cast<const LayoutObject*>(PoolHandle::object());
  }

  LayoutObject* operator->() {
    assert(isUninitialized() || (PoolHandle::object()));
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  bool isIdenticalTo(const Layout& l, bool makeEditable = false) const {
    return isUninitialized() ? l.isUninitialized()
                             : (*this)->isIdenticalTo(l, makeEditable);
  }

  static Layout Create(const Internal::Tree* structure,
                       Internal::ContextTrees ctx);
  // Implicit conversion to a const Tree*
  operator const Internal::Tree*() const { return tree(); }

  static Layout CodePoint(CodePoint cp);
  static Layout String(const char* str, int length = -1);
  static Layout Parse(const char* string);

  static Layout Builder(const Internal::Tree* tree);
  // Eat the tree
  static Layout Builder(Internal::Tree* tree);
  static Layout Concatenate(const Layout& layout1, const Layout& layout2);

  Internal::Tree* tree() const {
    return const_cast<Layout*>(this)->object()->tree();
  }

  int numberOfDescendants(bool includeSelf) const;

  // Serialization
  // TODO_PCJ: One of these is defined on PoolObject, the other is not
  // TODO_PCJ: Should these return a char* pointing to the end of the buffer ?
  size_t serialize(std::span<char> buffer) const {
    return (*this)->serialize(buffer);
  }
  size_t toLatex(char* buffer, size_t bufferSize) const;

  // Clone
  Layout clone() const;
  Layout cloneWithoutMargins() const;
  // KRackL(KAbsL("x"_l)) -> KRackL(KAbsL(""_l))
  Layout cloneWithoutChildrenRacks();

  Layout makeEditable() const { return cloneWithoutMargins(); }

  bool isEmpty() const;
  // Return the number of digits of the longest integer displayed.
  int longestIntegerSize() const;

  // Render
  void draw(KDContext* ctx, KDPoint p, const LayoutStyle& style,
            const Internal::LayoutCursor* cursor = nullptr) const;

  LayoutObject* object() {
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  const LayoutObject* object() const {
    return static_cast<LayoutObject*>(PoolHandle::object());
  }

  // True if rack with only code points in it
  bool isCodePointsString() const;

  bool isForbiddenForCopy() const;
};

}  // namespace Poincare
