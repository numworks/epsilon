#include <poincare/api.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layoutter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

/* JuniorPoolObject */

JuniorPoolObject::JuniorPoolObject(const Internal::Tree* tree,
                                   size_t treeSize) {
  memcpy(m_blocks, tree->block(), treeSize);
}

size_t JuniorPoolObject::size() const {
  return sizeof(JuniorPoolObject) + tree()->treeSize();
}

const Internal::Tree* JuniorPoolObject::tree() const {
  return Internal::Tree::FromBlocks(m_blocks);
}

/* JuniorPoolHandle */

JuniorPoolHandle JuniorPoolHandle::Builder(const Internal::Tree* tree) {
  if (!tree) {
    return JuniorPoolHandle();
  }
  size_t size = tree->treeSize();
  void* bufferNode = Pool::sharedPool->alloc(sizeof(JuniorPoolObject) + size);
  JuniorPoolObject* node = new (bufferNode) JuniorPoolObject(tree, size);
  PoolHandle h = PoolHandle::BuildWithGhostChildren(node);
  return static_cast<JuniorPoolHandle&>(h);
}

JuniorPoolHandle JuniorPoolHandle::Builder(Internal::Tree* tree) {
  JuniorPoolHandle result = Builder(const_cast<const Internal::Tree*>(tree));
  if (tree) {
    tree->removeTree();
  }
  return result;
}

/* NewLayout */

UserExpression NewLayout::parse() {
  // TODO: Pass context
  return UserExpression::Builder(Internal::Parser::Parse(layout, nullptr));
}

/* UserExpression */

NewLayout UserExpression::createLayout(bool linearMode) const {
  // TODO: Pass other optional parameters
  return NewLayout::Builder(
      Internal::Layoutter::LayoutExpression(tree()->clone(), linearMode));
}

SystemExpression UserExpression::projected() const {
  // TODO: Pass context.
  Internal::ProjectionContext context;
  return SystemExpression::Builder(
      Internal::Simplification::Simplify(tree()->clone(), context));
}

/* SystemExpression */
/* SystemFunction */
/* SystemParametricFunction */

}  // namespace Poincare
