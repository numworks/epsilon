#ifndef POINCARE_EXPRESSION_H
#define POINCARE_EXPRESSION_H

#include "cache_reference.h"

namespace Poincare {

class Expression : public CacheReference {
public:
  using CacheReference::CacheReference;
  static Expression Parse(const char * text);
  static Expression CreateBasicReduction(void * treeAddress);
  float approximate(float x) const;
};

static_assert(sizeof(Expression) == sizeof(CacheReference));

}

#endif
