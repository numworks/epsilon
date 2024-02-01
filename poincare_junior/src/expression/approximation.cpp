#include "approximation.h"

#include <math.h>
#include <poincare/float.h>
#include <poincare_junior/src/memory/node_iterator.h>

#include <bit>
#include <complex>

#include "../n_ary.h"
#include "constant.h"
#include "decimal.h"
#include "float.h"
#include "list.h"
#include "matrix.h"
#include "random.h"
#include "rational.h"
#include "variables.h"
#include "vector.h"

namespace PoincareJ {

AngleUnit Approximation::s_angleUnit;
ComplexFormat Approximation::s_complexFormat;
Approximation::VariableType Approximation::s_variables[k_maxNumberOfVariables];
uint8_t Approximation::s_variablesOffset = 0;
int Approximation::s_listElement;

// With a nullptr context, seeded random will be undef.
Random::Context* Approximation::s_context = nullptr;

void Approximation::ClearVariables() {
  for (int i = 0; i < k_maxNumberOfVariables; i++) {
    s_variables[i] = NAN;
  }
  s_variablesOffset = 0;
}
// with sum(sum(l,l,1,k),k,1,n) s_variables stores [n, NaN, …, NaN, l, k]
double& Approximation::Variable(size_t index) {
  return s_variables[(index + s_variablesOffset) % k_maxNumberOfVariables];
}
void Approximation::ShiftVariables() { s_variablesOffset--; }
void Approximation::UnshiftVariables() { s_variablesOffset++; }

template <typename T>
std::complex<T> Approximation::FloatMultiplication(std::complex<T> c,
                                                   std::complex<T> d) {
  // Special case to prevent (inf,0)*(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    constexpr T zero = static_cast<T>(0.0);
    // Handle case of pure imaginary/real multiplications
    if (c.imag() == zero && d.imag() == zero) {
      return {c.real() * d.real(), zero};
    }
    if (c.real() == zero && d.real() == zero) {
      return {-c.imag() * d.imag(), zero};
    }
    if (c.imag() == zero && d.real() == zero) {
      return {zero, c.real() * d.imag()};
    }
    if (c.real() == zero && d.imag() == zero) {
      return {zero, c.imag() * d.real()};
    }
    // Other cases are left to the standard library, and might return NaN.
  }
  return c * d;
}

template <typename T>
std::complex<T> Approximation::FloatDivision(std::complex<T> c,
                                             std::complex<T> d) {
  if (d.real() == 0 && d.imag() == 0) {
    return NAN;
  }
  // Special case to prevent (inf,0)/(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    // Handle case of pure imaginary/real divisions
    if (c.imag() == 0 && d.imag() == 0) {
      return {c.real() / d.real(), 0};
    }
    if (c.real() == 0 && d.real() == 0) {
      return {c.imag() / d.imag(), 0};
    }
    if (c.imag() == 0 && d.real() == 0) {
      return {0, -c.real() / d.imag()};
    }
    if (c.real() == 0 && d.imag() == 0) {
      return {0, c.imag() / d.real()};
    }
    // Other cases are left to the standard library, and might return NaN.
  }
  return c / d;
}

template <typename T>
Tree* Approximation::RootTreeToTree(const Tree* node, AngleUnit angleUnit,
                                    ComplexFormat complexFormat) {
  if (!Dimension::DeepCheckDimensions(node) ||
      !Dimension::DeepCheckListLength(node)) {
    return KUndef->clone();
  }
  if (Dimension::GetDimension(node).isMatrix()) {
    return Approximation::RootTreeToMatrix<T>(node, angleUnit, complexFormat);
  }
  if (Dimension::GetListLength(node) != -1) {
    return Approximation::RootTreeToList<T>(node, angleUnit, complexFormat);
  }
  std::complex<T> value =
      Approximation::RootTreeToComplex<T>(node, angleUnit, complexFormat);
  return Beautification::PushBeautifiedComplex(value, complexFormat);
}

template <typename T>
std::complex<T> Approximation::RootTreeToComplex(const Tree* node,
                                                 AngleUnit angleUnit,
                                                 ComplexFormat complexFormat) {
  Random::Context context;
  s_context = &context;
  s_angleUnit = angleUnit;
  s_complexFormat = complexFormat;
  s_listElement = -1;
  // TODO we should rather assume variable projection has already been done
  Tree* variables = Variables::GetUserSymbols(node);
  Tree* clone = node->clone();
  Variables::ProjectToId(clone, variables, ComplexSign::Unknown());
  std::complex<T> result = ToComplex<T>(clone);
  clone->removeTree();
  variables->removeTree();
  ClearVariables();
  s_context = nullptr;
  return result;
}

template <typename T>
std::complex<T> Approximation::ToComplex(const Tree* node) {
  /* TODO : the second part of this function and several ifs in different cases
   * act differently / more precisely on reals. We should have a dedicated,
   * faster, simpler and more precise real approximation to be used in every
   * cases where we know for sure there are no complexes. */
  assert(node->isExpression());
  if (node->isRational()) {
    return Rational::Numerator(node).to<T>() /
           Rational::Denominator(node).to<T>();
  }
  if (node->isRandomNode()) {
    return Random::Approximate<T>(node, s_context);
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
    case BlockType::Complex:
      return std::complex<T>(To<T>(node->child(0)), To<T>(node->child(1)));
    case BlockType::Addition:
      return MapAndReduce<T, std::complex<T>>(node,
                                              FloatAddition<std::complex<T>>);
    case BlockType::Multiplication:
      return MapAndReduce<T, std::complex<T>>(node, FloatMultiplication<T>);
    case BlockType::Division:
      return MapAndReduce<T, std::complex<T>>(node, FloatDivision<T>);
    case BlockType::Subtraction:
      return MapAndReduce<T, std::complex<T>>(
          node, FloatSubtraction<std::complex<T>>);
    case BlockType::Power:
      return approximatePower<T>(node, s_complexFormat);
    case BlockType::Logarithm:
      return MapAndReduce<T, std::complex<T>>(node, FloatLog<std::complex<T>>);
    case BlockType::Trig:
      return MapAndReduce<T, std::complex<T>>(node, FloatTrig<std::complex<T>>);
    case BlockType::ATrig:
      return MapAndReduce<T, std::complex<T>>(node,
                                              FloatATrig<std::complex<T>>);
    case BlockType::GCD:
      return MapAndReduce<T, T>(node, FloatGCD<T>,
                                PositiveIntegerApproximation<T>);
    case BlockType::LCM:
      return MapAndReduce<T, T>(node, FloatLCM<T>,
                                PositiveIntegerApproximation<T>);
    case BlockType::SquareRoot:
      return std::sqrt(ToComplex<T>(node->nextNode()));
    case BlockType::NthRoot:
      return std::pow(ToComplex<T>(node->nextNode()),
                      static_cast<T>(1) / ToComplex<T>(node->child(1)));
    case BlockType::Exponential:
      return std::exp(ToComplex<T>(node->nextNode()));
    case BlockType::Log:
    case BlockType::Ln: {
      std::complex<T> c = ToComplex<T>(node->nextNode());
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
      return std::abs(ToComplex<T>(node->nextNode()));
    case BlockType::Infinity:
      return INFINITY;
    case BlockType::Conjugate:
      return std::conj(ToComplex<T>(node->nextNode()));
    case BlockType::Opposite:
      return -ToComplex<T>(node->nextNode());
    case BlockType::RealPart:
      return ToComplex<T>(node->nextNode()).real();
    case BlockType::ImaginaryPart:
      return ToComplex<T>(node->nextNode()).imag();

    /* Trigonometry */
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
                                    ToComplex<T>(node->nextNode()));
    case BlockType::HyperbolicSine:
    case BlockType::HyperbolicCosine:
    case BlockType::HyperbolicTangent:
    case BlockType::HyperbolicArcSine:
    case BlockType::HyperbolicArcCosine:
    case BlockType::HyperbolicArcTangent:
      return HyperbolicToComplex(node->type(), ToComplex<T>(node->nextNode()));
    case BlockType::Variable:
      return Variable(Variables::Id(node));

    /* Analysis */
    case BlockType::Sum:
    case BlockType::Product: {
      const Tree* lowerBoundChild = node->child(Parametric::k_lowerBoundIndex);
      std::complex<T> low = ToComplex<T>(lowerBoundChild);
      if (low.imag() != 0 || (int)low.real() != low.real()) {
        return NAN;
      }
      const Tree* upperBoundChild = lowerBoundChild->nextTree();
      std::complex<T> up = ToComplex<T>(upperBoundChild);
      if (up.imag() != 0 || (int)up.real() != up.real()) {
        return NAN;
      }
      int lowerBound = low.real();
      int upperBound = up.real();
      const Tree* child = upperBoundChild->nextTree();
      ShiftVariables();
      std::complex<T> result = node->isSum() ? 0 : 1;
      for (int k = lowerBound; k <= upperBound; k++) {
        Variable(0) = k;
        std::complex<T> value = ToComplex<T>(child);
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
      int order = 1;  // TODO PCJ nth diff
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
      std::complex<T> at = ToComplex<T>(node->child(1));
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

    /* Matrices */
    case BlockType::Norm:
    case BlockType::Trace:
    case BlockType::Det: {
      Tree* m = ToMatrix<T>(node->child(0));
      Tree* value;
      if (node->isDet()) {
        Matrix::RowCanonize(m, true, &value, true);
      } else if (node->isNorm()) {
        value = Vector::Norm(m);
      } else {
        value = Matrix::Trace(m);
      }
      std::complex<T> v = ToComplex<T>(value);
      value->removeTree();
      m->removeTree();
      return v;
    }
    case BlockType::Dot: {
      // TODO use complex conjugate ?
      Tree* u = ToMatrix<T>(node->child(0));
      Tree* v = ToMatrix<T>(node->child(1));
      Tree* r = Vector::Dot(u, v);
      std::complex<T> result = ToComplex<T>(r);
      r->removeTree();
      v->removeTree();
      u->removeTree();
      return result;
    }

    /* Lists */
    case BlockType::List:
      return ToComplex<T>(node->child(s_listElement));
    case BlockType::ListSequence: {
      ShiftVariables();
      // epsilon sequences starts at one
      setXValue(s_listElement + 1);
      std::complex<T> result = ToComplex<T>(node->child(2));
      UnshiftVariables();
      return result;
    }
    case BlockType::Dim: {
      int n = Dimension::GetListLength(node->child(0));
      return n >= 0 ? n : NAN;
    }
    case BlockType::ListSum:
    case BlockType::ListProduct: {
      const Tree* values = node->child(0);
      int length = Dimension::GetListLength(values);
      int old = s_listElement;
      std::complex<T> result = node->isListSum() ? 0 : 1;
      for (int i = 0; i < length; i++) {
        s_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        result = node->isListSum() ? result + v : result * v;
      }
      s_listElement = old;
      return result;
    }
    case BlockType::Minimum:
    case BlockType::Maximum: {
      const Tree* values = node->child(0);
      int length = Dimension::GetListLength(values);
      int old = s_listElement;
      T result;
      for (int i = 0; i < length; i++) {
        s_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        if (v.imag() != 0 || std::isnan(v.real())) {
          return NAN;
        }
        if (i == 0 ||
            (node->isMinimum() ? (v.real() < result) : (v.real() > result))) {
          result = v.real();
        }
      }
      s_listElement = old;
      return result;
    }
    case BlockType::Mean:
    case BlockType::StdDev:
    case BlockType::SampleStdDev:
    case BlockType::Variance: {
      const Tree* values = node->child(0);
      const Tree* coefficients = node->child(1);
      int length = Dimension::GetListLength(values);
      int old = s_listElement;
      std::complex<T> sum = 0;
      std::complex<T> sumOfSquares = 0;
      T coefficientsSum = 0;
      for (int i = 0; i < length; i++) {
        s_listElement = i;
        std::complex<T> v = ToComplex<T>(values);
        std::complex<T> c = ToComplex<T>(coefficients);
        if (c.imag() != 0 || c.real() < 0) {
          return NAN;
        }
        sum += c.real() * v;
        sumOfSquares += c.real() * v * v;
        coefficientsSum += c.real();
      }
      s_listElement = old;
      if (coefficientsSum == 0) {
        return NAN;
      }
      sum /= coefficientsSum;
      if (node->isMean()) {
        return sum;
      }
      sumOfSquares /= coefficientsSum;
      std::complex<T> var = sumOfSquares - sum * sum;
      if (node->isVariance()) {
        return var;
      }
      std::complex<T> stdDev = std::pow(var, std::complex<T>(0.5));
      if (node->isStdDev()) {
        return stdDev;
      }
      T sampleStdDevCoef = std::pow(1 + 1 / (coefficientsSum - 1), 0.5);
      return stdDev * sampleStdDevCoef;
    }
    case BlockType::ListSort: {
      /* TODO we are computing all elements and sorting the list for all
       * elements, this is awful */
      Tree* list = ToList<T>(node->child(0));
      NAry::Sort(list);
      std::complex<T> result = ToComplex<T>(list);
      list->removeTree();
      return result;
    }
    default:;
  }
  // The remaining operators are defined only on reals
  // assert(node->numberOfChildren() <= 2);
  if (node->numberOfChildren() > 2) {
    return NAN;
  }
  T child[2];
  for (int i = 0; const Tree* childNode : node->children()) {
    std::complex<T> app = ToComplex<T>(childNode);
    if (app.imag() != 0) {
      return NAN;
    }
    if (std::isnan(app.real())) {
      return NAN;
    }
    child[i++] = app.real();
  }
  switch (node->type()) {
    case BlockType::Decimal:
      return child[0] *
             std::pow(10.0, -static_cast<T>(Decimal::DecimalOffset(node)));
    case BlockType::PowerReal: {
      T a = child[0];
      T b = child[1];
      /* PowerReal could not be reduced, b's reductions cannot be safely
       * interpreted as a rational. As a consequence, return NAN if a is
       * negative and b isn't an integer. */
      return (a < 0.0 && b != std::round(b)) ? NAN : std::pow(a, b);
    }
    case BlockType::Sign: {
      // TODO why no epsilon in Poincare ?
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
      if (child[1] != std::round(child[1])) {
        return NAN;
      }
      T err = std::pow(10, std::round(child[1]));
      return std::round(child[0] * err) / err;
    }
    case BlockType::Quotient:
    case BlockType::Remainder: {
      T a = child[0];
      T b = child[1];
      if (a != (int)a || b != (int)b) {
        return NAN;
      }
      // TODO : is this really better than std::remainder ?
      T quotient = b >= 0 ? std::floor(a / b) : -std::floor(a / (-b));
      return node->isQuotient() ? quotient : std::round(a - b * quotient);
    }

    case BlockType::Factorial: {
      T n = child[0];
      if (n != std::round(n) || n < 0) {
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
      if (k != std::round(k)) {
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
      if (n != std::round(n) || k != std::round(k) || n < 0.0f || k < 0.0f) {
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
    case BlockType::Factor:
      // Useful for the beautification only
      return child[0];

    default:
      if (node->isParametric()) {
        // TODO: Explicit tree if it contains random nodes.
      }
      // TODO: Implement more BlockTypes
      return NAN;
  };
}

template <typename T>
Tree* Approximation::RootTreeToList(const Tree* node, AngleUnit angleUnit,
                                    ComplexFormat complexFormat) {
  s_angleUnit = angleUnit;
  s_complexFormat = complexFormat;
  s_listElement = -1;
  ClearVariables();
  // TODO we should rather assume variable projection has already been done
  Tree* variables = Variables::GetUserSymbols(node);
  Tree* clone = node->clone();
  Variables::ProjectToId(clone, variables, ComplexSign::Unknown());
  {
    // Be careful to nest Random::Context since they create trees
    Random::Context context;
    s_context = &context;
    ToList<T>(clone);
    s_context = nullptr;
  }
  clone->removeTree();
  variables->removeTree();
  return variables;
}

template <typename T>
Tree* Approximation::RootTreeToMatrix(const Tree* node, AngleUnit angleUnit,
                                      ComplexFormat complexFormat) {
  s_angleUnit = angleUnit;
  s_complexFormat = complexFormat;
  s_listElement = -1;
  ClearVariables();
  // TODO we should rather assume variable projection has already been done
  Tree* variables = Variables::GetUserSymbols(node);
  Tree* clone = node->clone();
  Variables::ProjectToId(clone, variables);
  {
    // Be careful to nest Random::Context since they create trees
    Random::Context context;
    s_context = &context;
    Tree* m = ToMatrix<T>(clone);
    for (Tree* child : m->children()) {
      child->moveTreeOverTree(Beautification::PushBeautifiedComplex(
          ToComplex<T>(child), complexFormat));
    }
    s_context = nullptr;
  }
  clone->removeTree();
  variables->removeTree();
  return variables;
}

template <typename T>
Tree* PushComplex(std::complex<T> value) {
  if (value.imag() == 0.0) {
    return SharedEditionPool->push<FloatType<T>::type>(value.real());
  }
  Tree* result = SharedEditionPool->push(BlockType::Complex);
  SharedEditionPool->push<FloatType<T>::type>(value.real());
  SharedEditionPool->push<FloatType<T>::type>(value.imag());
  return result;
}

bool Approximation::SimplifyComplex(Tree* node) {
  node->moveTreeOverTree(PushComplex(ToComplex<double>(node)));
  return true;
}

template <typename T>
Tree* Approximation::ToList(const Tree* node) {
  int length = Dimension::GetListLength(node);
  int old = s_listElement;
  Tree* list = SharedEditionPool->push<BlockType::List>(length);
  for (int i = 0; i < length; i++) {
    s_listElement = i;
    std::complex<T> k = ToComplex<T>(node);
    Beautification::PushBeautifiedComplex(k, s_complexFormat);
  }
  s_listElement = old;
  return list;
}

/* Using our consteval operator- inside a template<float> does not work with
 * llvm14 it works with 17. */
constexpr KTree minusOne = -1_e;

template <typename T>
Tree* Approximation::ToMatrix(const Tree* node) {
  /* TODO : Normal matrix nodes and operations with approximated children are
   * used to carry matrix approximation. A dedicated node that knows its
   * children have a fixed size would be more efficient. */
  if (node->isMatrix()) {
    Tree* m = node->cloneNode();
    for (const Tree* child : node->children()) {
      std::complex<T> v = ToComplex<T>(child);
      PushComplex(v);
    }
    return m;
  }
  switch (node->type()) {
    case BlockType::Addition: {
      const Tree* child = node->child(0);
      int n = node->numberOfChildren() - 1;
      Tree* result = ToMatrix<T>(child);
      while (n--) {
        child = child->nextTree();
        Tree* approximatedChild = ToMatrix<T>(child);
        Matrix::Addition(result, approximatedChild, true);
        approximatedChild->removeTree();
        result->removeTree();
      }
      return result;
    }
    case BlockType::Subtraction: {
      Tree* a = ToMatrix<T>(node->child(0));
      Tree* b = ToMatrix<T>(node->child(1));
      b->moveTreeOverTree(Matrix::ScalarMultiplication(minusOne, b, true));
      Matrix::Addition(a, b);
      a->removeTree();
      a->removeTree();
      return a;
    }
    case BlockType::Multiplication: {
      bool resultIsMatrix = false;
      Tree* result = nullptr;
      for (const Tree* child : node->children()) {
        bool childIsMatrix = Dimension::GetDimension(child).isMatrix();
        Tree* approx = childIsMatrix ? ToMatrix<T>(child)
                                     : PushComplex(ToComplex<T>(child));
        if (result == nullptr) {
          resultIsMatrix = childIsMatrix;
          result = approx;
          continue;
        }
        if (resultIsMatrix && childIsMatrix) {
          Matrix::Multiplication(result, approx, true);
        } else if (resultIsMatrix) {
          Matrix::ScalarMultiplication(approx, result, true);
        } else {
          Matrix::ScalarMultiplication(result, approx, true);
        }
        approx->removeTree();
        result->removeTree();
      }
      return result;
    }
    case BlockType::Division: {
      Tree* a = ToMatrix<T>(node->child(0));
      Tree* s = PushComplex(static_cast<T>(1) / ToComplex<T>(node->child(1)));
      s->moveTreeOverTree(Matrix::ScalarMultiplication(s, a, true));
      a->removeTree();
      return a;
    }
    case BlockType::PowerMatrix: {
      const Tree* base = node->child(0);
      const Tree* index = base->nextTree();
      T value = To<T>(index);
      if (std::isnan(value) || value != std::round(value)) {
        return KUndef->clone();
      }
      Tree* result = ToMatrix<T>(base);
      result->moveTreeOverTree(Matrix::Power(result, value, true));
      return result;
    }
    case BlockType::Inverse:
    case BlockType::Transpose: {
      Tree* result = ToMatrix<T>(node->child(0));
      result->moveTreeOverTree(node->isInverse() ? Matrix::Inverse(result, true)
                                                 : Matrix::Transpose(result));
      return result;
    }
    case BlockType::Ref:
    case BlockType::Rref: {
      Tree* result = ToMatrix<T>(node->child(0));
      Matrix::RowCanonize(result, node->isRref(), nullptr, true);
      return result;
    }
    case BlockType::Dim: {
      Dimension dim = Dimension::GetDimension(node->child(0));
      assert(dim.isMatrix());
      Tree* result = SharedEditionPool->push<BlockType::Matrix>(1, 2);
      SharedEditionPool->push<FloatType<T>::type>(T(dim.matrix.rows));
      SharedEditionPool->push<FloatType<T>::type>(T(dim.matrix.cols));
      return result;
    }
    case BlockType::Cross: {
      Tree* u = ToMatrix<T>(node->child(0));
      Tree* v = ToMatrix<T>(node->child(1));
      Vector::Cross(u, v);
      u->removeTree();
      u->removeTree();
      return u;
    }
    default:;
  }
  return KUndef->clone();
}

template <typename T, typename U>
U Approximation::MapAndReduce(const Tree* node, Reductor<U> reductor,
                              Mapper<std::complex<T>, U> mapper) {
  /* TODO : this function, the use of function pointers and the general
   * recursive design of ToComplex incurs some overhead when approximating. For
   * instance (a+b)*c, will execute nextNode() on a twice (one for + and one
   * for *). We should use a non-recursive and more C-like algorithm. */
  U res;
  for (auto [child, index] : NodeIterator::Children<NoEditable>(node)) {
    std::complex<T> app = ToComplex<T>(child);
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
  if (tree->isFloat() || tree->isRandomNode() ||
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
      SharedEditionPool->push<FloatType<T>::type>(To<T>(tree)));
  return true;
}

/* TODO : not all this functions are worth templating on float and
 * double. ToComplex needs it but ToMatrix could take a bool and call the
 * correct ToComplex<T> as needed since the code is mostly independant of the
 * float type used in the tree. */

template std::complex<float> Approximation::RootTreeToComplex<float>(
    const Tree*, AngleUnit, ComplexFormat);
template std::complex<double> Approximation::RootTreeToComplex<double>(
    const Tree*, AngleUnit, ComplexFormat);

template std::complex<float> Approximation::ToComplex<float>(const Tree*);
template std::complex<double> Approximation::ToComplex<double>(const Tree*);

template Tree* Approximation::RootTreeToList<float>(const Tree*, AngleUnit,
                                                    ComplexFormat);
template Tree* Approximation::RootTreeToList<double>(const Tree*, AngleUnit,
                                                     ComplexFormat);

template Tree* Approximation::RootTreeToTree<float>(const Tree*, AngleUnit,
                                                    ComplexFormat);
template Tree* Approximation::RootTreeToTree<double>(const Tree*, AngleUnit,
                                                     ComplexFormat);

template bool Approximation::ApproximateAndReplaceEveryScalarT<float>(Tree*,
                                                                      bool);
template bool Approximation::ApproximateAndReplaceEveryScalarT<double>(Tree*,
                                                                       bool);

}  // namespace PoincareJ
