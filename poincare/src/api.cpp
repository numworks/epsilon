#include <poincare/api.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::API {

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

#if 0
/* NewLayout */

UserExpression NewLayout::parse() {
  // TODO: Pass context
  return UserExpression::Builder(Internal::Parser::Parse(tree(), nullptr));
}

#endif
/* UserExpression */

UserExpression UserExpression::Builder(const Internal::Tree* tree) {
  JuniorPoolHandle result = JuniorPoolHandle::Builder(tree);
  return static_cast<UserExpression&>(result);
}

UserExpression UserExpression::Builder(Internal::Tree* tree) {
  JuniorPoolHandle result = JuniorPoolHandle::Builder(tree);
  return static_cast<UserExpression&>(result);
}

Layout UserExpression::createLayout(bool linearMode) const {
  // TODO: Pass other optional parameters
  return Layout::Builder(
      Internal::Layouter::LayoutExpression(tree()->cloneTree(), linearMode));
}

SystemExpression UserExpression::projected() const {
  // TODO: Pass context.
  Internal::ProjectionContext context;
  // TODO: Handle Store and UnitConversion like in Simplification::Simplify.
  Internal::Tree* e = tree()->cloneTree();
  Internal::Simplification::PrepareForProjection(e, &context);
  Internal::Simplification::ToSystem(e, &context);
  Internal::Simplification::ReduceSystem(e, false);
  Internal::Simplification::TryApproximationStrategyAgain(e, context);
  return SystemExpression::Builder(e, context.m_dimension.unit.vector);
}

/* SystemExpression */

SystemExpression SystemExpression::Builder(
    const Internal::Tree* tree, Internal::Units::SIVector dimension) {
  JuniorPoolHandle result = JuniorPoolHandle::Builder(tree);
  static_cast<SystemExpression&>(result).setDimension(dimension);
  return static_cast<SystemExpression&>(result);
}

SystemExpression SystemExpression::Builder(
    Internal::Tree* tree, Internal::Units::SIVector dimension) {
  JuniorPoolHandle result = JuniorPoolHandle::Builder(tree);
  static_cast<SystemExpression&>(result).setDimension(dimension);
  return static_cast<SystemExpression&>(result);
}

UserExpression SystemExpression::beautified() const {
  // TODO: Pass context.
  Internal::ProjectionContext context;
  context.m_dimension.unit.vector = m_dimension;
  Internal::Tree* e = tree()->cloneTree();
  Internal::Beautification::DeepBeautify(e, context);
  return UserExpression::Builder(e);
}

/* SystemFunction */
/* SystemParametricFunction */

}  // namespace Poincare::API
