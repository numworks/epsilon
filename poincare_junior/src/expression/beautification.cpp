#include "beautification.h"

#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/memory/tree.h>
#include <poincare_junior/src/n_ary.h>

#include "advanced_simplification.h"
#include "angle.h"
#include "approximation.h"
#include "arithmetic.h"
#include "context.h"
#include "float.h"
#include "number.h"
#include "rational.h"
#include "simplification.h"

namespace PoincareJ {

float Beautification::DegreeForSortingAddition(const Tree* expr,
                                               bool symbolsOnly) {
  switch (expr->type()) {
    case Type::Multiplication: {
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
    case Type::Power: {
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
    case Type::Variable:
      return 1.;
    default:
      return symbolsOnly ? 0. : 1.;
  }
}

Tree* Factor(Tree* expr, int index) {
  if (expr->isMultiplication()) {
    return expr->child(index);
  }
  return expr;
}

const Tree* Factor(const Tree* expr, int index) {
  if (expr->isMultiplication()) {
    return expr->child(index);
  }
  return expr;
}

int NumberOfFactors(const Tree* expr) {
  if (expr->isMultiplication()) {
    return expr->numberOfChildren();
  }
  return 1;
}

bool MakePositiveAnyNegativeNumeralFactor(Tree* expr) {
  // The expression is a negative number
  Tree* factor = Factor(expr, 0);
  if (factor->isMinusOne() && expr->isMultiplication()) {
    NAry::RemoveChildAtIndex(expr, 0);
    NAry::SquashIfUnary(expr);
    return true;
  }
  return factor->isNumber() && Number::SetSign(factor, NonStrictSign::Positive);
}

bool Beautification::SplitMultiplication(const Tree* expr, TreeRef& numerator,
                                         TreeRef& denominator) {
  bool result = false;
  numerator = SharedEditionPool->push<Type::Multiplication>(0);
  denominator = SharedEditionPool->push<Type::Multiplication>(0);
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
          factorsNumerator = rNum.pushOnEditionPool();
        }
        IntegerHandler rDen = Rational::Denominator(factor);
        if (!rDen.isOne()) {
          factorsDenominator = rDen.pushOnEditionPool();
        }
      }
    } else if (factor->isPower() || factor->isPowerReal()) {
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
    expr = expr->nextNode();
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

bool Beautification::AddUnits(Tree* expr, ProjectionContext projectionContext) {
  Units::DimensionVector dimension = projectionContext.m_dimension.unit.vector;
  if (!projectionContext.m_dimension.isUnit()) {
    return false;
  }
  assert(!dimension.isEmpty());
  TreeRef units;
  if (projectionContext.m_dimension.hasNonKelvinTemperatureUnit()) {
    assert(dimension.supportSize() == 1);
    units = Units::Unit::Push(projectionContext.m_dimension.unit.representative,
                              Units::Prefix::EmptyPrefix());
  } else if (projectionContext.m_dimension.isAngleUnit()) {
    units = dimension.toBaseUnits();
  } else {
    double value = Approximation::RootTreeTo<double>(expr);
    units = SharedEditionPool->push<Type::Multiplication>(2);
    ChooseBestDerivedUnits(&dimension);
    dimension.toBaseUnits();
    Simplification::DeepSystematicReduce(units);
    Units::Unit::ChooseBestRepresentativeAndPrefixForValue(
        units, &value, projectionContext.m_unitFormat);
    Tree* approximated =
        SharedEditionPool->push<Type::DoubleFloat>(static_cast<double>(value));
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
                     (tree->isATrig() || tree->isArcTangentRad()));
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
  if (tree->isTrig() || tree->isTangentRad()) {
    if (angleUnit != AngleUnit::Radian) {
      Tree* child = tree->child(0);
      child->moveTreeOverTree(PatternMatching::CreateAndSimplify(
          KMult(KA, KB), {.KA = child, .KB = Angle::RadTo(angleUnit)}));
      /* This adds new potential multiplication expansions. Another advanced
       * reduction in DeepBeautify may be needed.
       * TODO: Call AdvancedReduce in DeepBeautify only if we went here. */
    }
    PatternMatching::MatchAndReplace(tree, KTrig(KA, 0_e), KCos(KA)) ||
        PatternMatching::MatchAndReplace(tree, KTrig(KA, 1_e), KSin(KA)) ||
        PatternMatching::MatchAndReplace(tree, KTanRad(KA), KTan(KA));
    return true;
  }
  if (tree->isATrig() || tree->isArcTangentRad()) {
    PatternMatching::MatchAndReplace(tree, KATrig(KA, 0_e), KACos(KA)) ||
        PatternMatching::MatchAndReplace(tree, KATrig(KA, 1_e), KASin(KA)) ||
        PatternMatching::MatchAndReplace(tree, KATanRad(KA), KATan(KA));
    if (angleUnit != AngleUnit::Radian) {
      tree->moveTreeOverTree(PatternMatching::CreateAndSimplify(
          KMult(KA, KB), {.KA = tree, .KB = Angle::ToRad(angleUnit)}));
    }
    return true;
  }
  return false;
}

bool Beautification::ShallowBeautifyPercent(Tree* ref) {
  // A% -> A / 100
  if (PatternMatching::MatchAndReplace(ref, KPercentSimple(KA),
                                       KDiv(KA, 100_e))) {
    return true;
  }
  // TODO PCJ PercentAddition had a deepBeautify to preserve addition order
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(KPercentAddition(KA, KB), ref, &ctx)) {
    return false;
  }
  // A + B% -> A * (1 + B / 100)
  return PatternMatching::MatchAndReplace(
      ref, KPercentAddition(KA, KB), KMult(KA, KAdd(1_e, KDiv(KB, 100_e))));
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
  return AddUnits(expr, projectionContext) || changed;
}

// Reverse most system projections to display better expressions
bool Beautification::ShallowBeautify(Tree* ref, void* context) {
  bool changed = false;
  if (ref->isAddition()) {
    NAry::Sort(ref, Comparison::Order::AdditionBeautification);
  } else if (ref->isFactor()) {
    if (Arithmetic::BeautifyFactor(ref)) {
      return true;
    }
  }

#if 0
  // TODO: handle lnReal too
  // ln(A)      * ln(B)^(-1) -> log(A, B)
  // ln(A)^(-1) * ln(B)      -> log(B, A)
  changed = PatternMatching::MatchAndReplace(
                ref, KMult(KA_s, KLn(KB), KPow(KLn(KC), -1_e), KD_s),
                KMult(KA_s, KLogarithm(KB, KC), KD_s)) ||
            PatternMatching::MatchAndReplace(
                ref, KMult(KA_s, KPow(KLn(KB), -1_e), KLn(KC), KD_s),
                KMult(KA_s, KLogarithm(KC, KB), KD_s));
#endif

  if (PatternMatching::MatchAndReplace(ref, KMult(-1_e, KA_p),
                                       KOpposite(KMult(KA_p)))) {
    return true;
  }

  // Turn multiplications with negative powers into divisions
  if (ref->isMultiplication() || ref->isPower() ||
      Number::IsStrictRational(ref)) {
    if (BeautifyIntoDivision(ref)) {
      return true;
    }
  }

  if (ref->isOfType({Type::Multiplication, Type::GCD, Type::LCM}) &&
      NAry::Sort(ref, Comparison::Order::Beautification)) {
    return true;
  }

  // PowerReal(A,B) -> A^B
  // PowerMatrix(A,B) -> A^B
  // exp(A? * ln(B) * C?) -> B^(A*C)
  if (PatternMatching::MatchAndReplace(ref, KPowMatrix(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchAndReplace(ref, KPowReal(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchAndReplace(ref, KExp(KMult(KA_s, KLn(KB), KC_s)),
                                       KPow(KB, KMult(KA_s, KC_s)))) {
    // A^0.5 -> Sqrt(A)
    PatternMatching::MatchAndReplace(ref, KPow(KA, KHalf), KSqrt(KA));
    return true;
  }
  return
      // lnReal(x) -> ln(x)
      PatternMatching::MatchAndReplace(ref, KLnReal(KA), KLn(KA)) ||
      // exp(1) -> e
      PatternMatching::MatchAndReplace(ref, KExp(1_e), e_e) ||
      // exp(A) -> e^A
      PatternMatching::MatchAndReplace(ref, KExp(KA), KPow(e_e, KA)) ||
      // -floor(-A) -> ceil(A)
      PatternMatching::MatchAndReplace(
          ref, KMult(-1_e, KA_s, KFloor(KMult(-1_e, KB)), KC_s),
          KMult(KA_s, KCeil(KB), KC_s)) ||
      // A - floor(A) -> frac(A)
      PatternMatching::MatchAndReplace(
          ref, KAdd(KA_s, KB, KC_s, KMult(-1_e, KFloor(KB)), KD_s),
          KAdd(KA_s, KC_s, KFrac(KB), KD_s)) ||
      ShallowBeautifyPercent(ref) || changed;
}

template <typename T>
Tree* Beautification::PushBeautifiedComplex(std::complex<T> value,
                                            ComplexFormat complexFormat) {
  // TODO : factorize with the code above somehow ?
  constexpr Type Type = FloatType<T>::type;
  T re = value.real(), im = value.imag();
  if (std::isnan(re) || std::isnan(im)) {
    return SharedEditionPool->push(Type::Undefined);
  }
  if (im != 0 && complexFormat == ComplexFormat::Real) {
    return SharedEditionPool->push(Type::Nonreal);
  }
  if (im == 0 && (complexFormat != ComplexFormat::Polar || re >= 0)) {
    return SharedEditionPool->push<Type>(re);
  }
  Tree* result = Tree::FromBlocks(SharedEditionPool->lastBlock());
  // Real part and separator
  if (complexFormat == ComplexFormat::Cartesian) {
    // [re+]
    if (re != 0) {
      SharedEditionPool->push<Type::Addition>(2);
      SharedEditionPool->push<Type>(re);
    }
  } else {
    // [abs×]e^
    T abs = std::abs(value);
    if (abs != 1) {
      SharedEditionPool->push<Type::Multiplication>(2);
      SharedEditionPool->push<Type>(abs);
    }
    SharedEditionPool->push(Type::Power);
    SharedEditionPool->push(Type::ExponentialE);
    im = std::arg(value);
  }
  // Complex part ±[im×]i
  if (im < 0) {
    SharedEditionPool->push(Type::Opposite);
    im = -im;
  }
  if (im != 1) {
    SharedEditionPool->push<Type::Multiplication>(2);
    SharedEditionPool->push<Type>(im);
  }
  SharedEditionPool->push(Type::ComplexI);
  return result;
}

template Tree* Beautification::PushBeautifiedComplex(
    std::complex<float>, ComplexFormat complexFormat);
template Tree* Beautification::PushBeautifiedComplex(
    std::complex<double>, ComplexFormat complexFormat);

}  // namespace PoincareJ
