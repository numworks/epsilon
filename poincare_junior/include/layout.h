#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare_junior/src/memory/cache_reference.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Expression;

class Layout final : public CacheReference {
public:
  using CacheReference::CacheReference;
  static EditionReference ParseFromTextInEditionPool(const char * text);
  static EditionReference ParseFromExpressionInEditionPool(Node node);
  static Layout CreateLayoutFromText(const char * text);
  static Layout CreateLayoutFromExpression(const Expression * expression);
  void layoutToBuffer(char * buffer, size_t bufferSize) const;
};

static_assert(sizeof(Layout) == sizeof(CacheReference));

}

#endif
