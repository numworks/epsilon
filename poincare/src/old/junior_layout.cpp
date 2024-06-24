#include <assert.h>
#include <poincare/k_tree.h>
#include <poincare/old/junior_layout.h>
#include <poincare/src/layout/app_helpers.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/layout_cursor.h>
#include <poincare/src/layout/layoutter.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/render.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

#include <algorithm>

namespace Poincare {

JuniorLayoutNode::JuniorLayoutNode(const Internal::Tree* tree,
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
  return Internal::Render::Size(tree(), font);
}

KDCoordinate JuniorLayoutNode::computeBaseline(KDFont::Size font) const {
  return Internal::Render::Baseline(tree(), font);
}

void JuniorLayoutNode::render(KDContext* ctx, KDPoint p,
                              KDGlyph::Style style) const {
  Internal::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                         style.backgroundColor);
}

size_t JuniorLayoutNode::serialize(char* buffer, size_t bufferSize,
                                   Preferences::PrintFloatMode floatDisplayMode,
                                   int numberOfSignificantDigits) const {
  return Internal::Serialize(tree(), buffer, buffer + bufferSize) - buffer;
}

bool JuniorLayoutNode::protectedIsIdenticalTo(OLayout l) const {
  /* TODO_PCJ have a comparison with a flag to ignore separators similar to what
   * isIdenticalTo(makeEditable=true)) was doing. */
  return tree()->treeIsIdenticalTo(static_cast<const JuniorLayout&>(l).tree());
}

const Internal::Tree* JuniorLayoutNode::tree() const {
  return Internal::Tree::FromBlocks(m_blocks);
}
Internal::Tree* JuniorLayoutNode::tree() {
  return Internal::Tree::FromBlocks(m_blocks);
}

JuniorLayout JuniorLayout::Builder(const Internal::Tree* tree) {
  if (!tree) {
    return JuniorLayout();
  }
  assert(Internal::AppHelpers::IsSanitizedRack(tree));
  size_t size = tree->treeSize();
  void* bufferNode = Pool::sharedPool->alloc(sizeof(JuniorLayoutNode) + size);
  JuniorLayoutNode* node = new (bufferNode) JuniorLayoutNode(tree, size);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorLayout&>(h);
}

JuniorLayout JuniorLayout::Builder(Internal::Tree* tree) {
  JuniorLayout result = Builder(const_cast<const Internal::Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

JuniorLayout JuniorLayout::Create(const Internal::Tree* structure,
                                  Internal::ContextTrees ctx) {
  Internal::Tree* tree = Internal::PatternMatching::Create(structure, ctx);
  Internal::AppHelpers::SanitizeRack(tree);
  return Builder(tree);
}

JuniorLayout JuniorLayout::CodePoint(::CodePoint cp) {
  Internal::Tree* tree = KRackL.node<1>->cloneNode();
  Internal::CodePointLayout::Push(cp);
  return Builder(tree);
}

JuniorLayout JuniorLayout::String(const char* str, int length) {
  Internal::Tree* tree = KRackL()->clone();
  UTF8Decoder decoder(str);
  int n = 0;
  ::CodePoint cp = 0;
  // TODO decoder could yield glyphs
  while (n != length && (cp = decoder.nextCodePoint())) {
    ::CodePoint cc = 0;
    if (n + 1 != length && (cc = decoder.nextCodePoint()) && cc.isCombining()) {
      Internal::TreeStack::SharedTreeStack->pushCombinedCodePointsLayout(cp,
                                                                         cc);
    } else {
      decoder.previousCodePoint();
      Internal::CodePointLayout::Push(cp);
    }
    n++;
  }
  Internal::NAry::SetNumberOfChildren(tree, n);
  return Builder(tree);
}

JuniorLayout JuniorLayout::Parse(const char* string) {
  if (!string || string[0] == 0) {
    return JuniorLayout();
  }
  return JuniorLayout::Builder(Internal::RackFromText(string));
}

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                        Internal::LayoutCursor* cursor,
                        KDColor selectionColor) {
  node()->draw(ctx, p, style, cursor, selectionColor);
}

void JuniorLayout::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style) {
  draw(ctx, p, style, nullptr);
}

// Rendering

void JuniorLayoutNode::draw(KDContext* ctx, KDPoint p, KDGlyph::Style style,
                            Internal::LayoutCursor* cursor,
                            KDColor selectionColor) const {
  Internal::Render::Draw(tree(), ctx, p, style.font, style.glyphColor,
                         style.backgroundColor, cursor);
}

JuniorLayout JuniorLayout::cloneWithoutMargins() {
  Internal::Tree* clone = tree()->clone();
  assert(clone->isRackLayout());
  Internal::Layoutter::StripSeparators(clone);
  return JuniorLayout::Builder(clone);
}

JuniorLayout JuniorLayout::cloneWithoutChildrenRacks() {
  Internal::Tree* clone = tree()->clone();
  assert(clone->isRackLayout());
  Internal::AppHelpers::DeleteChildrenRacks(clone);
  return JuniorLayout::Builder(clone);
}

bool JuniorLayout::isEmpty() const { return tree()->numberOfChildren() == 0; }

bool JuniorLayout::isCodePointsString() const {
  for (const Internal::Tree* child : tree()->children()) {
    if (!(child->isCodePointLayout() || child->isCombinedCodePointsLayout())) {
      return false;
    }
  }
  return true;
}

}  // namespace Poincare
