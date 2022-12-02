#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include <poincare_junior/src/memory/cache_reference.h>

namespace PoincareJ {

class Expression final : public CacheReference {
public:
  using CacheReference::CacheReference;
  static Expression Parse(const char * text);
  static Expression CreateBasicReduction(void * treeAddress);
  float approximate(float x) const;
};

static_assert(sizeof(Expression) == sizeof(CacheReference));

}

#endif

