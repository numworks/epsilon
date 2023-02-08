#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare_junior/src/memory/cache_reference.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Expression;

class Layout final : public CacheReference {
friend class Expression;
public:
  using CacheReference::CacheReference;
  static Layout Parse(const char * text);
  Expression toExpression() const;
  void toText(char * buffer, size_t bufferSize) const;
private:
  static EditionReference EditionPoolTextToLayout(const char * text);
  static EditionReference EditionPoolLayoutToExpression(Node node);
};

static_assert(sizeof(Layout) == sizeof(CacheReference));

}

#endif
