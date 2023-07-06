#include "number.h"

#include "rational.h"

namespace PoincareJ {

EditionReference Number::Addition(const Tree* i, const Tree* j) {
  // TODO: handle Float
  return Rational::Addition(i, j);
}
EditionReference Number::Multiplication(const Tree* i, const Tree* j) {
  // TODO: handle Float
  return Rational::Multiplication(i, j);
}

}  // namespace PoincareJ
