#include "approximation.h"

#include <math.h>
#include <poincare/float.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>
#include <complex>

#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "list.h"
#include "random.h"
#include "rational.h"
#include "variables.h"

namespace PoincareJ {

// TODO: tests

AngleUnit Approximation::angleUnit;
Approximation::VariableType Approximation::s_variables[k_maxNumberOfVariables];
int Approximation::s_listElement;

template <typename T>
std::complex<T> Approximation::RootTreeToComplex(const Tree* node,
                                                 AngleUnit angleUnit) {
  Random::Context context;
  Approximation::angleUnit = angleUnit;
  s_listElement = -1;
  // TODO we should rather assume variable projection has already been done
  Tree* variables = Variables::GetUserSymbols(node);
  Tree* clone = node->clone();
  Variables::ProjectToId(clone, variables, ComplexSign::Unknown());
  std::complex<T> result = ToComplex<T>(clone, &context);
  clone->removeTree();
  variables->removeTree();
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    s_variables[i] = NAN;
  }
  return result;
}

template <typename T>
std::complex<T> Approximation::ToComplex(const Tree* node,
                                         Random::Context* context) {
  assert(node->isExpression());
  if (node->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }
  if (node->isRandomNode()) {
    return Random::Approximate<T>(node, context);
  }
  switch (node->type()) {
    case BlockType::Constant:
      if (Constant::Type(node) == Constant::Type::I) {
        return std::complex<T>(0, 1);
      }
      return Constant::To<T>(Constant::Type(node));
    case BlockType::SingleFloat:
      return Float::FloatTo(node);
    case BlockType::DoubleFloat:
      return Float::DoubleTo(node);
    case BlockType::List:
      return ToComplex<T>(node->child(s_listElement), context);
    case BlockType::ListSequence: {
      ShiftVariables();
      // epsilon sequences starts at one
      setXValue(s_listElement + 1);
      std::complex<T> result = ToComplex<T>(node->child(2), context);
      UnshiftVariables();
      return result;
    }
    case BlockType::Addition:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatAddition<std::complex<T>>, context);
    case BlockType::Multiplication:
      return MapAndReduce<T, std::complex<T>>(node, FloatMultiplication<T>,
                                              context);
    case BlockType::Division:
      return MapAndReduce<T, std::complex<T>>(node, FloatDivision<T>, context);
    case BlockType::Subtraction:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatSubtraction<std::complex<T>>, context);
    case BlockType::PowerReal:
      return MapAndReduce<T, std::complex<T>>(node, FloatPowerReal<T>, context);
    case BlockType::Power:
      return MapAndReduce<T, std::complex<T>>(node, FloatPower<std::complex<T>>,
                                              context);
    case BlockType::Logarithm:
      return MapAndReduce<T, std::complex<T>>(node, FloatLog<std::complex<T>>,
                                              context);
    case BlockType::Trig:
      return MapAndReduce<T, std::complex<T>>(node, FloatTrig<std::complex<T>>,
                                              context);
    case BlockType::ATrig:
      return MapAndReduce<T, std::complex<T>>(node, FloatATrig<std::complex<T>>,
                                              context);
    case BlockType::GCD:
      return MapAndReduce<T, T>(node, FloatGCD<T>, context,
                                PositiveIntegerApproximation<T>);
    case BlockType::LCM:
      return MapAndReduce<T, T>(node, FloatLCM<T>, context,
                                PositiveIntegerApproximation<T>);
    case BlockType::SquareRoot:
      return std::sqrt(ToComplex<T>(node->nextNode(), context));
    case BlockType::NthRoot:
      return std::pow(
          ToComplex<T>(node->nextNode(), context),
          static_cast<T>(1) / ToComplex<T>(node->child(1), context));
    case BlockType::Exponential:
      return std::exp(ToComplex<T>(node->nextNode(), context));
    case BlockType::Log:
    case BlockType::Ln: {
      std::complex<T> c = ToComplex<T>(node->nextNode(), context);
      /* log has a branch cut on ]-inf, 0]: it is then multivalued on this cut.
       * We followed the convention chosen by the lib c++ of llvm on ]-inf+0i,
       * 0+0i] (warning: log takes the other side of the cut values on ]-inf-0i,
       * 0-0i]). We manually handle the case where the argument is null, as the
       * lib c++ gives log(0) = -inf, which is only a generous shorthand for the
       * limit. */
      return c == std::complex<T>(0) ? NAN
             : node->isLog()         ? std::log10(c)
                                     : std::log(c);
    }
    case BlockType::Abs:
      return std::abs(ToComplex<T>(node->nextNode(), context));
    case BlockType::Infinity:
      return INFINITY;
    case BlockType::Opposite:
      return -ToComplex<T>(node->nextNode(), context);
    case BlockType::RealPart:
      return ToComplex<T>(node->nextNode(), context).real();
    case BlockType::ImaginaryPart:
      return ToComplex<T>(node->nextNode(), context).imag();
    case BlockType::Cosine:
    case BlockType::Sine:
    case BlockType::Tangent:
    case BlockType::Secant:
    case BlockType::Cosecant:
    case BlockType::Cotangent:
    case BlockType::ArcCosine:
    case BlockType::ArcSine:
    case BlockType::ArcTangent:
    case BlockType::ArcSecant:
    case BlockType::ArcCosecant:
    case BlockType::ArcCotangent:
      return TrigonometricToComplex(node->type(),
                                    ToComplex<T>(node->nextNode(), context));
    case BlockType::HyperbolicSine:
    case BlockType::HyperbolicCosine:
    case BlockType::HyperbolicTangent:
    case BlockType::HyperbolicArcSine:
    case BlockType::HyperbolicArcCosine:
    case BlockType::HyperbolicArcTangent:
      return HyperbolicToComplex(node->type(),
                                 ToComplex<T>(node->nextNode(), context));
    case BlockType::Variable:
      return s_variables[Variables::Id(node)];
    case BlockType::Sum:
    case BlockType::Product: {
      const Tree* lowerBoundChild = node->child(Parametric::k_lowerBoundIndex);
      std::complex<T> low = ToComplex<T>(lowerBoundChild, context);
      if (low.imag() != 0 || (int)low.real() != low.real()) {
        return NAN;
      }
      const Tree* upperBoundChild = lowerBoundChild->nextTree();
      std::complex<T> up = ToComplex<T>(upperBoundChild, context);
      if (up.imag() != 0 || (int)up.real() != up.real()) {
        return NAN;
      }
      int lowerBound = low.real();
      int upperBound = up.real();
      const Tree* child = upperBoundChild->nextTree();
      ShiftVariables();
      std::complex<T> result = node->isSum() ? 0 : 1;
      for (int k = lowerBound; k <= upperBound; k++) {
        s_variables[0] = k;
        std::complex<T> value = ToComplex<T>(child, context);
        if (node->isSum()) {
          result += value;
        } else {
          result *= value;
        }
        if (std::isnan(result.real()) || std::isnan(result.imag())) {
          break;
        }
      }
      UnshiftVariables();
      return result;
    }
    case BlockType::Derivative: {
      constexpr static int k_maxOrderForApproximation = 4;
      int order = 1;  // TODO nth diff
      if (order < 0) {
        return NAN;
      }
      if (order > k_maxOrderForApproximation) {
        /* FIXME:
         * Since approximation of higher order derivative is exponentially
         * complex, we set a threshold above which we won't compute the
         * derivative.
         *
         * The method we use for now for the higher order derivatives is to
         * recursively approximate the derivatives of lower levels.
         * It's as if we approximated diff(diff(diff(diff(..(diff(f(x)))))))
         * But this is method is way too expensive in time and memory.
         *
         * Other methods exists for approximating higher order derivative.
         * This should be investigated
         * */
        return NAN;
      }
      std::complex<T> at = ToComplex<T>(node->child(1), context);
      if (std::isnan(at.real()) || at.imag() != 0) {
        return NAN;
      }
      ShiftVariables();
      T result = approximateDerivative(node->child(2), at.real(), order);
      UnshiftVariables();
      return result;
    }
    case BlockType::Integral:
      return approximateIntegral<T>(node);
    default:;
  }
  // The remaining operators are defined only on reals
  // assert(node->numberOfChildren() <= 2);
  if (node->numberOfChildren() > 2) {
    return NAN;
  }
  T child[2];
  for (int i = 0; const Tree* childNode : node->children()) {
    std::complex<T> app = ToComplex<T>(childNode, context);
    if (app.imag() != 0) {
      return NAN;
    }
    child[i++] = app.real();
  }
  switch (node->type()) {
    case BlockType::Decimal:
      return child[0] *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case BlockType::Sign: {
      // TODO why no epsilon in Poincare ?
      if (std::isnan(child[0])) {
        return NAN;
      }
      return child[0] == 0 ? 0 : child[0] < 0 ? -1 : 1;
    }
    case BlockType::LnReal:
      // TODO unreal
      return child[0] <= 0 ? NAN : std::log(child[0]);
    case BlockType::Floor:
    case BlockType::Ceiling: {
      /* Assume low deviation from natural numbers are errors */
      T delta = std::fabs((std::round(child[0]) - child[0]) / child[0]);
      if (delta <= Poincare::Float<T>::Epsilon()) {
        return std::round(child[0]);
      }
      return node->isFloor() ? std::floor(child[0]) : std::ceil(child[0]);
    }
    case BlockType::FracPart: {
      return child[0] - std::floor(child[0]);
    }
    case BlockType::Round: {
      if (std::isnan(child[1]) || child[1] != std::round(child[1])) {
        return NAN;
      }
      T err = std::pow(10, std::round(child[1]));
      return std::round(child[0] * err) / err;
    }
    case BlockType::Quotient:
    case BlockType::Remainder: {
      T a = child[0];
      T b = child[1];
      if (std::isnan(a) || std::isnan(b) || a != (int)a || b != (int)b) {
        return NAN;
      }
      // TODO : is this really better than std::remainder ?
      T quotient = b >= 0 ? std::floor(a / b) : -std::floor(a / (-b));
      return node->isQuotient() ? quotient : std::round(a - b * quotient);
    }

    case BlockType::Factorial: {
      T n = child[0];
      if (std::isnan(n) || n != std::round(n) || n < 0) {
        return NAN;
      }
      T result = 1;
      for (int i = 1; i <= (int)n; i++) {
        result *= static_cast<T>(i);
        if (std::isinf(result)) {
          return result;
        }
      }
      return std::round(result);
    }
    case BlockType::Binomial: {
      T n = child[0];
      T k = child[1];
      if (std::isnan(n) || std::isnan(k) || k != std::round(k)) {
        return NAN;
      }
      if (k < 0) {
        return 0;
      }
      // Generalized definition allows any n value
      bool generalized = (n != std::round(n) || n < k);
      // Take advantage of symmetry
      k = (!generalized && k > (n - k)) ? n - k : k;

      T result = 1;
      for (T i = 0; i < k; i++) {
        result *= (n - i) / (k - i);
        if (std::isinf(result) || std::isnan(result)) {
          return result;
        }
      }
      // If not generalized, the output must be rounded
      return generalized ? result : std::round(result);
    }
    case BlockType::Permute: {
      T n = child[0];
      T k = child[1];
      if (std::isnan(n) || std::isnan(k) || n != std::round(n) ||
          k != std::round(k) || n < 0.0f || k < 0.0f) {
        return NAN;
      }
      if (k > n) {
        return 0.0;
      }
      T result = 1;
      for (int i = (int)n - (int)k + 1; i <= (int)n; i++) {
        result *= i;
        if (std::isinf(result) || std::isnan(result)) {
          return result;
        }
      }
      return std::round(result);
    }

    default:
      if (node->isParametric()) {
        // TODO: Explicit tree if it contains random nodes.
      }
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
Tree* Approximation::RootTreeToList(const Tree* node, AngleUnit angleUnit) {
  Approximation::angleUnit = angleUnit;
  s_listElement = -1;
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    s_variables[i] = NAN;
  }
  // TODO we should rather assume variable projection has already been done
  Tree* variables = Variables::GetUserSymbols(node);
  Tree* clone = node->clone();
  Variables::ProjectToId(clone, variables, ComplexSign::Unknown());
  {
    // Be careful to nest Random::Context since they create trees
    Random::Context context;
    ToList<T>(clone, &context);
  }
  clone->removeTree();
  variables->removeTree();
  return variables;
}

template <typename T>
Tree* Approximation::ToList(const Tree* node, Random::Context* context) {
  int length = Dimension::GetListLength(node);
  int old = s_listElement;
  Tree* list = SharedEditionPool->push<BlockType::List>(length);
  for (int i = 0; i < length; i++) {
    s_listElement = i;
    std::complex<T> k = ToComplex<T>(node, context);
    SharedEditionPool->push(BlockType::Complex);
    SharedEditionPool->push<FloatType<T>::type>(k.real());
    SharedEditionPool->push<FloatType<T>::type>(k.imag());
  }
  s_listElement = old;
  return list;
}

template <typename T, typename U>
U Approximation::MapAndReduce(const Tree* node, Reductor<U> reductor,
                              Random::Context* context,
                              Mapper<std::complex<T>, U> mapper) {
  U res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    std::complex<T> app = ToComplex<T>(child, context);
    if (std::isnan(app.real()) || std::isnan(app.imag())) {
      return NAN;
    }
    U mapped;
    if constexpr (std::is_same_v<std::complex<T>, U>) {
      mapped = app;
    } else {
      assert(mapper);
      mapped = mapper(app);
      if (std::isnan(mapped)) {
        return NAN;
      }
    }
    if (index == 0) {
      res = mapped;
    } else {
      res = reductor(res, mapped);
    }
  }
  return res;
}

bool interruptApproximation(TypeBlock type, int childIndex,
                            TypeBlock childType) {
  switch (type) {
    case BlockType::ATrig:
    case BlockType::Trig:
      // Do not approximate second term in case tree isn't replaced.
      return (childIndex == 1);
    case BlockType::PowerMatrix:
    case BlockType::Power:
      // Note: After projection, Power's second term should always be integer.
      return (childIndex == 1 && childType.isInteger());
    case BlockType::Identity:
      return true;
    default:
      return false;
  }
}

template <typename T>
bool Approximation::ApproximateAndReplaceEveryScalarT(Tree* tree,
                                                      bool collapse) {
  // These types are either already approximated or impossible to approximate.
  if (tree->type() == FloatType<T>::type || tree->isRandomNode() ||
      tree->isOfType(
          {BlockType::UserSymbol, BlockType::Variable, BlockType::Unit})) {
    return false;
  }
  bool changed = false;
  bool approximateNode = collapse || (tree->numberOfChildren() == 0);
  int childIndex = 0;
  for (Tree* child : tree->children()) {
    if (interruptApproximation(tree->type(), childIndex++, child->type())) {
      break;
    }
    changed = ApproximateAndReplaceEveryScalarT<T>(child, collapse) || changed;
    approximateNode = approximateNode && child->type() == FloatType<T>::type;
  }
  if (!approximateNode) {
    // TODO: Partially approximate additions and multiplication anyway
    return changed;
  }
  tree->moveTreeOverTree(
      SharedEditionPool->push<FloatType<T>::type>(To<T>(tree, nullptr)));
  return true;
}

template std::complex<float> Approximation::RootTreeToComplex<float>(
    const Tree*, AngleUnit);
template std::complex<double> Approximation::RootTreeToComplex<double>(
    const Tree*, AngleUnit);

template std::complex<float> Approximation::ToComplex<float>(const Tree*,
                                                             Random::Context*);
template std::complex<double> Approximation::ToComplex<double>(
    const Tree*, Random::Context*);

template Tree* Approximation::RootTreeToList<float>(const Tree*, AngleUnit);
template Tree* Approximation::RootTreeToList<double>(const Tree*, AngleUnit);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(Tree*,
                                                                      bool);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(Tree*,
                                                                       bool);

}  // namespace PoincareJ
