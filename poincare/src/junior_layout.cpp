#include <assert.h>
#include <poincare/junior_layout.h>
#include <poincare_junior/src/layout/app_helpers.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/layout/serialize.h>
#include <poincare_junior/src/n_ary.h>

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
  return PoincareJ::Serialize(tree(), buffer, buffer + bufferSize) - buffer;
}

bool JuniorLayoutNode::protectedIsIdenticalTo(OLayout l) {
  if (l.type() != LayoutNode::Type::JuniorLayout) {
    return false;
  }
  return tree()->treeIsIdenticalTo(static_cast<const JuniorLayout&>(l).tree());
}

JuniorLayout JuniorLayout::Builder(const PoincareJ::Tree* tree) {
  assert(PoincareJ::AppHelpers::IsSanitizedRack(tree));
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
  if (tree) {
    tree->removeTree();
  }
  return result;
}

JuniorLayout JuniorLayout::Create(const PoincareJ::Tree* structure,
                                  PoincareJ::ContextTrees ctx) {
  PoincareJ::Tree* tree = PoincareJ::PatternMatching::Create(structure, ctx);
  PoincareJ::AppHelpers::SanitizeRack(tree);
  return Builder(tree);
}

JuniorLayout JuniorLayout::CodePoint(::CodePoint cp) {
  PoincareJ::Tree* tree = KRackL.node<1>->cloneNode();
  PoincareJ::EditionPool::SharedEditionPool
      ->push<PoincareJ::BlockType::CodePointLayout>(cp);
  return Builder(tree);
}

JuniorLayout JuniorLayout::String(const char* str, int length) {
  PoincareJ::Tree* tree = KRackL()->clone();
  UTF8Decoder decoder(str);
  int n = 0;
  ::CodePoint cp = 0;
  // TODO decoder could yield glyphs
  while (n != length && (cp = decoder.nextCodePoint())) {
    ::CodePoint cc = 0;
    if (n + 1 != length && (cc = decoder.nextCodePoint()) && cc.isCombining()) {
      PoincareJ::EditionPool::SharedEditionPool
          ->push<PoincareJ::BlockType::CombinedCodePointsLayout>(cp, cc);
    } else {
      decoder.previousCodePoint();
      PoincareJ::EditionPool::SharedEditionPool
          ->push<PoincareJ::BlockType::CodePointLayout>(cp);
    }
    n++;
  }
  PoincareJ::NAry::SetNumberOfChildren(tree, n);
  return Builder(tree);
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
