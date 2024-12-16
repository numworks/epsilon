#include <assert.h>
#include <poincare/helpers/layout.h>
#include <poincare/k_tree.h>
#include <poincare/layout.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/layout_cursor.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/layout/render.h>
#include <poincare/src/layout/serialize.h>
#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

LayoutObject::LayoutObject(const Internal::Tree* tree, size_t treeSize) {
  memcpy(m_blocks, tree->block(), treeSize);
}

size_t LayoutObject::size() const {
  return sizeof(LayoutObject) + tree()->treeSize();
}

#if POINCARE_TREE_LOG
void LayoutObject::logAttributes(std::ostream& stream) const {
  stream << '\n';
  tree()->log(stream);
}
#endif

KDSize LayoutObject::computeSize(KDFont::Size font,
                                 const Internal::LayoutCursor* cursor) const {
  return Internal::Render::Size(
      tree(), font,
      cursor ? cursor->simpleCursor() : Internal::SimpleLayoutCursor());
}

KDCoordinate LayoutObject::computeBaseline(
    KDFont::Size font, const Internal::LayoutCursor* cursor) const {
  return Internal::Render::Baseline(
      tree(), font,
      cursor ? cursor->simpleCursor() : Internal::SimpleLayoutCursor());
}

size_t LayoutObject::serialize(char* buffer, size_t bufferSize,
                               Preferences::PrintFloatMode floatDisplayMode,
                               int numberOfSignificantDigits) const {
  return Internal::Serialize(tree(), buffer, buffer + bufferSize) - buffer;
}

bool LayoutObject::isIdenticalTo(Layout l, bool makeEditable) const {
  if (l.isUninitialized()) {
    return false;
  }
  if (identifier() == l.identifier()) {
    return true;
  }
  /* TODO_PCJ we should either ignore separators when makeEditable is true or
   * remove the flag completely. */
  return tree()->treeIsIdenticalTo(static_cast<const Layout&>(l).tree());
}

const Internal::Tree* LayoutObject::tree() const {
  return Internal::Tree::FromBlocks(m_blocks);
}
Internal::Tree* LayoutObject::tree() {
  return Internal::Tree::FromBlocks(m_blocks);
}

Layout Layout::Builder(const Internal::Tree* tree) {
  if (!tree) {
    return Layout();
  }
  assert(LayoutHelpers::IsSanitizedRack(tree));
  size_t size = tree->treeSize();
  void* bufferNode = Pool::sharedPool->alloc(sizeof(LayoutObject) + size);
  LayoutObject* node = new (bufferNode) LayoutObject(tree, size);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<Layout&>(h);
}

Layout Layout::Builder(Internal::Tree* tree) {
  Layout result = Builder(const_cast<const Internal::Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

Layout Layout::Create(const Internal::Tree* structure,
                      Internal::ContextTrees ctx) {
  Internal::Tree* tree = Internal::PatternMatching::Create(structure, ctx);
  LayoutHelpers::SanitizeRack(tree);
  return Builder(tree);
}

Layout Layout::CodePoint(::CodePoint cp) {
  Internal::Tree* tree = KRackL.node<1>->cloneNode();
  Internal::CodePointLayout::Push(cp);
  return Builder(tree);
}

Layout Layout::String(const char* str, int length) {
  Internal::Tree* tree = KRackL()->cloneTree();
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

Layout Layout::Parse(const char* string) {
  if (!string || string[0] == 0) {
    return Layout();
  }
  return Layout::Builder(Internal::RackFromText(string));
}

Layout Layout::Concatenate(Layout layout1, Layout layout2) {
  assert(!layout1.isUninitialized() && !layout2.isUninitialized());
  assert(layout1.tree()->isRackLayout() && layout2.tree()->isRackLayout());
  Internal::Tree* result = layout1.tree()->cloneTree();
  Internal::NAry::AddOrMergeChild(result, layout2.tree()->cloneTree());
  return Builder(result);
}

void Layout::draw(KDContext* ctx, KDPoint p, const LayoutStyle& style,
                  Internal::LayoutCursor* cursor) {
  object()->draw(ctx, p, style, cursor);
}

// Rendering

void LayoutObject::draw(KDContext* ctx, KDPoint p, const LayoutStyle& style,
                        Internal::LayoutCursor* cursor) const {
  Internal::Render::Draw(
      tree(), ctx, p, style,
      cursor ? cursor->simpleCursor() : Internal::SimpleLayoutCursor(),
      cursor ? cursor->selection() : Internal::LayoutSelection());
}

int Layout::numberOfDescendants(bool includeSelf) const {
  assert(tree());
  return tree()->numberOfDescendants(includeSelf);
}

Layout Layout::clone() const {
  if (isUninitialized()) {
    return Layout();
  }
  PoolHandle c = PoolHandle::clone();
  Layout cast = static_cast<Layout&>(c);
  cast->invalidAllSizesPositionsAndBaselines();
  return cast;
}

Layout Layout::cloneWithoutMargins() {
  Internal::Tree* clone = tree()->cloneTree();
  assert(clone->isRackLayout());
  Internal::Layouter::StripSeparators(clone);
  return Layout::Builder(clone);
}

Layout Layout::cloneWithoutChildrenRacks() {
  Internal::Tree* clone = tree()->cloneTree();
  assert(clone->isRackLayout());
  LayoutHelpers::DeleteChildrenRacks(clone);
  return Layout::Builder(clone);
}

bool Layout::isEmpty() const { return tree()->numberOfChildren() == 0; }

bool Layout::isCodePointsString() const {
  for (const Internal::Tree* child : tree()->children()) {
    if (!(child->isCodePointLayout() || child->isCombinedCodePointsLayout())) {
      return false;
    }
  }
  return true;
}

}  // namespace Poincare
