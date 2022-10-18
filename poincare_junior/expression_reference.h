#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include "cache_reference.h"

namespace Poincare {

class ExpressionReference : public CacheReference {
public:
  using CacheReference::CacheReference;
  static ExpressionReference Parse(const char * text);
  static ExpressionReference CreateBasicReduction(void * treeAddress);
  float approximate(float x) const;
};

static_assert(sizeof(ExpressionReference) == sizeof(CacheReference));

}

#endif
