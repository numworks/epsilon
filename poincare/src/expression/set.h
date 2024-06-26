#ifndef POINCARE_EXPRESSION_SET_H
#define POINCARE_EXPRESSION_SET_H

#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

class Set {
  /*
   * Set elements are always ordered and unique
   */
 public:
  static Tree* PushEmpty();
  static bool Includes(const Tree* set, const Tree* e);
  static void Add(Tree* set, const Tree* e);
  static Tree* Pop(Tree* set);
  static Tree* Union(Tree* set0, Tree* set1);
  static Tree* Intersection(Tree* set0, Tree* set1);
  static Tree* Difference(Tree* set0, Tree* set1);
};

}  // namespace Poincare::Internal

#endif
