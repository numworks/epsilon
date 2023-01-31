#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare_junior/src/memory/cache_reference.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Layout;

class Expression final : public CacheReference {
public:
  using CacheReference::CacheReference;
  static EditionReference ParseFromLayoutInEditionPool(Node node);
  static Expression CreateExpressionFromText(const char * text);
  static Expression CreateExpressionFromLayout(const Layout * layout);
  static Expression CreateBasicReduction(void * treeAddress);
  float approximate(float x) const;
};

static_assert(sizeof(Expression) == sizeof(CacheReference));

}

#endif

