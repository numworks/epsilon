#ifndef POINCARE_EXPRESSION_SEQUENCE_H
#define POINCARE_EXPRESSION_SEQUENCE_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Sequence {
 public:
  enum class Type : uint8_t {
    Explicit = 0,
    SingleRecurrence = 1,
    DoubleRecurrence = 2
  };

  static Type GetType(const Tree* sequence);
  static int InitialRank(const Tree* sequence);

  static bool MainExpressionContainsForbiddenTerms(
      const Tree* e, const char* name, Type type, int initialRank,
      bool recursion, bool systemSymbol, bool otherSequences);
};

}  // namespace Poincare::Internal
#endif
