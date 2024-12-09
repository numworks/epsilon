#ifndef POINCARE_EXPRESSION_SYMBOL_H
#define POINCARE_EXPRESSION_SYMBOL_H

#include <poincare/old/context.h>
#include <poincare/sign.h>

#include "context.h"
#include "k_tree.h"

namespace Poincare::Internal {

class Symbol final {
 public:
  constexpr static int k_maxSymbolReplacementsCount = 10;

  static uint8_t Length(const Tree* e) {
    assert(e->isUserNamed());
    return e->nodeValue(0) - 1;
  }
  static char* CopyName(const Tree* e, char* buffer, size_t bufferSize);
  static const char* GetName(const Tree* e);

  static ComplexSign GetComplexSign(const Tree* e);

  static bool InvolvesCircularity(const Tree* e, Poincare::Context* context);
};

}  // namespace Poincare::Internal

#endif
