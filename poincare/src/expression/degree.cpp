#include "degree.h"

#include "advanced_simplification.h"
#include "integer.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"

using namespace Poincare::Internal;

// By default, degree is -1 unless all children have a 0 degree.
int Degree::PrivateGet(const Tree* t, const Tree* symbol) {
  switch (t->type()) {
    case Type::UserSymbol:
      return t->treeIsIdenticalTo(symbol) ? 1 : 0;
    case Type::Matrix:
    case Type::Store:
    case Type::UnitConversion:
      return k_unknown;
    case Type::Undef:
      /* Previously the return degree was unknown, but it was causing problems
       * in the equations of type `y = piecewise(x,x>0,undefined,x<=0)` since
       * the computed yDeg here was unknown instead of 0. */
    case Type::UserFunction:
      // Functions would have been replaced beforehand if it had a definition.
    case Type::NthDiff:
      // TODO: One could implement something like :
      // Deg(Diff(f(t), t, g(x)), x) = max(0, Deg(f(t), t) - 1) * Deg(g(x), x)
    default:
      break;
  }
  int degree = 0;
  for (uint8_t i = 0; const Tree* child : t->children()) {
    int childDegree = PrivateGet(child, symbol);
    if (childDegree == k_unknown) {
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
          if (childDegree != 0) {
            return k_unknown;
          }
          if (degree != 0) {
            if (!child->isInteger() ||
                ComplexSign::Get(child).realSign().canBeStrictlyNegative()) {
              return k_unknown;
            }
            // TODO: Check overflow
            degree *= Integer::Handler(child).to<uint8_t>();
          }
        }
        break;
      default:
        if (childDegree > 0) {
          // not handled
          return k_unknown;
        }
    }
    i++;
  }
  return degree;
}

int Degree::Get(const Tree* t, const Tree* symbol,
                ProjectionContext projectionContext) {
  assert(symbol->isUserSymbol());
  if (t->isStore() || t->isUnitConversion()) {
    return k_unknown;
  }
  // Project, simplify and expand the expression for a more accurate degree.
  Tree* clone = t->cloneTree();
  Simplification::ToSystem(clone, &projectionContext);
  Simplification::SimplifySystem(clone, false);
  AdvancedSimplification::DeepExpand(clone);
  int degree = PrivateGet(clone, symbol);
  clone->removeTree();
  return degree;
}
