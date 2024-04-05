#include <assert.h>
#include <poincare/junior_layout.h>
#include <poincare/k_tree.h>
#include <poincare_junior/src/layout/app_helpers.h>
#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/layout/layout_cursor.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/layout/render.h>
#include <poincare_junior/src/layout/serialize.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

#include <algorithm>

namespace Poincare {

JuniorLayoutNode::JuniorLayoutNode(const PoincareJ::Tree* tree,
                                   size_t treeSize) {
  memcpy(m_blocks, tree->block(), treeSize);
}

size_t JuniorLayoutNode::size() const {
  return sizeof(JuniorLayoutNode) + tree()->treeSize();
}

#if POINCARE_TREE_LOG
void JuniorLayoutNode::logAttributes(std::ostream& stream) const {
  stream << '\n';
  tree()->log(stream);
}
#endif

KDSize JuniorLayoutNode::computeSize(KDFont::Size font) const {
  return PoincareJ::Render::Size(tree(), font);
}

KDCoordinate JuniorLayoutNode::computeBaseline(KDFont::Size font) const {
  return PoincareJ::Render::Baseline(tree(), font);
}

void JuniorLayoutNode::render(KDContext* ctx, KDPoint p,
                              KDGlyph::Style style) const {
  PoincareJ::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                          style.backgroundColor);
}

size_t JuniorLayoutNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return PoincareJ::Serialize(tree(), buffer, buffer + bufferSize) - buffer;
}

bool JuniorLayoutNode::protectedIsIdenticalTo(OLayout l) const {
  /* TODO PCJ have a comparison with a flag to ignore separators similar to what
   * isIdenticalTo(makeEditable=true)) was doing. */
  return tree()->treeIsIdenticalTo(static_cast<const JuniorLayout&>(l).tree());
}

const PoincareJ::Tree* JuniorLayoutNode::tree() const {
  return PoincareJ::Tree::FromBlocks(m_blocks);
}
PoincareJ::Tree* JuniorLayoutNode::tree() {
  return PoincareJ::Tree::FromBlocks(m_blocks);
}

JuniorLayout JuniorLayout::Builder(const PoincareJ::Tree* tree) {
  assert(PoincareJ::AppHelpers::IsSanitizedRack(tree));
  if (!tree) {
    return JuniorLayout();
  }
  size_t size = tree->treeSize();
  void* bufferNode = Pool::sharedPool->alloc(sizeof(JuniorLayoutNode) + size);
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
  PoincareJ::CodePointLayout::Push(cp);
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
      PoincareJ::TreeStack::SharedTreeStack
          ->push<PoincareJ::Type::CombinedCodePointsLayout>(cp, cc);
    } else {
      decoder.previousCodePoint();
      PoincareJ::CodePointLayout::Push(cp);
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
                            KDColor selectionColor) const {
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

bool JuniorLayout::isEmpty() const { return tree()->numberOfChildren() == 0; }

bool JuniorLayout::isCodePointsString() const {
  for (const PoincareJ::Tree* child : tree()->children()) {
    if (!(child->isCodePointLayout() || child->isCombinedCodePointsLayout())) {
      return false;
    }
  }
  return true;
}

}  // namespace Poincare
