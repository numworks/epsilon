#include "degree.h"

#include "advanced_simplification.h"
#include "integer.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"

using namespace Poincare::Internal;

// By default, degree is -1 unless all children have a 0 degree.
int privateDegree(const Tree* t, const Tree* variable) {
  switch (t->type()) {
    case Type::UserSymbol:
      // Ignore UserSymbol's sign
      return strcmp(Symbol::GetName(t), Symbol::GetName(variable)) == 0 ? 1 : 0;
    case Type::Matrix:
    case Type::Store:
    case Type::UnitConversion:
      return -1;
    case Type::Undef:
      /* Previously the return value was -1, but it was causing problems in the
       * equations of type `y = piecewise(x,x>0,undefined,x<=0)` since the
       * computed yDeg here was -1 instead of 0. */
    case Type::UserFunction:
      // Functions would have been replaced beforehand if it had a definition.
    default:
      break;
  }
  int degree = 0;
  for (uint8_t i = 0; const Tree* child : t->children()) {
    int childDegree = privateDegree(child, variable);
    if (childDegree == -1) {
      return childDegree;
    }
    switch (t->type()) {
      case Type::Dependency:
        assert(i == 0);
        return childDegree;
      case Type::Add:
        degree = degree > childDegree ? degree : childDegree;
        break;
      case Type::Mult:
        // TODO: Check overflow
        degree += childDegree;
        break;
      case Type::PowReal:
      case Type::Pow:
        if (i == 0) {
          degree = childDegree;
        } else {
          assert(child->isInteger());
          if (childDegree > 0 ||
              ComplexSign::Get(child).realSign().canBeStriclyNegative()) {
            return -1;
          }
          // TODO: Check overflow
          degree *= Integer::Handler(child).to<uint8_t>();
        }
        break;
      default:
        if (childDegree > 0) {
          // not handled
          return -1;
        }
    }
    i++;
  }
  return degree;
}

int Degree::Get(const Tree* t, const Tree* variable,
                ProjectionContext projectionContext) {
  assert(variable->isUserSymbol());
  if (t->isStore() || t->isUnitConversion()) {
    return -1;
  }
  // Project, simplify and and expand the expression for a more accurate degree.
  Tree* clone = t->clone();
  Simplification::ToSystem(clone, &projectionContext);
  Simplification::SimplifySystem(clone, false);
  AdvancedSimplification::DeepExpand(clone);
  int degree = privateDegree(clone, variable);
  clone->removeTree();
  return degree;
}
