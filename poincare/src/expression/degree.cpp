#include "degree.h"

#include "advanced_reduction.h"
#include "integer.h"
#include "sign.h"
#include "simplification.h"
#include "symbol.h"
#include "systematic_reduction.h"

using namespace Poincare::Internal;

// By default, degree is -1 unless all children have a 0 degree.
int Degree::PrivateGet(const Tree* e, const Tree* symbol) {
  switch (e->type()) {
    case Type::UserSymbol:
      return e->treeIsIdenticalTo(symbol) ? 1 : 0;
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
    case Type::Diff:
      // TODO: One could implement something like :
      // Deg(Diff(f(t), t, g(x)), x) = max(0, Deg(f(t), t) - 1) * Deg(g(x), x)
    default:
      break;
  }
  int degree = 0;
  for (uint8_t i = 0; const Tree* child : e->children()) {
    int childDegree = PrivateGet(child, symbol);
    if (childDegree == k_unknown) {
      return childDegree;
    }
    if (childDegree >= k_maxPolynomialDegree) {
      /* Early return the maximum authorized value, this also avoids potential
       * overflows when adding or multiplying degrees */
      return k_maxPolynomialDegree;
    }
    switch (e->type()) {
      case Type::Dep:
        assert(i == 0);
        return childDegree;
      case Type::Add:
        degree = degree > childDegree ? degree : childDegree;
        break;
      case Type::Mult:
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
                GetComplexSign(child).realSign().canBeStrictlyNegative()) {
              return k_unknown;
            }
            IntegerHandler handler = Integer::Handler(child);
            if (!handler.is<int>()) {
              return k_maxPolynomialDegree;
            }
            int power = handler.to<int>();
            if (power >= k_maxPolynomialDegree) {
              /* Early return the maximum authorized value to avoid an overflow.
               */
              return k_maxPolynomialDegree;
            }
            degree *= power;
          }
        }
        break;
      default:
        if (childDegree > 0) {
          // not handled
          return k_unknown;
        }
    }
    if (degree >= k_maxPolynomialDegree) {
      /* Early return the maximum authorized value, this also avoids potential
       * overflows when adding or multiplying degrees */
      return k_maxPolynomialDegree;
    }
    i++;
  }
  return degree;
}

int Degree::Get(const Tree* e, const Tree* symbol) {
  assert(Simplification::IsSystem(e));
  assert(symbol->isUserSymbol());
  if (e->isStore() || e->isUnitConversion()) {
    return k_unknown;
  }
  // Expand the expression for a more accurate degree.
  Tree* clone = e->cloneTree();
  SystematicReduction::DeepReduce(clone);
  AdvancedReduction::DeepExpandAlgebraic(clone);
  int degree = PrivateGet(clone, symbol);
  clone->removeTree();
  return degree;
}

int Degree::Get(const Tree* e, const char* symbolName) {
  Tree* symbol = SharedTreeStack->pushUserSymbol(symbolName);
  int degree = Degree::Get(e, symbol);
  symbol->removeTree();
  return degree;
}
