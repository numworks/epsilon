#ifndef POINCARE_EXPRESSION_SET_H
#define POINCARE_EXPRESSION_SET_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace PoincareJ {

class Set {
  /*
   * Set elements are always ordered and unique
   */
 public:
  static Tree* PushEmpty();
  static bool Includes(const Tree* set, const Tree* expression);
  static void Add(Tree* set, const Tree* expression);
  static Tree* Pop(Tree* set);
  static EditionReference Union(EditionReference set0, EditionReference set1);
  static EditionReference Intersection(EditionReference set0,
                                       EditionReference set1);
  static EditionReference Difference(EditionReference set0,
                                     EditionReference set1);
};

}  // namespace PoincareJ

#endif
