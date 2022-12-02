#ifndef POINCARE_EXPRESSION_SET_H
#define POINCARE_EXPRESSION_SET_H

#include <poincare_junior/src/memory/edition_reference.h>

namespace Poincare {

class Set {
public:
  static bool Includes(EditionReference set, Node expression);
  static EditionReference Add(EditionReference set, Node expression);
  static EditionReference Union(EditionReference set0, EditionReference set1);
  static EditionReference Intersection(EditionReference set0, EditionReference set1);
  static EditionReference Difference(EditionReference set0, EditionReference set1);
};


}

#endif
