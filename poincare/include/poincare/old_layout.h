#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <escher/palette.h>
#include <omg/directions.h>
#include <poincare/context.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>
#include <poincare/trinary_boolean.h>

namespace PoincareJ {
class LayoutCursor;
}

namespace Poincare {

class OLayout : public TreeHandle {
  friend class AdditionNode;
  friend class GridLayoutNode;
  friend class HorizontalLayoutNode;
  friend class InputBeautification;
  friend class LayoutNode;
  friend class VerticalOffsetLayoutNode;

 public:
  OLayout() : TreeHandle() {}
  OLayout(const LayoutNode *node) : TreeHandle(node) {}
  OLayout clone() const;
  LayoutNode *node() const {
    assert(isUninitialized() ||
           (TreeHandle::node() && !TreeHandle::node()->isGhost()));
    return static_cast<LayoutNode *>(TreeHandle::node());
  }
  static OLayout LayoutFromAddress(const void *address, size_t size);

  // Properties
  LayoutNode::Type type() const { return node()->type(); }
  bool isHorizontal() const { return node()->isHorizontal(); }
  bool isEmpty() const { return node()->isEmpty(); }
  // True if horizontal layout with only code points in it
  bool isCodePointsString() const;
  bool isIdenticalTo(OLayout l, bool makeEditable = false) {
    return isUninitialized() ? l.isUninitialized()
                             : node()->isIdenticalTo(l, makeEditable);
  }

  // Rendering
  void render(KDContext *ctx, KDPoint p, KDGlyph::Style style) {
    return node()->render(ctx, p, style);
  }
  KDSize layoutSize(KDFont::Size font,
                    PoincareJ::LayoutCursor *cursor = nullptr) const;
  KDPoint absoluteOrigin(KDFont::Size font) const {
    return node()->absoluteOrigin(font);
  }
  KDCoordinate baseline(KDFont::Size font,
                        PoincareJ::LayoutCursor *cursor = nullptr);
  void invalidAllSizesPositionsAndBaselines() {
    return node()->invalidAllSizesPositionsAndBaselines();
  }

  // Serialization
  size_t serializeForParsing(char *buffer, size_t bufferSize) const {
    return node()->serialize(buffer, bufferSize);
  }
  size_t serializeParsedExpression(char *buffer, size_t bufferSize,
                                   Context *context) const;

  // Layout properties
  /* Return True if the layout succeeded the test, Unknown if its children
   * could succeed, and False if the recursion should stop. */
  typedef TrinaryBoolean (*LayoutTest)(const OLayout l);
  OLayout recursivelyMatches(LayoutTest test) const;

  bool shouldCollapseSiblingsOnLeft() const {
    return type() == LayoutNode::Type::FractionLayout;
  }
  bool shouldCollapseSiblingsOnRight() const;
  int leftCollapsingAbsorbingChildIndex() const { return 0; }
  int rightCollapsingAbsorbingChildIndex() const {
    return type() == LayoutNode::Type::FractionLayout ? 1 : 0;
  }
  bool isCollapsable(int *numberOfOpenParenthesis,
                     OMG::HorizontalDirection direction) const {
    return const_cast<OLayout *>(this)->node()->isCollapsable(
        numberOfOpenParenthesis, direction);
  }

  // Layout modification
  int indexOfChildToPointToWhenInserting() {
    return node()->indexOfChildToPointToWhenInserting();
  }
  bool createGraySquaresAfterEnteringGrid(OLayout layoutToExclude = OLayout()) {
    return node()->createGraySquaresAfterEnteringGrid(layoutToExclude);
  }
  bool deleteGraySquaresBeforeLeavingGrid(OLayout layoutToExclude = OLayout()) {
    return node()->deleteGraySquaresBeforeLeavingGrid(layoutToExclude);
  }

  // Tree
  OLayout childAtIndex(int i) const;
  OLayout root() const {
    assert(!isUninitialized());
    return OLayout(node()->root());
  }
  OLayout parent() const {
    assert(!isUninitialized());
    return OLayout(node()->parent());
  }

  // Replace strings with codepoints
  OLayout makeEditable() { return node()->makeEditable(); }

  // Cursor move
  int indexAfterHorizontalCursorMove(OMG::HorizontalDirection direction,
                                     int currentIndex,
                                     bool *shouldRedrawLayout) {
    return node()->indexAfterHorizontalCursorMove(direction, currentIndex,
                                                  shouldRedrawLayout);
  }
  int indexAfterVerticalCursorMove(
      OMG::VerticalDirection direction, int currentIndex,
      LayoutNode::PositionInLayout positionAtCurrentIndex,
      bool *shouldRedrawLayout) {
    return node()->indexAfterVerticalCursorMove(
        direction, currentIndex, positionAtCurrentIndex, shouldRedrawLayout);
  }

  // Cursor deletion
  LayoutNode::DeletionMethod deletionMethodForCursorLeftOfChild(
      int childIndex) const {
    return node()->deletionMethodForCursorLeftOfChild(childIndex);
  }

 private:
  bool privateHasTopLevelComparisonSymbol(bool includingNotEqualSymbol) const;
};

template <typename T, typename U, int N, typename Parent>
class LayoutBuilder : public Parent {
 public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<OLayout, Parent>::value);
#endif
  static T Builder() {
    static_assert(N == 0);
    TreeHandle h =
        TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {});
    return static_cast<T &>(h);
  }
  static T Builder(OLayout child) {
    static_assert(N == 1);
    TreeHandle h =
        TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child});
    return static_cast<T &>(h);
  }
  static T Builder(OLayout child1, OLayout child2) {
    static_assert(N == 2);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2});
    return static_cast<T &>(h);
  }
  static T Builder(OLayout child1, OLayout child2, OLayout child3) {
    static_assert(N == 3);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U),
                                                   {child1, child2, child3});
    return static_cast<T &>(h);
  }
  static T Builder(OLayout child1, OLayout child2, OLayout child3,
                   OLayout child4) {
    static_assert(N == 4);
    TreeHandle h = TreeHandle::BuilderWithChildren(
        Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    return static_cast<T &>(h);
  }
};

template <typename T, typename U, typename P = OLayout>
using LayoutNoChildren = LayoutBuilder<T, U, 0, P>;
template <typename T, typename U, typename P = OLayout>
using LayoutOneChild = LayoutBuilder<T, U, 1, P>;
template <typename T, typename U, typename P = OLayout>
using LayoutTwoChildren = LayoutBuilder<T, U, 2, P>;
template <typename T, typename U, typename P = OLayout>
using LayoutThreeChildren = LayoutBuilder<T, U, 3, P>;
template <typename T, typename U, typename P = OLayout>
using LayoutFourChildren = LayoutBuilder<T, U, 4, P>;

}  // namespace Poincare

#endif

// TODO remove this hack
#include "junior_layout.h"
