#include "vertical_offset_layout.h"

#include "parser.h"

namespace PoincareJ {

KDSize VerticalOffsetLayout::Size(const Tree* node, const Tree* root,
                                  KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  KDSize indexSize = Render::Size(node->childAtIndex(0), root, font);
  const Tree* base = BaseLayout(node, root);
  KDCoordinate baseHeight = base ? Render::Size(base, root, font).height()
                                 : KDFont::GlyphHeight(font);

  return KDSize(indexSize.width(),
                baseHeight - k_indiceHeight + indexSize.height());
}

KDCoordinate VerticalOffsetLayout::Baseline(const Tree* node, const Tree* root,
                                            KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  const Tree* base = BaseLayout(node, root);
  KDCoordinate baseBaseline =
      base ? Render::Baseline(base, root, font) : KDFont::GlyphHeight(font) / 2;
  KDCoordinate indexHeight =
      Render::Size(node->childAtIndex(0), root, font).height();
  return indexHeight - k_indiceHeight + baseBaseline;
}

KDPoint VerticalOffsetLayout::PositionOfChild(const Tree* node, int childIndex,
                                              const Tree* parent,
                                              KDFont::Size font) {
  assert(IsSuffixSuperscript(node));
  return KDPointZero;
}

const Tree* VerticalOffsetLayout::BaseLayout(const Tree* node,
                                             const Tree* root) {
  int index;
  const Tree* parent = root->parentOfDescendant(node, &index);
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
