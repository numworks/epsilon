#ifndef POINCARE_LAYOUT_H
#define POINCARE_LAYOUT_H

#include <poincare_junior/src/memory/cache_reference.h>
#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Layout final : public CacheReference {
friend class Expression;
public:
  Layout(const Node node) : CacheReference(node) { assert(node.block()->isLayout()); }
  using CacheReference::CacheReference;
  static Layout Parse(const char * text);
  void toText(char * buffer, size_t bufferSize) const;
private:
  static EditionReference EditionPoolTextToLayout(const char * text);
};

static_assert(sizeof(Layout) == sizeof(CacheReference));

}

#endif
