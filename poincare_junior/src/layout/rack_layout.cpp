#include "rack_layout.h"

#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/memory/node_iterator.h>
#include <poincare_junior/src/n_ary.h>

#include "empty_rectangle.h"

namespace PoincareJ {

KDSize RackLayout::Size(const Tree* node, const Tree* root, KDFont::Size font) {
  return SizeBetweenIndexes(node, 0, node->numberOfChildren(), root, font);
}

KDCoordinate RackLayout::Baseline(const Tree* node, const Tree* root,
                                  KDFont::Size font) {
  return BaselineBetweenIndexes(node, 0, node->numberOfChildren(), root, font);
}

KDSize RackLayout::SizeBetweenIndexes(const Tree* node, int leftIndex,
                                      int rightIndex, const Tree* root,
                                      KDFont::Size font) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    KDSize emptyRectangleSize = EmptyRectangle::RectangleSize(font);
    KDCoordinate width =
        ShouldDrawEmptyRectangle(node) ? emptyRectangleSize.width() : 0;
    return KDSize(width, emptyRectangleSize.height());
  }
  KDCoordinate totalWidth = 0;
  KDCoordinate maxUnderBaseline = 0;
  KDCoordinate maxAboveBaseline = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    const Tree* childi = node->childAtIndex(i);
    KDSize childSize = Render::Size(childi, root, font);
    totalWidth += childSize.width();
    KDCoordinate childBaseline = Render::Baseline(childi, root, font);
    maxUnderBaseline = std::max<KDCoordinate>(
        maxUnderBaseline, childSize.height() - childBaseline);
    maxAboveBaseline = std::max(maxAboveBaseline, childBaseline);
  }
  return KDSize(totalWidth, maxUnderBaseline + maxAboveBaseline);
}

KDCoordinate RackLayout::BaselineBetweenIndexes(const Tree* node, int leftIndex,
                                                int rightIndex,
                                                const Tree* root,
                                                KDFont::Size font) {
  assert(0 <= leftIndex && leftIndex <= rightIndex &&
         rightIndex <= node->numberOfChildren());
  if (node->numberOfChildren() == 0) {
    return EmptyRectangle::RectangleBaseLine(font);
  }
  KDCoordinate result = 0;
  for (int i = leftIndex; i < rightIndex; i++) {
    result =
        std::max(result, Render::Baseline(node->childAtIndex(i), root, font));
  }
  return result;
}

bool RackLayout::ShouldDrawEmptyRectangle(const Tree* node) {
  // TODO : complete this method
  return node->numberOfChildren() == 0;
}

void RackLayout::RenderNode(const Tree* node, const Tree* root, KDContext* ctx,
                            KDPoint p, KDFont::Size font,
                            KDColor expressionColor, KDColor backgroundColor) {
  if (ShouldDrawEmptyRectangle(node)) {
    EmptyRectangle::DrawEmptyRectangle(ctx, p, font,
                                       EmptyRectangle::Color::Yellow);
  }
}

int RackLayout::NumberOfLayouts(const Tree* node) {
  return Layout::IsHorizontal(node) ? node->numberOfChildren() : 1;
}

EditionReference RackLayout::AddOrMergeLayoutAtIndex(EditionReference reference,
                                                     EditionReference child,
                                                     int* index,
                                                     const Tree* root) {
  assert(*index <= NumberOfLayouts(reference));
  EditionReference nary = RackParent(reference, index, root);
  NAry::AddOrMergeChildAtIndex(nary, child, *index);
  return nary;
}

EditionReference RackLayout::RemoveLayoutAtIndex(EditionReference reference,
                                                 int* index, const Tree* root) {
  assert(*index <= NumberOfLayouts(reference));
  EditionReference nary = RackParent(reference, index, root);
  NAry::RemoveChildAtIndex(nary, *index);
  return nary;
}

// Return the nearest NAry
EditionReference RackLayout::RackParent(EditionReference reference, int* index,
                                        const Tree* root) {
  if (Layout::IsHorizontal(reference)) {
    return reference;
  }
  assert(*index <= 1);
  // Find or make a RackLayout parent
  int refIndex;
  EditionReference parent = root->parentOfDescendant(reference, &refIndex);
  if (parent.isUninitialized() || !Layout::IsHorizontal(parent)) {
    parent = SharedEditionPool->push<BlockType::RackLayout>(1);
    reference->moveNodeBeforeNode(parent);
  } else {
    // TODO : This is not supposed to happen with cursor layouts.
    // For now we do not take advantage of this :/
    assert(false);
    // Index of reference in parent may not be 0
    *index += refIndex;
  }
  return parent;
}

}  // namespace PoincareJ
