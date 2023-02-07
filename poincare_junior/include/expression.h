#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare_junior/src/memory/cache_reference.h>
#include <poincare_junior/src/memory/edition_reference.h>
#include <poincare_junior/include/layout.h>

namespace PoincareJ {

class Expression final : public CacheReference {
public:
  Expression(const Node node) : CacheReference(node) { assert(node.block()->isExpression()); }
  using CacheReference::CacheReference;
  // TODO : Delete this method and adapt tests ?
  static Expression Parse(const char * text);
  static Expression Parse(const Layout * layout);
  static Expression CreateBasicReduction(void * treeAddress);
  Layout toLayout() const;
  float approximate(float x) const;
private:
  static EditionReference EditionPoolExpressionToLayout(Node node);
  static EditionReference EditionPoolLayoutToExpression(Node node);
};

static_assert(sizeof(Expression) == sizeof(CacheReference));

}

#endif

