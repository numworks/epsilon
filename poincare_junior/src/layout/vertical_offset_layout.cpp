#include "vertical_offset_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize VerticalOffsetLayout::Size(const Node* node, KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  KDSize indexSize = Render::Size(node->childAtIndex(0), font);
  const Node* base = BaseLayout(node);
  KDCoordinate baseHeight =
      base ? Render::Size(base, font).height() : KDFont::GlyphHeight(font);

  return KDSize(indexSize.width(),
                baseHeight - k_indiceHeight + indexSize.height());
}

KDCoordinate VerticalOffsetLayout::Baseline(const Node* node,
                                            KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  const Node* base = BaseLayout(node);
  KDCoordinate baseBaseline =
      base ? Render::Baseline(base, font) : KDFont::GlyphHeight(font) / 2;
  KDCoordinate indexHeight = Render::Size(node->childAtIndex(0), font).height();
  return indexHeight - k_indiceHeight + baseBaseline;
}

KDPoint VerticalOffsetLayout::PositionOfChild(const Node* node, int childIndex,
                                              KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  return KDPointZero;
}

const Node* VerticalOffsetLayout::BaseLayout(const Node* node) {
  const Node* parent = node->parent();
  if (parent->type() != BlockType::RackLayout) {
    return nullptr;
  }
  assert(IsSuffixSuperscript(node));
  const Node* previousNode = node->previousTree();
  if (previousNode == parent) {
    return nullptr;
  }
  return previousNode;
}

}  // namespace PoincareJ
