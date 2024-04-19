#include "beautification.h"

#include <poincare/src/memory/n_ary.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>

#include "advanced_simplification.h"
#include "angle.h"
#include "approximation.h"
#include "arithmetic.h"
#include "context.h"
#include "float.h"
#include "number.h"
#include "rational.h"
#include "sign.h"
#include "simplification.h"
#include "variables.h"

namespace Poincare::Internal {

float Beautification::DegreeForSortingAddition(const Tree* expr,
                                               bool symbolsOnly) {
  switch (expr->type()) {
    case Type::Mult: {
      /* If we consider the symbol degree, the degree of a multiplication is
       * the sum of the degrees of its terms :
       * 3*(x^2)*y -> deg = 0+2+1 = 3.
       *
       * If we consider the degree of any term, we choose that the degree of a
       * multiplication is the degree of the most-right term :
       * 4*sqrt(2) -> deg = 0.5.
       *
       * This is to ensure that deg(5) > deg(5*sqrt(3)) and deg(x^4) >
       * deg(x*y^3)
       * */
      if (symbolsOnly) {
        float degree = 0.;
        for (const Tree* c : expr->children()) {
          degree += DegreeForSortingAddition(c, symbolsOnly);
        }
        return degree;
      }
      assert(expr->numberOfChildren() > 0);
      return DegreeForSortingAddition(expr->lastChild(), symbolsOnly);
    }
    case Type::Pow: {
      double baseDegree = DegreeForSortingAddition(expr->child(0), symbolsOnly);
      if (baseDegree == 0.) {
        /* We escape here so that even if the exponent is not a number,
         * the degree is still computed to 0.
         * It is useful for 2^ln(3) for example, which has a symbol degree
         * of 0 even if the exponent is not a number.*/
        return 0.;
      }
      const Tree* index = expr->child(1);
      if (index->isNumber()) {
        return Approximation::To<float>(index) * baseDegree;
      }
      return NAN;
    }
    case Type::UserSymbol:
    case Type::Var:
      return 1.;
    default:
      return symbolsOnly ? 0. : 1.;
  }
}

Tree* Factor(Tree* expr, int index) {
  if (expr->isMult()) {
    return expr->child(index);
  }
  return expr;
}

const Tree* Factor(const Tree* expr, int index) {
  if (expr->isMult()) {
    return expr->child(index);
  }
  return expr;
}

int NumberOfFactors(const Tree* expr) {
  if (expr->isMult()) {
    return expr->numberOfChildren();
  }
  return 1;
}

bool MakePositiveAnyNegativeNumeralFactor(Tree* expr) {
  // The expression is a negative number
  Tree* factor = Factor(expr, 0);
  if (factor->isMinusOne() && expr->isMult()) {
    NAry::RemoveChildAtIndex(expr, 0);
    NAry::SquashIfUnary(expr);
    return true;
  }
  return factor->isNumber() && Number::SetSign(factor, NonStrictSign::Positive);
}

bool Beautification::SplitMultiplication(const Tree* expr, TreeRef& numerator,
                                         TreeRef& denominator) {
  bool result = false;
  numerator = SharedTreeStack->push<Type::Mult>(0);
  denominator = SharedTreeStack->push<Type::Mult>(0);
  // TODO replace NumberOfFactors and Factor with an iterable
  const int numberOfFactors = NumberOfFactors(expr);
  for (int i = 0; i < numberOfFactors; i++) {
    const Tree* factor = Factor(expr, i);
    TreeRef factorsNumerator;
    TreeRef factorsDenominator;
    if (factor->isRational()) {
      if (factor->isOne()) {
        // Special case: add a unary numeral factor if r = 1
        factorsNumerator = factor->clone();
      } else {
        IntegerHandler rNum = Rational::Numerator(factor);
        if (rNum.isMinusOne()) {
          result = !result;
        } else if (!rNum.isOne()) {
          factorsNumerator = rNum.pushOnTreeStack();
        }
        IntegerHandler rDen = Rational::Denominator(factor);
        if (!rDen.isOne()) {
          factorsDenominator = rDen.pushOnTreeStack();
        }
      }
    } else if (factor->isPow() || factor->isPowReal()) {
      Tree* pow = factor->clone();
      if (!pow->child(0)->isUnit() &&
          MakePositiveAnyNegativeNumeralFactor(pow->child(1))) {
        if (pow->child(1)->isOne()) {
          pow->moveTreeOverTree(pow->child(0));
        }
        factorsDenominator = pow;
      } else {
        factorsNumerator = pow;
      }
    } else {
      factorsNumerator = factor->clone();
    }
    if (factorsDenominator) {
      NAry::AddChild(denominator, factorsDenominator);
    }
    if (factorsNumerator) {
      NAry::AddChild(numerator, factorsNumerator);
    }
  }
  NAry::SquashIfEmpty(numerator) || NAry::SquashIfUnary(numerator);
  NAry::SquashIfEmpty(denominator) || NAry::SquashIfUnary(denominator);
  return result;
}

bool Beautification::BeautifyIntoDivision(Tree* expr) {
  TreeRef num;
  TreeRef den;
  if (SplitMultiplication(expr, num, den)) {
    expr->cloneNodeBeforeNode(KOpposite);
    expr = expr->child(0);
  }
  if (!den->isOne()) {
    num->cloneNodeAtNode(KDiv);
    expr->moveTreeOverTree(num);
    return true;
  } else {
    num->removeTree();
    den->removeTree();
  }
  return false;
}

/* TODO_PCJ: Added temperature unit used to depend on the input (5°C should
 *           output 5°C, 41°F should output 41°F). */
bool Beautification::AddUnits(Tree* expr, ProjectionContext projectionContext) {
  Units::DimensionVector dimension = projectionContext.m_dimension.unit.vector;
  if (!projectionContext.m_dimension.isUnit()) {
    return false;
  }
  assert(!dimension.isEmpty());
  TreeRef units;
  if (projectionContext.m_dimension.isAngleUnit()) {
    units = dimension.toBaseUnits();
  } else {
    double value = Approximation::RootTreeToReal<double>(expr);
    units = SharedTreeStack->push<Type::Mult>(2);
    ChooseBestDerivedUnits(&dimension);
    dimension.toBaseUnits();
    Simplification::DeepSystematicReduce(units);
    Units::Unit::ChooseBestRepresentativeAndPrefixForValue(
        units, &value, projectionContext.m_unitFormat);
    Tree* approximated =
        SharedTreeStack->push<Type::DoubleFloat>(static_cast<double>(value));
    expr->moveTreeOverTree(approximated);
  }
  Beautification::DeepBeautify(units);
  expr->moveTreeOverTree(
      PatternMatching::Create(KMult(KA, KB), {.KA = expr, .KB = units}));
  units->removeTree();
  return true;
}

/* Find and beautify trigonometric system nodes while converting the angles.
 * Simplifications are needed, this has to be done before beautification.
 * A bottom-up pattern is also needed because inverse trigonometric must
 * simplify its parents. */
bool Beautification::DeepBeautifyAngleFunctions(Tree* tree, AngleUnit angleUnit,
                                                bool* simplifyParent) {
  bool modified = false;
  bool mustSystematicReduce = false;
  for (Tree* child : tree->children()) {
    bool tempMustSystematicReduce = false;
    modified |=
        DeepBeautifyAngleFunctions(child, angleUnit, &tempMustSystematicReduce);
    mustSystematicReduce |= tempMustSystematicReduce;
  }
  // A parent simplification is required after inverse trigonometry beautify
  *simplifyParent = (angleUnit != AngleUnit::Radian &&
                     (tree->isATrig() || tree->isATanRad()));
  if (ShallowBeautifyAngleFunctions(tree, angleUnit)) {
    return true;
  } else if (mustSystematicReduce) {
    assert(modified);
    *simplifyParent = Simplification::ShallowSystematicReduce(tree);
  }
  return modified;
}

// At this stage of the simplification, advanced reductions are expected.
bool Beautification::ShallowBeautifyAngleFunctions(Tree* tree,
                                                   AngleUnit angleUnit) {
  // Beautify System nodes to prevent future simplifications.
  if (tree->isTrig() || tree->isTanRad()) {
    if (angleUnit != AngleUnit::Radian) {
      Tree* child = tree->child(0);
      child->moveTreeOverTree(PatternMatching::CreateSimplify(
          KMult(KA, KB), {.KA = child, .KB = Angle::RadTo(angleUnit)}));
      /* This adds new potential multiplication expansions. Another advanced
       * reduction in DeepBeautify may be needed.
       * TODO: Call AdvancedReduce in DeepBeautify only if we went here. */
    }
    PatternMatching::MatchReplace(tree, KTrig(KA, 0_e), KCos(KA)) ||
        PatternMatching::MatchReplace(tree, KTrig(KA, 1_e), KSin(KA)) ||
        PatternMatching::MatchReplace(tree, KTanRad(KA), KTan(KA));
    return true;
  }
  if (tree->isATrig() || tree->isATanRad()) {
    PatternMatching::MatchReplace(tree, KATrig(KA, 0_e), KACos(KA)) ||
        PatternMatching::MatchReplace(tree, KATrig(KA, 1_e), KASin(KA)) ||
        PatternMatching::MatchReplace(tree, KATanRad(KA), KATan(KA));
    if (angleUnit != AngleUnit::Radian) {
      tree->moveTreeOverTree(PatternMatching::CreateSimplify(
          KMult(KA, KB), {.KA = tree, .KB = Angle::ToRad(angleUnit)}));
    }
    return true;
  }
  return false;
}

bool Beautification::ShallowBeautifyPercent(Tree* e) {
  // A% -> A / 100
  if (PatternMatching::MatchReplace(e, KPercentSimple(KA), KDiv(KA, 100_e))) {
    return true;
  }
  // TODO_PCJ PercentAddition had a deepBeautify to preserve addition order
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(KPercentAddition(KA, KB), e, &ctx)) {
    return false;
  }
  // A + B% -> A * (1 + B / 100)
  return PatternMatching::MatchReplace(e, KPercentAddition(KA, KB),
                                       KMult(KA, KAdd(1_e, KDiv(KB, 100_e))));
}

bool Beautification::DeepBeautify(Tree* expr,
                                  ProjectionContext projectionContext) {
  bool dummy = false;
  bool changed =
      DeepBeautifyAngleFunctions(expr, projectionContext.m_angleUnit, &dummy);
  if (changed && projectionContext.m_angleUnit != AngleUnit::Radian) {
    // A ShallowBeautifyAngleFunctions may have added expands possibilities.
    AdvancedSimplification::AdvancedReduce(expr);
  }
  changed = Tree::ApplyShallowInDepth(expr, ShallowBeautify, nullptr, false) ||
            changed;
  changed = Variables::BeautifyToName(expr) || changed;
  return AddUnits(expr, projectionContext) || changed;
}

// Reverse most system projections to display better expressions
bool Beautification::ShallowBeautify(Tree* e, void* context) {
  bool changed = false;
  if (e->isAdd()) {
    NAry::Sort(e, Comparison::Order::AdditionBeautification);
  } else if (e->isFactor()) {
    if (Arithmetic::BeautifyFactor(e)) {
      return true;
    }
  }

#if 0
  // TODO: handle lnReal too
  // ln(A)      * ln(B)^(-1) -> log(A, B)
  // ln(A)^(-1) * ln(B)      -> log(B, A)
  changed = PatternMatching::MatchReplace(
                ref, KMult(KA_s, KLn(KB), KPow(KLn(KC), -1_e), KD_s),
                KMult(KA_s, KLogarithm(KB, KC), KD_s)) ||
            PatternMatching::MatchReplace(
                ref, KMult(KA_s, KPow(KLn(KB), -1_e), KLn(KC), KD_s),
                KMult(KA_s, KLogarithm(KC, KB), KD_s));
#endif

  if (PatternMatching::MatchReplace(e, KMult(-1_e, KA_p),
                                    KOpposite(KMult(KA_p)))) {
    return true;
  }

  // Turn multiplications with negative powers into divisions
  if (e->isMult() || e->isPow() || Number::IsStrictRational(e)) {
    if (BeautifyIntoDivision(e)) {
      return true;
    }
  }

  if (e->isOfType({Type::Mult, Type::GCD, Type::LCM}) &&
      NAry::Sort(e, Comparison::Order::Beautification)) {
    return true;
  }

  // PowerReal(A,B) -> A^B
  // PowerMatrix(A,B) -> A^B
  // exp(A? * ln(B) * C?) -> B^(A*C)
  if (PatternMatching::MatchReplace(e, KPowMatrix(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchReplace(e, KPowReal(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchReplace(e, KExp(KMult(KA_s, KLn(KB), KC_s)),
                                    KPow(KB, KMult(KA_s, KC_s)))) {
    // A^0.5 -> Sqrt(A)
    PatternMatching::MatchReplace(e, KPow(KA, 1_e / 2_e), KSqrt(KA));
    return true;
  }
  return
      // lnReal(x) -> ln(x)
      PatternMatching::MatchReplace(e, KLnReal(KA), KLn(KA)) ||
      // exp(1) -> e
      PatternMatching::MatchReplace(e, KExp(1_e), e_e) ||
      // exp(A) -> e^A
      PatternMatching::MatchReplace(e, KExp(KA), KPow(e_e, KA)) ||
      // -floor(-A) -> ceil(A)
      PatternMatching::MatchReplace(
          e, KMult(-1_e, KA_s, KFloor(KMult(-1_e, KB)), KC_s),
          KMult(KA_s, KCeil(KB), KC_s)) ||
      // A - floor(A) -> frac(A)
      PatternMatching::MatchReplace(
          e, KAdd(KA_s, KB, KC_s, KMult(-1_e, KFloor(KB)), KD_s),
          KAdd(KA_s, KC_s, KFrac(KB), KD_s)) ||
      ShallowBeautifyPercent(e) || changed;
}

template <typename T>
Tree* Beautification::PushBeautifiedComplex(std::complex<T> value,
                                            ComplexFormat complexFormat) {
  // TODO: factorize with the code above somehow ?
  constexpr Type Type = FloatType<T>::type;
  T re = value.real(), im = value.imag();
  if (std::isnan(re) || std::isnan(im)) {
    return SharedTreeStack->push(Type::Undef);
  }
  if (im != 0 && complexFormat == ComplexFormat::Real) {
    return SharedTreeStack->push(Type::NonReal);
  }
  if (im == 0 && (complexFormat != ComplexFormat::Polar || re >= 0)) {
    return SharedTreeStack->push<Type>(re);
  }
  Tree* result = Tree::FromBlocks(SharedTreeStack->lastBlock());
  // Real part and separator
  if (complexFormat == ComplexFormat::Cartesian) {
    // [re+]
    if (re != 0) {
      SharedTreeStack->push<Type::Add>(2);
      SharedTreeStack->push<Type>(re);
    }
  } else {
    // [abs×]e^
    T abs = std::abs(value);
    if (abs != 1) {
      SharedTreeStack->push<Type::Mult>(2);
      SharedTreeStack->push<Type>(abs);
    }
    SharedTreeStack->push(Type::Pow);
    SharedTreeStack->push(Type::EulerE);
    im = std::arg(value);
  }
  // Complex part ±[im×]i
  if (im < 0) {
    SharedTreeStack->push(Type::Opposite);
    im = -im;
  }
  if (im != 1) {
    SharedTreeStack->push<Type::Mult>(2);
    SharedTreeStack->push<Type>(im);
  }
  SharedTreeStack->push(Type::ComplexI);
  return result;
}

template Tree* Beautification::PushBeautifiedComplex(
    std::complex<float>, ComplexFormat complexFormat);
template Tree* Beautification::PushBeautifiedComplex(
    std::complex<double>, ComplexFormat complexFormat);

}  // namespace Poincare::Internal
