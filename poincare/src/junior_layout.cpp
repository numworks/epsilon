#include <assert.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/src/layout/conversion.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/render.h>

#include <algorithm>

namespace Poincare {

KDSize JuniorLayoutNode::computeSize(KDFont::Size font) {
  return PoincareJ::Render::Size(tree(), font);
}

KDCoordinate JuniorLayoutNode::computeBaseline(KDFont::Size font) {
  return PoincareJ::Render::Baseline(tree(), font);
}

void JuniorLayoutNode::render(KDContext* ctx, KDPoint p, KDGlyph::Style style) {
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                          style.backgroundColor);
}

size_t JuniorLayoutNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  OLayout l = PoincareJ::ToPoincareLayout(tree());
  return l.node()->serialize(buffer, bufferSize, floatDisplayMode,
                             numberOfSignificantDigits);
}

OLayout JuniorLayoutNode::makeEditable() {
  return JuniorLayout(this).cloneWithoutMargins();
}

bool JuniorLayoutNode::protectedIsIdenticalTo(OLayout l) {
  if (l.type() != LayoutNode::Type::JuniorLayout) {
    return false;
  }
  return tree()->treeIsIdenticalTo(static_cast<const JuniorLayout&>(l).tree());
}

JuniorLayout JuniorLayout::Builder(const PoincareJ::Tree* tree) {
  if (!tree) {
    return JuniorLayout();
  }
  size_t size = tree->treeSize();
  void* bufferNode =
      TreePool::sharedPool->alloc(sizeof(JuniorLayoutNode) + size);
  JuniorLayoutNode* node = new (bufferNode) JuniorLayoutNode(tree, size);
  TreeHandle h = TreeHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorLayout&>(h);
}

JuniorLayout JuniorLayout::Builder(PoincareJ::Tree* tree) {
  JuniorLayout result = Builder(const_cast<const PoincareJ::Tree*>(tree));
  tree->removeTree();
  return result;
}

JuniorLayout JuniorLayout::Create(const PoincareJ::Tree* structure,
                                  PoincareJ::ContextTrees ctx) {
  return Builder(PoincareJ::PatternMatching::Create(structure, ctx));
}

JuniorLayout JuniorLayout::Juniorize(OLayout l) {
  if (l.isUninitialized() || l.type() == LayoutNode::Type::JuniorLayout) {
    // l is already a junior layout
    return static_cast<JuniorLayout&>(l);
  }
  return Builder(PoincareJ::FromPoincareLayout(l));
}

OLayout JuniorLayout::UnJuniorize(JuniorLayout l) {
  return PoincareJ::ToPoincareLayout(l.tree());
}

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                        PoincareJ::LayoutCursor* cursor,
                        KDColor selectionColor) {
  node()->draw(ctx, p, style, cursor, selectionColor);
}

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style) {
  draw(ctx, p, style, nullptr);
}

// Rendering

void JuniorLayoutNode::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                            PoincareJ::LayoutCursor* cursor,
                            KDColor selectionColor) {
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                          style.backgroundColor, cursor);
}

JuniorLayout JuniorLayout::cloneWithoutMargins() {
  PoincareJ::Tree* clone = tree()->clone();
  if (clone->isRackLayout()) {
    PoincareJ::Layoutter::StripSeparators(clone);
  }
  return JuniorLayout::Builder(clone);
}

}  // namespace Poincare
