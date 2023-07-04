#include "vertical_offset_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize VerticalOffsetLayout::Size(const Node* node, const Node* root,
                                  KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  KDSize indexSize = Render::Size(node->childAtIndex(0), root, font);
  const Node* base = BaseLayout(node, root);
  KDCoordinate baseHeight = base ? Render::Size(base, root, font).height()
                                 : KDFont::GlyphHeight(font);

  return KDSize(indexSize.width(),
                baseHeight - k_indiceHeight + indexSize.height());
}

KDCoordinate VerticalOffsetLayout::Baseline(const Node* node, const Node* root,
                                            KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  const Node* base = BaseLayout(node, root);
  KDCoordinate baseBaseline =
      base ? Render::Baseline(base, root, font) : KDFont::GlyphHeight(font) / 2;
  KDCoordinate indexHeight =
      Render::Size(node->childAtIndex(0), root, font).height();
  return indexHeight - k_indiceHeight + baseBaseline;
}

KDPoint VerticalOffsetLayout::PositionOfChild(const Node* node, int childIndex,
                                              const Node* parent,
                                              KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  return KDPointZero;
}

const Node* VerticalOffsetLayout::BaseLayout(const Node* node,
                                             const Node* root) {
  int index;
  const Node* parent = root->parentOfDescendant(node, &index);
  if (parent->type() != BlockType::RackLayout) {
    return nullptr;
  }
  assert(IsSuffixSuperscript(node));
  if (index == 0) {
    return nullptr;
  }
  return parent->childAtIndex(index - 1);
}

}  // namespace PoincareJ
