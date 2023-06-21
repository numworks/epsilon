#include "vertical_offset_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize VerticalOffsetLayout::Size(const Node* node, KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  KDSize indexSize = Render::Size(node.childAtIndex(0), font);
  const Node* base = BaseLayout(node);
  KDCoordinate baseHeight = base.isUninitialized()
                                ? KDFont::GlyphHeight(font)
                                : Render::Size(base, font).height();
  return KDSize(indexSize.width(),
                baseHeight - k_indiceHeight + indexSize.height());
}

KDCoordinate VerticalOffsetLayout::Baseline(const Node* node,
                                            KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  const Node* base = BaseLayout(node);
  KDCoordinate baseBaseline = base.isUninitialized()
                                  ? KDFont::GlyphHeight(font) / 2
                                  : Render::Baseline(base, font);
  KDCoordinate indexHeight = Render::Size(node.childAtIndex(0), font).height();
  return indexHeight - k_indiceHeight + baseBaseline;
}

KDPoint VerticalOffsetLayout::PositionOfChild(const Node* node, int childIndex,
                                              KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  return KDPointZero;
}

const Node* VerticalOffsetLayout::BaseLayout(const Node* node) {
  const Node* parent = node.parent();
  if (parent.type() != BlockType::RackLayout) {
    return Node * ();
  }
  assert(IsSuffixSuperscript(node));
  const Node* previousNode = node.previousTree();
  if (previousNode == parent) {
    return Node * ();
  }
  return previousNode;
}

}  // namespace PoincareJ
