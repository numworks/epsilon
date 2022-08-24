#ifndef POINCARE_LAYOUT_REFERENCE_H
#define POINCARE_LAYOUT_REFERENCE_H

#include <poincare/context.h>
#include <poincare/layout_node.h>
#include <poincare/tree_handle.h>
#include <escher/palette.h>

namespace Poincare {

class LayoutCursor;
class Expression;

class Layout : public TreeHandle {
  friend class GridLayoutNode;
  friend class HorizontalLayoutNode;
  friend class LayoutNode;
  friend class LayoutCursor;
  friend class VerticalOffsetLayoutNode;
public:
  Layout() : TreeHandle() {}
  Layout(const LayoutNode * node) : TreeHandle(node) {}
  Layout clone() const;
  LayoutNode * node() const {
    assert(isUninitialized() || !TreeHandle::node()->isGhost());
    return static_cast<LayoutNode *>(TreeHandle::node());
  }
  static Layout LayoutFromAddress(const void * address, size_t size);

  // Properties
  LayoutNode::Type type() const { return node()->type(); }
  bool isIdenticalTo(Layout l, bool makeEditable = false) { return isUninitialized() ? l.isUninitialized() : node()->isIdenticalTo(l, makeEditable); }

  // Rendering
  void draw(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor = KDColorBlack, KDColor backgroundColor = KDColorWhite, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = Escher::Palette::Select) {
    return node()->draw(ctx, p, font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
  }
  void render(KDContext * ctx, KDPoint p, KDFont::Size font, KDColor expressionColor, KDColor backgroundColor, Layout * selectionStart = nullptr, Layout * selectionEnd = nullptr, KDColor selectionColor = KDColorRed) {
    return node()->render(ctx, p, font, expressionColor, backgroundColor, selectionStart, selectionEnd, selectionColor);
  }
  KDSize layoutSize(KDFont::Size font) const { return node()->layoutSize(font); }
  KDPoint absoluteOrigin(KDFont::Size font) const { return node()->absoluteOrigin(font); }
  KDCoordinate baseline(KDFont::Size font) { return node()->baseline(font); }
  void invalidAllSizesPositionsAndBaselines() { return node()->invalidAllSizesPositionsAndBaselines(); }

  // Serialization
  int serializeForParsing(char * buffer, int bufferSize) const { return node()->serialize(buffer, bufferSize); }
  int serializeParsedExpression(char * buffer, int bufferSize, Context * context) const;

  // Layout properties
  typedef bool (*LayoutTest)(const Layout l);
  Layout recursivelyMatches(LayoutTest test) const;
  bool mustHaveLeftSibling() const { return const_cast<Layout *>(this)->node()->mustHaveLeftSibling(); }
  bool isEmpty() const { return const_cast<Layout *>(this)->node()->isEmpty(); }
  bool isCollapsable(int * numberOfOpenParenthesis, bool goingLeft) const { return const_cast<Layout *>(this)->node()->isCollapsable(numberOfOpenParenthesis, goingLeft); }
  int leftCollapsingAbsorbingChildIndex() const { return const_cast<Layout *>(this)->node()->leftCollapsingAbsorbingChildIndex(); }
  int rightCollapsingAbsorbingChildIndex() const { return const_cast<Layout *>(this)->node()->rightCollapsingAbsorbingChildIndex(); }
  bool hasText() { return node()->hasText(); }
  Layout XNTLayout() const;
  bool displayEmptyLayouts() const { return type() == LayoutNode::Type::ParenthesisLayout || type() == LayoutNode::Type::CurlyBraceLayout; }

  // Layout modification
  void deleteBeforeCursor(LayoutCursor * cursor) { return node()->deleteBeforeCursor(cursor); }
  bool removeGraySquaresFromAllMatrixAncestors() { return node()->removeGraySquaresFromAllMatrixAncestors(); }
  bool removeGraySquaresFromAllMatrixChildren() { return node()->removeGraySquaresFromAllMatrixChildren(); }
  bool addGraySquaresToAllMatrixAncestors() { return node()->addGraySquaresToAllMatrixAncestors(); }
  Layout layoutToPointWhenInserting(Expression * correspondingExpression) {
    // Pointer to correspondingExpr because expression.h includes layout.h
    assert(correspondingExpression != nullptr);
    return Layout(node()->layoutToPointWhenInserting(correspondingExpression));
  }
  void setMargin(bool hasMargin) { node()->setMargin(hasMargin); }

  // Cursor
  LayoutCursor cursor() const;
  LayoutCursor equivalentCursor(LayoutCursor * cursor);

  // Tree
  Layout childAtIndex(int i) const;
  Layout root() const {
    assert(!isUninitialized());
    return Layout(node()->root());
  }
  Layout parent() const {
    assert(!isUninitialized());
    return Layout(node()->parent());
  }

  // Tree modification
  // Add
  void addSibling(LayoutCursor * cursor, Layout sibling, bool moveCursor);
  // Replace
  void replaceChild(Layout oldChild, Layout newChild, LayoutCursor * cursor = nullptr, bool force = false);
  void replaceChildWithEmpty(Layout oldChild, LayoutCursor * cursor = nullptr);
  void replaceWith(Layout newChild, LayoutCursor * cursor);
  void replaceWithJuxtapositionOf(Layout leftChild, Layout rightChild, LayoutCursor * cursor, bool putCursorInTheMiddle = false);
  // Collapse
  void collapseSiblings(LayoutCursor * cursor);
  // Replace strings with codepoints
  Layout makeEditable() { return node()->makeEditable(); }
protected:
  // Add
  void addChildAtIndex(Layout l, int index, int currentNumberOfChildren, LayoutCursor * cursor);
  // Remove
  void removeChild(Layout l, LayoutCursor * cursor, bool force = false);
  void removeChildAtIndex(int index, LayoutCursor * cursor, bool force = false);
private:
  // Tree modification
  enum class HorizontalDirection {
    Left,
    Right
  };
  void collapseOnDirection(HorizontalDirection direction, int absorbingChildIndex);
};

template<typename T, typename U, int N, typename Parent>
class LayoutBuilder : public Parent {
public:
#ifndef PLATFORM_DEVICE
  static_assert(std::is_base_of<Layout, Parent>::value);
#endif
  static T Builder() {
    static_assert(N == 0);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child) {
    static_assert(N == 1);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2) {
    static_assert(N == 2);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2, Layout child3) {
    static_assert(N == 3);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2, child3});
    return static_cast<T&>(h);
  }
  static T Builder(Layout child1, Layout child2, Layout child3, Layout child4) {
    static_assert(N == 4);
    TreeHandle h = TreeHandle::BuilderWithChildren(Initializer<U>, sizeof(U), {child1, child2, child3, child4});
    return static_cast<T&>(h);
  }
};

template<typename T, typename U, typename P = Layout> using LayoutNoChildren = LayoutBuilder<T,U,0,P>;
template<typename T, typename U, typename P = Layout> using LayoutOneChild = LayoutBuilder<T,U,1,P>;
template<typename T, typename U, typename P = Layout> using LayoutTwoChildren = LayoutBuilder<T,U,2,P>;
template<typename T, typename U, typename P = Layout> using LayoutThreeChildren = LayoutBuilder<T,U,3,P>;
template <typename T, typename U, typename P = Layout>
using LayoutFourChildren = LayoutBuilder<T, U, 4, P>;

}

#endif
