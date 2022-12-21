#include <poincare/multiplication.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/code_point_layout.h>
#include <poincare/cotangent.h>
#include <poincare/dependency.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/float.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/list.h>
#include <poincare/matrix.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/tangent.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <ion.h>
#include <assert.h>
#include <cmath>
#include <utility>
#include <algorithm>

namespace Poincare {

/* Multiplication Node */

TrinaryBoolean MultiplicationNode::isPositive(Context * context) const {
  if (numberOfChildren() == 0) {
    return TrinaryBoolean::Unknown;
  }
  int sign = 1;
  for (ExpressionNode * c : children()) {
    sign *= (int)(c->isPositive(context));
    if (sign == 0) {
      return TrinaryBoolean::Unknown;
    }
  }
  return static_cast<TrinaryBoolean>(sign);
}

int MultiplicationNode::polynomialDegree(Context * context, const char * symbolName) const {
  int degree = 0;
  for (ExpressionNode * c : children()) {
    int d = c->polynomialDegree(context, symbolName);
    if (d < 0) {
      return -1;
    }
    degree += d;
  }
  return degree;
}

TrinaryBoolean MultiplicationNode::isNull(Context * context) const {
  if (numberOfChildren() == 0) {
    return TrinaryBoolean::Unknown;
  }
  /* If multiplying elements with same null-status, multiplication has this
   * status. If the null-status are different, we return Unknown because it
   * could be inf * 0. */
  TrinaryBoolean isNull = childAtIndex(0)->isNull(context);
  int childrenNumber = numberOfChildren();
  for (int i = 1; i < childrenNumber; i++) {
    if (childAtIndex(i)->isNull(context) != isNull) {
      return TrinaryBoolean::Unknown;
    }
  }
  return isNull;
}

int MultiplicationNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  return Multiplication(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

bool MultiplicationNode::childAtIndexNeedsUserParentheses(const Expression & child, int childIndex) const {
  if (NAryInfixExpressionNode::childAtIndexNeedsUserParentheses(child, childIndex)) {
    return true;
  }
  return child.isOfType({Type::Subtraction, Type::Addition});
}

Expression MultiplicationNode::removeUnit(Expression * unit) {
  return Multiplication(this).removeUnit(unit);
}

double MultiplicationNode::degreeForSortingAddition(bool symbolsOnly) const {
  /* If we consider the symbol degree, the degree of a multiplication is
   * the sum of the degrees of its terms :
   * 3*(x^2)*y -> deg = 0+2+1 = 3.
   *
   * If we consider the degree of any term, we choose that the degree of a
   * multiplication is the degree of the most-right term :
   * 4*sqrt(2) -> deg = 0.5.
   *
   * This is to ensure that deg(5) > deg(5*sqrt(3)) and deg(x^4) > deg(x*y^3)
   * */
  if (symbolsOnly) {
    double degree = 0.;
    for (ExpressionNode * c : children()) {
      degree += c->degreeForSortingAddition(symbolsOnly);
    }
    return degree;
  }
  assert(numberOfChildren() > 0);
  return childAtIndex(numberOfChildren() - 1)->degreeForSortingAddition(symbolsOnly);
}

template<typename T> Complex<T> MultiplicationNode::computeOnComplex(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat) {
  // Special case to prevent (inf,0)*(1,0) from returning (inf, nan).
  if (std::isinf(std::abs(c)) || std::isinf(std::abs(d))) {
    constexpr T zero = static_cast<T>(0.0);
    // Handle case of pure imaginary/real multiplications
    if (c.imag() == zero && d.imag() == zero) {
      return Complex<T>::Builder(c.real()*d.real(), zero);
    }
    if (c.real() == zero && d.real() == zero) {
      return Complex<T>::Builder(-c.imag()*d.imag(), zero);
    }
    if (c.imag() == zero && d.real() == zero) {
      return Complex<T>::Builder(zero, c.real()*d.imag());
    }
    if (c.real() == zero && d.imag() == zero) {
      return Complex<T>::Builder(zero, c.imag()*d.real());
    }
    // Other cases are left to the standard library, and might return NaN.
  }
  return Complex<T>::Builder(c*d);
}

template<typename T>
MatrixComplex<T> MultiplicationNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
  if (m.numberOfColumns() != n.numberOfRows()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfRows(); i++) {
    for (int j = 0; j < n.numberOfColumns(); j++) {
      std::complex<T> c(0.0);
      for (int k = 0; k < m.numberOfColumns(); k++) {
        c += m.complexAtIndex(i*m.numberOfColumns()+k)*n.complexAtIndex(k*n.numberOfColumns()+j);
      }
      result.addChildAtIndexInPlace(Complex<T>::Builder(c), i*n.numberOfColumns()+j, result.numberOfChildren());
    }
  }
  result.setDimensions(m.numberOfRows(), n.numberOfColumns());
  return result;
}

/* Operative symbol between two expressions depends on the layout shape on the
 * left and the right of the operator:
 *
 * Left  \ Right | Decimal | Integer | OneLetter | MoreLetters | BundaryPunct. | Root | NthRoot | Fraction | Unit |   Default
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Decimal       |    ×    |    ×    |     ø     |      ×      |       ×       |  ×   |    ×    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Integer       |    ×    |    ×    |     ø     |      •      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * OneLetter     |    ×    |    •    |     •     |      •      |       •       |  ø   |    •    |    ø     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * MoreLetters   |    ×    |    •    |     •     |      •      |       •       |  ø   |    •    |    ø     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * BundaryPunct. |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Root          |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Fraction      |    ×    |    ×    |     ø     |      ø      |       ø       |  ø   |    •    |    ×     |  ø   |      •
 * --------------+---------+---------+-----------+-------------+---------------+------+---------+----------+------+-------------
 * Default       |    •    |    •    |     •     |      •      |       •       |  •   |    •    |    •     |  ø   |      •
 *
 * Two Units are separated by a •, Unit on the left is treated according to its type
 * */

MultiplicationNode::MultiplicationSymbol MultiplicationNode::OperatorSymbolBetween(ExpressionNode::LayoutShape left, ExpressionNode::LayoutShape right) {
  if (left == ExpressionNode::LayoutShape::Default || right == ExpressionNode::LayoutShape::Default) {
    return MultiplicationSymbol::MiddleDot;
  }
  switch (left) {
    case ExpressionNode::LayoutShape::Decimal:
      switch (right) {
        case ExpressionNode::LayoutShape::OneLetter:
          return MultiplicationSymbol::Empty;
        default:
          return MultiplicationSymbol::MultiplicationSign;
      }
    case ExpressionNode::LayoutShape::Integer:
      switch (right) {
        case ExpressionNode::LayoutShape::Integer:
        case ExpressionNode::LayoutShape::Decimal:
        case ExpressionNode::LayoutShape::Fraction:
          return MultiplicationSymbol::MultiplicationSign;
        case ExpressionNode::LayoutShape::MoreLetters:
        case ExpressionNode::LayoutShape::NthRoot:
          return MultiplicationSymbol::MiddleDot;
        default:
          return MultiplicationSymbol::Empty;
      }
    case ExpressionNode::LayoutShape::OneLetter:
    case ExpressionNode::LayoutShape::MoreLetters:
      switch (right) {
        case ExpressionNode::LayoutShape::Decimal:
          return MultiplicationSymbol::MultiplicationSign;
        case ExpressionNode::LayoutShape::Fraction:
        case ExpressionNode::LayoutShape::Root:
          return MultiplicationSymbol::Empty;
        default:
          return MultiplicationSymbol::MiddleDot;
      }
    default:
    //case ExpressionNode::LayoutShape::BoundaryPunctuation:
    //case ExpressionNode::LayoutShape::Fraction:
    //case ExpressionNode::LayoutShape::Root:
      switch (right) {
        case ExpressionNode::LayoutShape::Decimal:
        case ExpressionNode::LayoutShape::Integer:
        case ExpressionNode::LayoutShape::Fraction:
          return MultiplicationSymbol::MultiplicationSign;
        case ExpressionNode::LayoutShape::NthRoot:
          return MultiplicationSymbol::MiddleDot;
        default:
          return MultiplicationSymbol::Empty;
      }
  }
}

CodePoint MultiplicationNode::CodePointForOperatorSymbol(MultiplicationSymbol symbol) {
  switch (symbol) {
    case MultiplicationSymbol::Empty:
      return UCodePointNull;
    case MultiplicationSymbol::MiddleDot:
      return UCodePointMiddleDot;
    default:
      assert(symbol == MultiplicationSymbol::MultiplicationSign);
      return UCodePointMultiplicationSign;
  }
}

static bool ExpressionIsUnit(Expression e, bool * shouldLockMargin = nullptr) {
  Expression unitExpression;
  if (e.type() == ExpressionNode::Type::Unit) {
    unitExpression = e;
  } else if (e.type() == ExpressionNode::Type::Power && e.childAtIndex(0).type() == ExpressionNode::Type::Unit) {
    unitExpression =  e.childAtIndex(0);
  } else {
    return false;
  }
  if (shouldLockMargin) {
    *shouldLockMargin = Unit::ForceMarginLeftOfUnit(static_cast<Unit&>(unitExpression));
  }
  return true;
}

CodePoint MultiplicationNode::operatorSymbol() const {
  int sign = -1;
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber - 1; i++) {
    /* The operator symbol must be the same for all operands of the multiplication.
     * If one operator has to be '×', they will all be '×'. Idem for '·'. */
    Expression left = childAtIndex(i);
    Expression right = childAtIndex(i+1);
    MultiplicationSymbol symbol;
    if (ExpressionIsUnit(right)) {
      symbol = ExpressionIsUnit(left) ? MultiplicationSymbol::MiddleDot : MultiplicationSymbol::Empty;
    } else {
      symbol = OperatorSymbolBetween(left.rightLayoutShape(), right.leftLayoutShape());
    }
    sign = std::max(sign, static_cast<int>(symbol));
  }
  assert(sign >= 0);
  return CodePointForOperatorSymbol(static_cast<MultiplicationSymbol>(sign));
}

Layout MultiplicationNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, operatorSymbol());
  return LayoutHelper::Infix(
    Multiplication(this),
    floatDisplayMode,
    numberOfSignificantDigits,
    string,
    context,
    [](const char * operatorName, Expression left, Expression right, Layout rightLayout) {
      bool forceMarginOfRightUnit = true;
      bool leftIsUnit = ExpressionIsUnit(left, nullptr);
      bool rightIsUnit = ExpressionIsUnit(right, &forceMarginOfRightUnit);
      if (rightIsUnit && leftIsUnit) {
        // Both children are unit: symbol is always a middle dot with no margin
        Layout symbolLayout = CodePointLayout::Builder(CodePointForOperatorSymbol(MultiplicationSymbol::MiddleDot));
        symbolLayout.setMargin(false);
        rightLayout.setMargin(false);
        return symbolLayout;
      }
      if (rightIsUnit) {
        // Unit on the right: don't display useless symbol and force margin
        rightLayout.setMargin(true);
        rightLayout.lockMargin(forceMarginOfRightUnit);
        return Layout();
      }
      return LayoutHelper::DefaultCreateOperatorLayoutForInfix(operatorName, left, right, rightLayout);
    });
}

int MultiplicationNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  constexpr int stringMaxSize = CodePoint::MaxCodePointCharLength + 1;
  char string[stringMaxSize];
  SerializationHelper::CodePoint(string, stringMaxSize, UCodePointMultiplicationSign);
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, string);
}

Expression MultiplicationNode::shallowReduce(const ReductionContext& reductionContext) {
  return Multiplication(this).shallowReduce(reductionContext);
}

Expression MultiplicationNode::shallowBeautify(const ReductionContext& reductionContext) {
  return Multiplication(this).shallowBeautify(reductionContext);
}

bool MultiplicationNode::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return Multiplication(this).derivate(reductionContext, symbol, symbolValue);
}

/* Multiplication */

int Multiplication::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  // Initialization of coefficients
  for (int i = 1; i <= deg; i++) {
    coefficients[i] = Rational::Builder(0);
  }
  coefficients[0] = Rational::Builder(1);

  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Let's note result = a(0)+a(1)*X+a(2)*X^2+a(3)*x^3+..
  int childrenNumber = numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    // childAtIndex(i) = b(0)+b(1)*X+b(2)*X^2+b(3)*x^3+...
    int degI = childAtIndex(i).getPolynomialCoefficients(context, symbolName, intermediateCoefficients);
    assert(degI <= Expression::k_maxPolynomialDegree);
    for (int j = deg; j > 0; j--) {
      // new coefficients[j] = b(0)*a(j)+b(1)*a(j-1)+b(2)*a(j-2)+...
      Addition a = Addition::Builder();
      int jbis = j > degI ? degI : j;
      for (int l = 0; l <= jbis ; l++) {
        // Always copy the a and b coefficients are they are used multiple times
        a.addChildAtIndexInPlace(Multiplication::Builder(intermediateCoefficients[l].clone(), coefficients[j-l].clone()), a.numberOfChildren(), a.numberOfChildren());
      }
      /* a(j) and b(j) are used only to compute coefficient at rank >= j, we
       * can delete them as we compute new coefficient by decreasing ranks. */
      coefficients[j] = a;
    }
    // new coefficients[0] = a(0)*b(0)
    coefficients[0] = Multiplication::Builder(coefficients[0], intermediateCoefficients[0]);
  }
  return deg;
}

Expression Multiplication::removeUnit(Expression * unit) {
  Multiplication unitMult = Multiplication::Builder();
  int resultChildrenCount = 0;
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression childI = childAtIndex(i);
    assert(!childI.isUndefined());
    Expression currentUnit;
    childI = childI.removeUnit(&currentUnit);
    if (childI.isUndefined()) {
      /* If the child was a unit convert, it replaced itself with an undefined
       * during the removeUnit. */
      *unit = Expression();
      return replaceWithUndefinedInPlace();
    }
    if (!currentUnit.isUninitialized()) {
      unitMult.addChildAtIndexInPlace(currentUnit, resultChildrenCount, resultChildrenCount);
      resultChildrenCount++;
      assert(childAtIndex(i).isRationalOne());
      removeChildAtIndexInPlace(i--);  // Remove unit node
    }
  }
  if (resultChildrenCount == 0) {
    *unit = Expression();
  } else {
    *unit = unitMult.squashUnaryHierarchyInPlace();
  }
  if (numberOfChildren() == 0) {
    Expression one = Rational::Builder(1);
    replaceWithInPlace(one);
    return one;
  }
  return squashUnaryHierarchyInPlace();
}

template<typename T>
void Multiplication::computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns) {
  for (int i = 0; i < mNumberOfRows; i++) {
    for (int j = 0; j < nNumberOfColumns; j++) {
      T res = 0.0f;
      for (int k = 0; k < mNumberOfColumns; k++) {
        res+= m[i*mNumberOfColumns+k]*n[k*nNumberOfColumns+j];
      }
      result[i*nNumberOfColumns+j] = res;
    }
  }
}

static bool CanSimplifyUnitProduct(
    const UnitNode::Vector<int> &unitsExponents, size_t &unitsSupportSize,
    const UnitNode::Vector<int> * entryUnitExponents, int entryUnitExponent,
    int8_t &bestUnitExponent, UnitNode::Vector<int> &bestRemainderExponents, size_t &bestRemainderSupportSize) {
  /* This function tries to simplify a Unit product (given as the
   * 'unitsExponents' int array), by applying a given operation. If the
   * result of the operation is simpler, 'bestUnit' and
   * 'bestRemainder' are updated accordingly. */
  UnitNode::Vector<int> simplifiedExponents;

  #if 0
  /* In the current algorithm, simplification is attempted using derived units
   * with no exponents. Some good simplifications might be missed:
   *    For instance with _A^2*_s^2, a first attempt will be to simplify to
   *    _C_A_s which has a bigger supportSize and will not be kept, the output
   *    will stay _A^2*_s^2.
   * With the commented code, this issue is solved by trying to simplify with
   * the highest exponent possible, so that, in this example, _A^2*_s^2 can be
   * simplified to _C^2.
   * An optimization might be possible using algorithms minimizing the sum of
   * absolute difference of array elements */
  int n = 0;
  int best_norm;
  // TODO define a norm function summing all base units exponents
  int norm_temp = unitsExponents.norm();
  /* To extend this algorithm to square root simplifications, rational exponents
   * can be handled, and a 1/2 step can be used (but it should be asserted that
   * no square root simplification is performed if all exponents are integers.*/
  int step = 1;
  for (size_t i = 0; i < Unit::NumberOfBaseUnits; i++) {
    // Set simplifiedExponents to unitsExponents
    simplifiedExponents.setCoefficientAtIndex(i, unitsExponents.coefficientAtIndex(i));
  }
  do {
    best_norm = norm_temp;
    n+= step;
    for (size_t i = 0; i < Unit::NumberOfBaseUnits; i++) {
      // Simplify unitsExponents with base units from derived unit
      simplifiedExponents.setCoefficientAtIndex(i, simplifiedExponents.coefficientAtIndex(i) - entryUnitExponent * step * entryUnitExponents->coefficientAtIndex(i));
    }
    int simplifiedNorm = simplifiedExponents.norm();
    // Temp norm is derived norm (n) + simplified norm
    norm_temp = n + simplifiedNorm;
  } while (norm_temp < best_norm);
  // Undo last step as it did not reduce the norm
  n -= step;
  #endif

  for (size_t i = 0; i < UnitNode::k_numberOfBaseUnits; i++) {
    #if 0
    // Undo last step as it did not reduce the norm
    simplifiedExponents.setCoefficientAtIndex(i, simplifiedExponents.coefficientAtIndex(i) + entryUnitExponent * step * entryUnitExponents->coefficientAtIndex(i));
    #else
    // Simplify unitsExponents with base units from derived unit
    simplifiedExponents.setCoefficientAtIndex(i, unitsExponents.coefficientAtIndex(i) - entryUnitExponent * entryUnitExponents->coefficientAtIndex(i));
    #endif
  }
  size_t simplifiedSupportSize = simplifiedExponents.supportSize();
  /* Note: A metric is considered simpler if the support size (number of
   * symbols) is reduced. A norm taking coefficients into account is possible.
   * One could use the sum of all coefficients to favor _C_s from _A_s^2.
   * However, replacing _m_s^-2 with _N_kg^-1 should be avoided. */
  bool isSimpler = (1 + simplifiedSupportSize < unitsSupportSize);

  if (isSimpler) {
    #if 0
    bestUnitExponent = entryUnitExponent * n * step;
    #else
    bestUnitExponent = entryUnitExponent;
    #endif
    bestRemainderExponents = simplifiedExponents;
    bestRemainderSupportSize = simplifiedSupportSize;
    /* unitsSupportSize is updated and will be taken into
     * account in next iterations of CanSimplifyUnitProduct. */
    unitsSupportSize = 1 + simplifiedSupportSize;
  }
  return isSimpler;
}

Expression Multiplication::shallowBeautify(const ExpressionNode::ReductionContext& reductionContext) {
  /* Beautifying a Multiplication consists in several possible operations:
   * - Add Opposite ((-3)*x -> -(3*x), useful when printing fractions)
   * - Recognize derived units in the product of units
   * - Creating a Division if relevant
   */

  // Step 1: Turn -n*A into -(n*A)
  Expression noNegativeNumeral = makePositiveAnyNegativeNumeralFactor(reductionContext);
  // If one negative numeral factor was made positive, we turn the expression in an Opposite
  if (!noNegativeNumeral.isUninitialized()) {
    Opposite o = Opposite::Builder();
    noNegativeNumeral.replaceWithInPlace(o);
    o.replaceChildAtIndexInPlace(0, noNegativeNumeral);
    return std::move(o);
  }

  Expression result;
  Expression self = *this;

  // Step 2: Handle the units
  if (hasUnit()) {
    Expression units;
    /* removeUnit has to be called on reduced expression but we want to modify
     * the least the expression so we use the uninvasive reduction context. */
    self = self.reduceAndRemoveUnit(ExpressionNode::ReductionContext::NonInvasiveReductionContext(reductionContext), &units);

    if (self.isUndefined() || units.isUninitialized()) {
      // TODO: handle error "Invalid unit"
      result = Undefined::Builder();
      goto replace_by_result;
    }

    ExpressionNode::UnitConversion unitConversionMode = reductionContext.unitConversion();
    if (units.isPureAngleUnit()) {
      if (unitConversionMode == ExpressionNode::UnitConversion::Default) {
        // Pure angle unit is the only unit allowed to be evaluated exactly
        double value = self.approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
        Expression toUnit = units.clone();
        Unit::ChooseBestRepresentativeAndPrefixForValue(toUnit, &value, reductionContext);
        // Divide the left member by the new unit
        Expression division = Division::Builder(Multiplication::Builder(self.clone(), units), toUnit.clone());
        Expression divisionUnit;
        division = division.reduceAndRemoveUnit(reductionContext, &divisionUnit);
        assert(divisionUnit.isUninitialized());
        division = division.shallowReduce(reductionContext).shallowBeautify(reductionContext);
        result = Multiplication::Builder(division, toUnit);
      } else {
        result = Multiplication::Builder(self.clone().shallowBeautify(reductionContext), units);
      }
      assert(!result.isUninitialized());
      self.replaceWithInPlace(result);
      return result;
    }

    if (unitConversionMode == ExpressionNode::UnitConversion::Default) {
      /* Step 2a: Recognize derived units
       * - Look up in the table of derived units, the one which itself or its inverse simplifies 'units' the most.
       * - If an entry is found, simplify 'units' and add the corresponding unit or its inverse in 'unitsAccu'.
       * - Repeat those steps until no more simplification is possible.
       */
      Multiplication unitsAccu = Multiplication::Builder();
      /* If exponents are not integers, FromBaseUnits will return a null
       * vector, preventing any attempt at simplification. This protects us
       * against undue "simplifications" such as _C^1.3 -> _C*_A^0.3*_s^0.3 */
      UnitNode::Vector<int> unitsExponents = UnitNode::Vector<int>::FromBaseUnits(units);
      size_t unitsSupportSize = unitsExponents.supportSize();
      UnitNode::Vector<int> bestRemainderExponents;
      size_t bestRemainderSupportSize;
      while (unitsSupportSize > 1) {
        const UnitNode::Representative * bestDim = nullptr;
        int8_t bestUnitExponent = 0;
        // Look up in the table of derived units.
        for (int i = UnitNode::k_numberOfBaseUnits; i < UnitNode::Representative::k_numberOfDimensions - 1; i++) {
          const UnitNode::Representative * dim = UnitNode::Representative::DefaultRepresentatives()[i];
          const UnitNode::Vector<int> entryUnitExponents = dim->dimensionVector();
          // A simplification is tried by either multiplying or dividing
          if (CanSimplifyUnitProduct(
                unitsExponents, unitsSupportSize,
                &entryUnitExponents, 1,
                bestUnitExponent, bestRemainderExponents, bestRemainderSupportSize
                )
              ||
              CanSimplifyUnitProduct(
                unitsExponents, unitsSupportSize,
                &entryUnitExponents, -1,
                bestUnitExponent, bestRemainderExponents, bestRemainderSupportSize
                ))
          {
          /* If successful, unitsSupportSize, bestUnitExponent,
           * bestRemainderExponents and bestRemainderSupportSize have been updated*/
            bestDim = dim;
          }
        }
        if (bestDim == nullptr) {
          // No simplification could be performed
          break;
        }
        // Build and add the best derived unit
        Expression derivedUnit = Unit::Builder(bestDim->representativesOfSameDimension(), bestDim->basePrefix());

        #if 0
        if (bestUnitExponent != 1) {
          derivedUnit = Power::Builder(derivedUnit, Rational::Builder(bestUnitExponent));
        }
        #else
        assert(bestUnitExponent == 1 || bestUnitExponent == -1);
        if (bestUnitExponent == -1) {
          derivedUnit = Power::Builder(derivedUnit, Rational::Builder(-1));
        }
        #endif

        const int position = unitsAccu.numberOfChildren();
        unitsAccu.addChildAtIndexInPlace(derivedUnit, position, position);
        // Update remainder units and their exponents for next simplifications
        unitsExponents = bestRemainderExponents;
        unitsSupportSize = bestRemainderSupportSize;
      }
      // Apply simplifications
      if (unitsAccu.numberOfChildren() > 0) {
        Expression newUnits;
        // Divide by derived units, separate units and generated values
        units = Division::Builder(units, unitsAccu.clone()).reduceAndRemoveUnit(reductionContext, &newUnits);
        // Assemble final value
        Multiplication m = Multiplication::Builder(units);
        self.replaceWithInPlace(m);
        m.addChildAtIndexInPlace(self, 0, 1);
        self = m;
        // Update units with derived and base units
        if (newUnits.isUninitialized()) {
          units = unitsAccu;
        } else {
          units = Multiplication::Builder(unitsAccu, newUnits);
          static_cast<Multiplication &>(units).mergeSameTypeChildrenInPlace();
        }
      }
    }

    /* Step 2b: Turn into 'Float x units'.
     * Choose a unit multiple adequate for the numerical value.
     * An exhaustive exploration of all possible multiples would have
     * exponential complexity with respect to the number of factors. Instead,
     * we focus on one single factor. The first Unit factor is certainly the
     * most relevant.
     */

    double value = self.approximateToScalar<double>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
    if (std::isnan(value)) {
      // If the value is undefined, return "undef" without any unit
      result = Undefined::Builder();
    } else {
      if (unitConversionMode == ExpressionNode::UnitConversion::Default) {
        // Find the right unit prefix
        /* In most cases, unit composition works the same for imperial and
         * metric units. However, in imperial, we want volumes to be displayed
         * using volume units instead of cubic length. */
        const bool forceVolumeRepresentative = reductionContext.unitFormat() == Preferences::UnitFormat::Imperial && UnitNode::Vector<int>::FromBaseUnits(units) == UnitNode::VolumeRepresentative::Default().dimensionVector();
        const UnitNode::Representative * repr;
        if (forceVolumeRepresentative) {
          /* The choice of representative doesn't matter, as it will be tuned to a
           * system appropriate one in Step 2b. */
          repr = UnitNode::VolumeRepresentative::Default().representativesOfSameDimension();
          units = Unit::Builder(repr, UnitNode::Prefix::EmptyPrefix());
          value /= repr->ratio();
          Unit::ChooseBestRepresentativeAndPrefixForValue(units, &value, reductionContext);
        } else {
          Unit::ChooseBestRepresentativeAndPrefixForValue(units, &value, reductionContext);
        }
      }
      // Build final Expression
      result = Multiplication::Builder(Number::FloatNumber(value), units);
      static_cast<Multiplication &>(result).mergeSameTypeChildrenInPlace();
    }
  } else {
  /* Step 3: cos(x)/sin(x) is not always reduced to 1/tan(x), so we need to
   * turn into cot(x) here.
   *This only handle the simple cos(x)/sin(x) case and not cos(x)^p/sin(x)^q*/
  for (int i = 0; i < numberOfChildren(); i++) {
    Expression child = childAtIndex(i);
    if (child.type() == ExpressionNode::Type::Power
        && child.childAtIndex(0).type() == ExpressionNode::Type::Sine
        && IsMinusOne(child.childAtIndex(1))) {
      for (int j = i+1; j < numberOfChildren(); j++) {
        // Cosine are after sine in simplification order
        Expression otherChild = childAtIndex(j);
        if (otherChild.type() == ExpressionNode::Type::Cosine
            && otherChild.childAtIndex(0).isIdenticalTo(child.childAtIndex(0).childAtIndex(0))) {
          Expression cotangent = Cotangent::Builder(otherChild.childAtIndex(0));
          replaceChildAtIndexInPlace(i, cotangent);
          removeChildAtIndexInPlace(j);
          break;
        }
      }
    }
  }
  // Step 4: Create a Division if relevant
    Expression numer, denom;
    splitIntoNormalForm(numer, denom, reductionContext);
    if (!numer.isUninitialized()) {
      result = numer;
    }
    if (!denom.isUninitialized()) {
      result = Division::Builder(numer.isUninitialized() ? Rational::Builder(1) : numer, denom);
    }
  }

replace_by_result:
  self.replaceWithInPlace(result);
  return result;
}

Expression Multiplication::denominator(const ExpressionNode::ReductionContext& reductionContext) const {
  /* TODO ?
   * Turn the denominator const method into an extractDenominator method
   * (non const) in the same same way as extractUnits.
   * Then remove splitIntoNormalForm.
   */
  Expression numer, denom;
  splitIntoNormalForm(numer, denom, reductionContext);
  return denom;
}

bool Multiplication::derivate(const ExpressionNode::ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  {
    Expression e = Derivative::DefaultDerivate(*this, reductionContext, symbol);
    if (!e.isUninitialized()) {
      return true;
    }
  }

  Addition resultingAddition = Addition::Builder();
  int numberOfTerms = numberOfChildren();
  assert (numberOfTerms > 0);
  Expression childI;

  for (int i = 0; i < numberOfTerms; ++i) {
    childI = clone();
    childI.derivateChildAtIndexInPlace(i, reductionContext, symbol, symbolValue);
    resultingAddition.addChildAtIndexInPlace(childI, i, i);
  }

  replaceWithInPlace(resultingAddition);
  return true;
}

Expression Multiplication::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this, &reductionContext, SimplificationHelper::BooleanReduction::UndefinedOnBooleans);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  bool shouldExpand = reductionContext.shouldExpandMultiplication();
  bool productHasUnit = hasUnit();

  /* Before merging with multiplication children, we must catch a forbidden
   * case of unit reduction. */
  if (productHasUnit && Unit::IsForbiddenTemperatureProduct(*this)) {
    return replaceWithUndefinedInPlace();
  }

  /* MultiplicationNode is associative, so let's start by merging children
   * which also are multiplications themselves.
   * TODO If the parent Expression is a Multiplication, one should perhaps
   * return now and let the parent do the reduction.
   */
  mergeSameTypeChildrenInPlace();

  Context * context = reductionContext.context();

  // Sort the children
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, true); }, context, reductionContext.shouldCheckMatrices());

  /* Distribute the multiplication over lists */
  Expression distributed = SimplificationHelper::distributeReductionOverLists(*this, reductionContext);
  if (!distributed.isUninitialized()) {
    return distributed;
  }

  // Handle matrices
  /* Thanks to the simplification order, all matrix children (if any) are the
   * last children. */
  Expression lastChild = childAtIndex(numberOfChildren()-1);
  if (lastChild.type() == ExpressionNode::Type::Matrix) {
    Matrix resultMatrix = static_cast<Matrix &>(lastChild);
    // Use the last matrix child as the final matrix
    int n = resultMatrix.numberOfRows();
    int m = resultMatrix.numberOfColumns();
    /* Scan across the children to find other matrices. The last child is the
     * result matrix so we start at numberOfChildren()-2. */
    int multiplicationChildIndex = numberOfChildren()-2;
    while (multiplicationChildIndex >= 0) {
      Expression currentChild = childAtIndex(multiplicationChildIndex);
      if (currentChild.type() != ExpressionNode::Type::Matrix) {
        break;
      }
      Matrix currentMatrix = static_cast<Matrix &>(currentChild);
      int currentN = currentMatrix.numberOfRows();
      int currentM = currentMatrix.numberOfColumns();
      if (currentM != n) {
        // Matrices dimensions do not match for multiplication
        return replaceWithUndefinedInPlace();
      }
      /* Create the matrix resulting of the multiplication of the current matrix
       * and the result matrix
       *                                resultMatrix
       *                                  i2= 0..m
       *                                +-+-+-+-+-+
       *                                | | | | | |
       *                                +-+-+-+-+-+
       *                         j=0..n | | | | | |
       *                                +-+-+-+-+-+
       *                                | | | | | |
       *                                +-+-+-+-+-+
       *                currentMatrix
       *                j=0..currentM
       *                +---+---+---+   +-+-+-+-+-+
       *                |   |   |   |   | | | | | |
       *                +---+---+---+   +-+-+-+-+-+
       * i1=0..currentN |   |   |   |   | |e| | | |
       *                +---+---+---+   +-+-+-+-+-+
       *                |   |   |   |   | | | | | |
       *                +---+---+---+   +-+-+-+-+-+
       * */
      int newResultN = currentN;
      int newResultM = m;
      Matrix newResult = Matrix::Builder();
      for (int i = 0; i < newResultN; i++) {
        for (int j = 0; j < newResultM; j++) {
          Addition a = Addition::Builder();
          for (int k = 0; k < n; k++) {
            Expression e = Multiplication::Builder(currentMatrix.matrixChild(i, k).clone(), resultMatrix.matrixChild(k, j).clone());
            a.addChildAtIndexInPlace(e, a.numberOfChildren(), a.numberOfChildren());
            e.shallowReduce(reductionContext);
          }
          newResult.addChildAtIndexInPlace(a, newResult.numberOfChildren(), newResult.numberOfChildren());
          a.shallowReduce(reductionContext);
        }
      }
      newResult.setDimensions(newResultN, newResultM);
      n = newResultN;
      m = newResultM;
      removeChildInPlace(currentMatrix, currentMatrix.numberOfChildren());
      replaceChildInPlace(resultMatrix, newResult);
      resultMatrix = newResult;
      multiplicationChildIndex--;
    }
    /* Distribute the remaining multiplication children on the matrix children,
     * if there are no other matrices.
     */

    if (multiplicationChildIndex >= 0) {
      if (childAtIndex(multiplicationChildIndex).deepIsMatrix(context, reductionContext.shouldCheckMatrices())) {
        return *this;
      }
      removeChildInPlace(resultMatrix, resultMatrix.numberOfChildren());
      for (int i = 0; i < n*m; i++) {
        Multiplication m = clone().convert<Multiplication>();
        Expression entryI = resultMatrix.childAtIndex(i);
        resultMatrix.replaceChildInPlace(entryI, m);
        m.addChildAtIndexInPlace(entryI, m.numberOfChildren(), m.numberOfChildren());
        m.shallowReduce(reductionContext);
      }
    }
    replaceWithInPlace(resultMatrix);
    return resultMatrix.shallowReduce(reductionContext);
  }

  // Gather like terms together
  Expression gatheredExpression = gatherLikeTerms(reductionContext);
  if (!gatheredExpression.isUninitialized()) {
    return gatheredExpression;
  }

  /* We look for terms of form sin(x)^p*cos(x)^q with p, q rational of
   * opposite signs. We replace them by either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise
   * We need to do this here for ReductionTarget::User (and not in beautification)
   * because we don't want Addition to put eveything under a common denominator
   * for example : tan(3)ln(2)+π --> sin(3)/cos(3)ln(2)+π --> (sin(3)ln(2) + πcos(3)) / cos(3)
   * won't be beautificated into tan(3)ln(2)+π */
  bool hasFactorizedTangent = false;
  if (reductionContext.target() == ExpressionNode::ReductionTarget::User) {
    int childrenNumber = numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      Expression o1 = childAtIndex(i);
      if (Base(o1).type() == ExpressionNode::Type::Sine && TermHasNumeralExponent(o1)) {
        const Expression x = Base(o1).childAtIndex(0);
        /* Thanks to the SimplificationOrder, Cosine-base factors are after
         * Sine-base factors */
        for (int j = i+1; j < childrenNumber; j++) {
          Expression o2 = childAtIndex(j);
          if (Base(o2).type() == ExpressionNode::Type::Cosine && TermHasNumeralExponent(o2) && Base(o2).childAtIndex(0).isIdenticalTo(x)) {
            hasFactorizedTangent = factorizeSineAndCosine(i, j, reductionContext) || hasFactorizedTangent;
            break;
          }
        }
      }
    }
  }

  if (hasFactorizedTangent) {
    /* Regather terms in case some factors that already existed have appeared.
     * For example: tan(3)*cos(3)^-1*sin(3) = tan(3)*tan(3) */
    gatheredExpression = gatherLikeTerms(reductionContext);
    if (!gatheredExpression.isUninitialized()) {
      return gatheredExpression;
    }
  }

  /* We remove rational children that appeared in the middle of sorted
   * children. It's important to do this after having factorized because
   * factorization can lead to new ones. Indeed:
   * pi^(-1)*pi-> 1
   * i*i -> -1
   * 2^(1/2)*2^(1/2) -> 2
   * sin(x)*cos(x) -> 1*tan(x)
   */
  int i = 1;
  while (i < numberOfChildren()) {
    Expression o = childAtIndex(i);
    if (o.type() == ExpressionNode::Type::Rational && static_cast<Rational &>(o).isOne()) {
      removeChildAtIndexInPlace(i);
      continue;
    }
    if (o.isNumber()) {
      if (childAtIndex(0).isNumber()) {
        Expression o0 = childAtIndex(0);
        Number m = Number::Multiplication(static_cast<Number &>(o0), static_cast<Number &>(o));
        if ((IsInfinity(m, context) || m.isUndefined())
            && !IsInfinity(o0, context) && !o0.isUndefined()
            && !IsInfinity(o, context) && !o.isUndefined())
        {
          // Stop the reduction due to a multiplication overflow
          ExceptionCheckpoint::Raise();
        }
        if (m.isUndefined()) {
          return replaceWithUndefinedInPlace();
        }
        replaceChildAtIndexInPlace(0, m);
        removeChildAtIndexInPlace(i);
      } else {
        // Number child has to be first
        removeChildAtIndexInPlace(i);
        addChildAtIndexInPlace(o, 0, numberOfChildren());
      }
      continue;
    }
    i++;
  }

   /* If the first child is zero, the multiplication result is zero. We
    * do this after merging the rational children, because the merge takes care
    * of turning 0*inf into undef. We still have to check that no other child
    * involves an infinity expression to avoid reducing 0*e^(inf) to 0.
    * If the first child is 1, we remove it if there are other children. */
  {
    const Expression c = childAtIndex(0);
    if (productHasUnit) {
      // Do not expand Multiplication in presence of units
      shouldExpand = false;
    } else if (c.type() != ExpressionNode::Type::Rational) {
    } else if (static_cast<const Rational &>(c).isZero()) {
      // Check that other children don't match inf or matrix
      if (!recursivelyMatches([](const Expression e, Context * context) { return IsInfinity(e, context) || IsMatrix(e, context); }, context)) {
        if (numberOfChildren() > 1) {
          /* If we replace the multiplication with 0, we must add a dependency
           * in case the other terms of the multiplication are undef.
           * For example, if f(x) = 0*(1/x), then f(0) = undef. */
          Dependency dep = Dependency::Builder(c, List::Builder());
          removeChildAtIndexInPlace(0);
          replaceWithInPlace(dep);
          dep.addDependency(numberOfChildren() > 1 ? *this : childAtIndex(0));
          return dep.shallowReduce(reductionContext);
        }
        replaceWithInPlace(c);
        return c;
      }
    } else if (static_cast<const Rational &>(c).isOne() && numberOfChildren() > 1) {
      removeChildAtIndexInPlace(0);
    }
  }

  /* Expand multiplication over addition children if any. For example,
   * turn (a+b)*c into a*c + b*c. We do not want to do this step right now if
   * the parent is a multiplication or if the reduction is done bottom up to
   * avoid missing factorization such as (x+y)^(-1)*((a+b)*(x+y)).
   * Note: This step must be done after gathering similar terms, otherwise we
   * wouldn't be able to reduce expressions such as (x+y)^(-1)*(x+y)(a+b).
   * If there is a random somewhere, do not expand. */
  Expression p = parent();
  bool hasRandom = recursivelyMatches(Expression::IsRandom, context);
  if (shouldExpand
      && (p.isUninitialized() || p.type() != ExpressionNode::Type::Multiplication)
      && !hasRandom)
  {
    int childrenNumber = numberOfChildren();
    for (int i = 0; i < childrenNumber; i++) {
      if (childAtIndex(i).type() == ExpressionNode::Type::Addition) {
        return distributeOnOperandAtIndex(i, reductionContext);
      }
    }
  }

  // Let's remove the multiplication altogether if it has one child
  Expression result = squashUnaryHierarchyInPlace();
  if (result != *this) {
    return result;
  }

  /* Let's bubble up the complex operator if possible
   * 3 cases:
   * - All children are real, we do nothing (allChildrenAreReal == 1)
   * - One of the child is non-real and not a ComplexCartesian: it means a
   *   complex expression could not be resolved as a ComplexCartesian, we cannot
   *   do anything about it now (allChildrenAreReal == -1)
   * - All children are either real or ComplexCartesian (allChildrenAreReal == 0)
   *   We can bubble up ComplexCartesian nodes.
   * Do not simplify if there are randoms !*/
  if (!hasRandom && allChildrenAreReal(context, reductionContext.shouldCheckMatrices()) == 0) {
    int nbChildren = numberOfChildren();
    int i = nbChildren-1;
    // Children are sorted so ComplexCartesian nodes are at the end
    assert(childAtIndex(i).type() == ExpressionNode::Type::ComplexCartesian);
    // First, we merge all ComplexCartesian children into one
    ComplexCartesian child = childAtIndex(i).convert<ComplexCartesian>();
    while (true) {
      removeChildAtIndexInPlace(i);
      i--;
      if (i < 0) {
        break;
      }
      Expression e = childAtIndex(i);
      if (e.type() != ExpressionNode::Type::ComplexCartesian) {
        // the Multiplication is sorted so ComplexCartesian nodes are the last ones
        break;
      }
      child = child.multiply(static_cast<ComplexCartesian &>(e), reductionContext);
    }
    // The real children are both factors of the real and the imaginary multiplication
    Multiplication real = *this;
    Multiplication imag = clone().convert<Multiplication>();
    real.addChildAtIndexInPlace(child.real(), real.numberOfChildren(), real.numberOfChildren());
    imag.addChildAtIndexInPlace(child.imag(), real.numberOfChildren(), real.numberOfChildren());
    ComplexCartesian newComplexCartesian = ComplexCartesian::Builder();
    replaceWithInPlace(newComplexCartesian);
    newComplexCartesian.replaceChildAtIndexInPlace(0, real);
    newComplexCartesian.replaceChildAtIndexInPlace(1, imag);
    real.shallowReduce(reductionContext);
    imag.shallowReduce(reductionContext);
    return newComplexCartesian.shallowReduce(reductionContext);
  }

  return result;
}

/* This function factorizes two children which have a common base. For example
  * if this is Multiplication::Builder(pi^2, pi^3), then pi^2 and pi^3 could be merged
  * and this turned into Multiplication::Builder(pi^5). */
void Multiplication::factorizeBase(int i, int j, const ExpressionNode::ReductionContext& reductionContext, List dependenciesCreatedDuringReduction) {
  Expression e = childAtIndex(j);
  // Step 1: Get rid of the child j
  removeChildAtIndexInPlace(j);
  // Step 2: Merge child j in child i by factorizing base
  mergeInChildByFactorizingBase(i, e, reductionContext, dependenciesCreatedDuringReduction);
}

void Multiplication::mergeInChildByFactorizingBase(int i, Expression e, const ExpressionNode::ReductionContext& reductionContext, List dependenciesCreatedDuringReduction) {
  /* This function replace the child at index i by its factorization with e. e
   * and childAtIndex(i) are supposed to have a common base. */
  // Step 1: Find the new exponent
  Expression s = Addition::Builder(CreateExponent(childAtIndex(i)), CreateExponent(e)); // pi^2*pi^3 -> pi^(2+3) -> pi^5
  // Step 2: Create the new Power
  Expression p = Power::Builder(Base(childAtIndex(i).clone()), s); // pi^2*pi^-2 -> pi^0 -> 1
  s.shallowReduce(reductionContext);

  if (!dependenciesCreatedDuringReduction.isUninitialized()) {
    Power::AddPowerToListOfDependenciesIfNeeded(childAtIndex(i), static_cast<Power &>(p), dependenciesCreatedDuringReduction, reductionContext, true);
    Power::AddPowerToListOfDependenciesIfNeeded(e, static_cast<Power &>(p), dependenciesCreatedDuringReduction, reductionContext, true);
  }

  // Step 3: Replace one of the child
  replaceChildAtIndexInPlace(i, p);
  p = p.shallowReduce(reductionContext);

  // Step 4: Merge dependencies introduced by the reduction of power if any
  if (p.type() == ExpressionNode::Type::Dependency) {
    if (!dependenciesCreatedDuringReduction.isUninitialized()) {
      int n = dependenciesCreatedDuringReduction.numberOfChildren();
      dependenciesCreatedDuringReduction.mergeChildrenAtIndexInPlace(p.childAtIndex(1), n);
    }
    replaceChildAtIndexInPlace(i, p.childAtIndex(0));
  }

  /* Step 5: Reducing the new power might have turned it into a multiplication,
   * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
   * node with this. */
  if (p.type() == ExpressionNode::Type::Multiplication) {
    mergeChildrenAtIndexInPlace(p, i);
  }
}

bool Multiplication::factorizeExponent(int i, int j, const ExpressionNode::ReductionContext& reductionContext) {
  /* This function factorizes children which share a common exponent. For
   * example, it turns Multiplication::Builder(2^x,3^x) into Multiplication::Builder(6^x). */

  Expression thisAfterFactorization;
  {
    /* Multiplication can raise an exception is Base(child(i)) * Base(child(j))
     * is too big. If it's the case, do not factorize exponents. */
    char * treePoolCursor = TreePool::sharedPool()->cursor();
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Multiplication cloneOfThis = clone().convert<Multiplication>();
      // Step 1: Find the new base
      Expression m = Multiplication::Builder(Base(cloneOfThis.childAtIndex(i)), Base(cloneOfThis.childAtIndex(j))); // 2^x*3^x -> (2*3)^x -> 6^x
      // Step 2: Get rid of one of the children
      cloneOfThis.removeChildAtIndexInPlace(j);
      // Step 3: Replace the other child
      cloneOfThis.childAtIndex(i).replaceChildAtIndexInPlace(0, m);
      // Step 4: Reduce expressions
      m.shallowReduce(reductionContext);
      Expression p = cloneOfThis.childAtIndex(i).shallowReduce(reductionContext); // 2^x*(1/2)^x -> (2*1/2)^x -> 1
      /* Step 5: Reducing the new power might have turned it into a multiplication,
       * ie: 12^(1/2) -> 2*3^(1/2). In that case, we need to merge the multiplication
       * node with this. */
      if (p.type() == ExpressionNode::Type::Multiplication) {
        cloneOfThis.mergeChildrenAtIndexInPlace(p, i);
      }
      thisAfterFactorization = cloneOfThis;
    } else {
      return false;
    }
  }
  assert(thisAfterFactorization.type() == ExpressionNode::Type::Multiplication);
  replaceWithInPlace(thisAfterFactorization);
  *this = static_cast<Multiplication&>(thisAfterFactorization);
  return true;
}

Expression Multiplication::gatherLikeTerms(const ExpressionNode::ReductionContext & reductionContext) {
  /* Gather like terms. For example, turn pi^2*pi^3 into pi^5. Thanks to
   * the simplification order, such terms are guaranteed to be next to each
   * other. */
  int i = 0;
  bool gatheredTerms = false;
  List dependencies = List::Builder();
  while (i < numberOfChildren()-1) {
    Expression oi = childAtIndex(i);
    Expression oi1 = childAtIndex(i+1);
    if (oi.recursivelyMatches(Expression::IsRandom, reductionContext.context())) {
      // Do not factorize random or randint
    } else if (TermsHaveIdenticalBase(oi, oi1)
        && (!TermHasNumeralBase(oi)
          || (oi.type() == ExpressionNode::Type::Power && oi1.type() == ExpressionNode::Type::Power))) {
      /* The previous condition exists because combining powers
       * of a given rational isn't straightforward. Indeed,
       * there are two cases we want to deal with:
       *  - 2*2^(1/2) or 2*2^pi, we want to keep as-is
       *  - 2^(1/2)*2^(3/2) we want to combine. */
      factorizeBase(i, i+1, reductionContext, dependencies);
      /* An undef term could have appeared when factorizing 1^inf and 1^-inf
       * for instance. In that case, we escape and return undef. */
      if (childAtIndex(i).isUndefined()) {
        return replaceWithUndefinedInPlace();
      }
      gatheredTerms = true;
      continue;
    } else if (TermHasNumeralBase(oi) && TermHasNumeralBase(oi1) && TermsHaveIdenticalExponent(oi, oi1)) {
      bool managedToFactorize = factorizeExponent(i, i+1, reductionContext);
      if (managedToFactorize) {
        gatheredTerms = true;
        continue;
      }
    } else if (TermIsPowerOfRationals(oi) && TermIsPowerOfRationals(oi1)
            && !(oi.childAtIndex(1).convert<Rational>().isInteger() || oi1.childAtIndex(1).convert<Rational>().isInteger()))
    {
      if (gatherRationalPowers(i, i+1, reductionContext)) {
        gatheredTerms = true;
        continue;
      }
    }
    i++;
  }

  if (dependencies.numberOfChildren() > 0) {
    Dependency dep = Dependency::Builder(Undefined::Builder(), dependencies);
    replaceWithInPlace(dep);
    dep.replaceChildAtIndexInPlace(0, *this);
    shallowReduce(reductionContext);
    return dep.shallowReduce(reductionContext);
  }

  if (gatheredTerms) {
    // Sort the children again in case gatherLikeTerms messed with the order
    sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, true); }, reductionContext.context(), reductionContext.shouldCheckMatrices());
  }

  return Expression();
}

bool Multiplication::gatherRationalPowers(int i, int j, const ExpressionNode::ReductionContext& reductionContext) {
  /* Turn x^(a/b)*y^(p/q) into (x^(ak/b)*y^(pk/q))^(1/k) where k = lcm(b,q)
   * This effectively gathers all roots into a single root.
   * Returns true if operation was successful. */
  assert(TermIsPowerOfRationals(childAtIndex(i)) && TermIsPowerOfRationals(childAtIndex(j)));

  Rational x = childAtIndex(i).childAtIndex(0).convert<Rational>();
  Rational y = childAtIndex(j).childAtIndex(0).convert<Rational>();
  Rational ab = childAtIndex(i).childAtIndex(1).convert<Rational>();
  Rational pq = childAtIndex(j).childAtIndex(1).convert<Rational>();
  Integer a = ab.signedIntegerNumerator(), b = ab.integerDenominator(), p = pq.signedIntegerNumerator(), q = pq.integerDenominator();
  Integer k = Arithmetic::LCM(b, q);
  IntegerDivision divB = Integer::Division(k, b), divQ = Integer::Division(k, q);
  assert(divB.remainder.isZero() && divQ.remainder.isZero());
  Rational m = Rational::Multiplication(
      Rational::IntegerPower(x, Integer::Multiplication(a, divB.quotient)),
      Rational::IntegerPower(y, Integer::Multiplication(p, divQ.quotient))
      );
  if (m.numeratorOrDenominatorIsInfinity() || k.isOverflow()) {
    // Escape to prevent the introduction of overflown rationals
    return false;
  }
  Integer one(1);
  Expression result = Power::Builder(m, Rational::Builder(one, k));

  removeChildAtIndexInPlace(j);
  replaceChildAtIndexInPlace(i, result);
  Expression child = childAtIndex(i).shallowReduce(reductionContext);
  if (child.type() == ExpressionNode::Type::Multiplication) {
    mergeChildrenAtIndexInPlace(child, i);
  }
  return true;
}

Expression Multiplication::distributeOnOperandAtIndex(int i, const ExpressionNode::ReductionContext& reductionContext) {
  /* This method creates a*...*b*y... + a*...*c*y... + ... from
   * a*...*(b+c+...)*y... */
  assert(i >= 0 && i < numberOfChildren());
  assert(childAtIndex(i).type() == ExpressionNode::Type::Addition);

  Addition a = Addition::Builder();
  Expression childI = childAtIndex(i);
  int numberOfAdditionTerms = childI.numberOfChildren();
  for (int j = 0; j < numberOfAdditionTerms; j++) {
    Multiplication m = clone().convert<Multiplication>();
    m.replaceChildAtIndexInPlace(i, childI.childAtIndex(j));
    // Reduce m: pi^(-1)*(pi + x) -> pi^(-1)*pi + pi^(-1)*x -> 1 + pi^(-1)*x
    a.addChildAtIndexInPlace(m, a.numberOfChildren(), a.numberOfChildren());
    m.shallowReduce(reductionContext);
  }
  replaceWithInPlace(a);
  return a.shallowReduce(reductionContext); // Order terms, put under a common denominator if needed
}

void Multiplication::addMissingFactors(Expression factor, const ExpressionNode::ReductionContext& reductionContext) {
  if (factor.type() == ExpressionNode::Type::Multiplication) {
    int childrenNumber = factor.numberOfChildren();
    /* WARNING: This is wrong in general case.
     * LCM(x, a*b*c) != LCM(LCM(LCM(x,a),b),c)
     * It relies on the fact that factor is reduced, so it cannot
     * be a multiplication containing terms with common factors.
     * Example:
     * LCM(10,2*4*6) = 240.
     * With this method we would have LCM(10, 2*4*6) = 60 which is false.
     * But since 2*4*6 is 48 in reduced form, we would compute LCM(10,48) = 240
     * which is true.
     */
    for (int j = 0; j < childrenNumber; j++) {
      addMissingFactors(factor.childAtIndex(j), reductionContext);
    }
    return;
  }
  /* Special case when factor is a Rational: if 'this' has already a rational
   * child, we replace it by its LCM with factor ; otherwise, we simply add
   * factor as a child. */
  if (numberOfChildren() > 0 && childAtIndex(0).type() == ExpressionNode::Type::Rational && factor.type() == ExpressionNode::Type::Rational) {
    assert(static_cast<Rational &>(factor).isInteger());
    assert(childAtIndex(0).convert<Rational>().isInteger());
    Integer lcm = Arithmetic::LCM(static_cast<Rational &>(factor).unsignedIntegerNumerator(), childAtIndex(0).convert<Rational>().unsignedIntegerNumerator());
    if (lcm.isOverflow()) {
      addChildAtIndexInPlace(Rational::Builder(static_cast<Rational &>(factor).unsignedIntegerNumerator()), 1, numberOfChildren());
      return;
    }
    replaceChildAtIndexInPlace(0, Rational::Builder(lcm));
    return;
  }
  if (factor.type() != ExpressionNode::Type::Rational) {
    /* If factor is not a rational, we merge it with the child of identical
     * base if any. Otherwise, we add it as an new child. */
    for (int i = 0; i < numberOfChildren(); i++) {
      if (TermsHaveIdenticalBase(childAtIndex(i), factor)) {
        Expression sub = Subtraction::Builder(CreateExponent(childAtIndex(i)), CreateExponent(factor)).deepReduce(reductionContext);
        if (sub.isPositive(reductionContext.context()) == TrinaryBoolean::False) { // index[0] < index[1]
          sub = Opposite::Builder(sub);
          if (factor.type() == ExpressionNode::Type::Power) {
            factor.replaceChildAtIndexInPlace(1, sub);
          } else {
            factor = Power::Builder(factor, sub);
          }
          sub.shallowReduce(reductionContext);
          mergeInChildByFactorizingBase(i, factor, reductionContext);
        } else if (sub.isPositive(reductionContext.context()) == TrinaryBoolean::Unknown) {
          mergeInChildByFactorizingBase(i, factor, reductionContext);
        }
        return;
      }
    }
  }
  addChildAtIndexInPlace(factor.clone(), 0, numberOfChildren());
  sortChildrenInPlace([](const ExpressionNode * e1, const ExpressionNode * e2) { return ExpressionNode::SimplificationOrder(e1, e2, true); }, reductionContext.context(), reductionContext.shouldCheckMatrices());
}

bool Multiplication::factorizeSineAndCosine(int i, int j, const ExpressionNode::ReductionContext& reductionContext) {
  /* This function turn sin(x)^p * cos(x)^q into either:
   * - tan(x)^p*cos(x)^(p+q) if |p|<|q|
   * - tan(x)^(-q)*sin(x)^(p+q) otherwise */
  const Expression x = Base(childAtIndex(i)).childAtIndex(0);
  // We check before that p and q were numbers
  Number p = CreateExponent(childAtIndex(i)).convert<Number>();
  Number q = CreateExponent(childAtIndex(j)).convert<Number>();
  // If p and q have the same sign, we cannot replace them by a tangent
  TrinaryBoolean pIsPositive = p.isPositive();
  if (pIsPositive != TrinaryBoolean::Unknown && pIsPositive == q.isPositive()) {
    return false;
  }
  Number sumPQ = Number::Addition(p, q);
  Number absP = p.clone().convert<Number>().setSign(true);
  Number absQ = q.clone().convert<Number>().setSign(true);
  Expression tan = Tangent::Builder(x.clone());

  // First case: replace sin(x)^p by tan(x)^p
  Number tanPower = p;
  int tanIndex = i;
  int otherIndex = j;
  // Second case: Replace cos(x)^q by tan(x)^(-q)
  if (Number::NaturalOrder(absP, absQ) >= 0) {
    tanPower = Number::Multiplication(q, Rational::Builder(-1));
    tanIndex = j;
    otherIndex = i;
  }

  /* If the power of tan is negative and tan(x) = undef, we can't transform
   * cos/sin into 1/tan. Indeed, cos(pi/2)/sin(pi/2) is defined, but tan(pi/2)
   * is undef. */
  if (tanPower.isPositive() == TrinaryBoolean::False && tan.approximate<float>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit(), true).isUndefined()) {
    return false;
  }
  replaceChildAtIndexInPlace(tanIndex, Power::Builder(tan, tanPower));
  childAtIndex(tanIndex).shallowReduce(reductionContext);
  // Replace cos(x)^q by cos(x)^(p+q) or sin(x)^p by sin(x)^(p+q)
  if (Expression::IsZero(sumPQ)) {
    /* We have to do this because x^0 != 1 because 0^0 is undef
     * so sin(x)^0 creates a dependency. */
    replaceChildAtIndexInPlace(otherIndex, Rational::Builder(1));
    return true;
  }
  replaceChildAtIndexInPlace(otherIndex, Power::Builder(Base(childAtIndex(otherIndex)), sumPQ));
  childAtIndex(otherIndex).shallowReduce(reductionContext);
  return true;
}

bool Multiplication::HaveSameNonNumeralFactors(const Expression & e1, const Expression & e2) {
  assert(e1.numberOfChildren() > 0);
  assert(e2.numberOfChildren() > 0);
  int numberOfNonNumeralFactors1 = e1.childAtIndex(0).isNumber() ? e1.numberOfChildren()-1 : e1.numberOfChildren();
  int numberOfNonNumeralFactors2 = e2.childAtIndex(0).isNumber() ? e2.numberOfChildren()-1 : e2.numberOfChildren();
  if (numberOfNonNumeralFactors1 != numberOfNonNumeralFactors2) {
    return false;
  }
  int firstNonNumeralOperand1 = e1.childAtIndex(0).isNumber() ? 1 : 0;
  int firstNonNumeralOperand2 = e2.childAtIndex(0).isNumber() ? 1 : 0;
  for (int i = 0; i < numberOfNonNumeralFactors1; i++) {
    Expression currentChild1 = e1.childAtIndex(firstNonNumeralOperand1+i);
    if (currentChild1.isRandom()
        || !(currentChild1.isIdenticalTo(e2.childAtIndex(firstNonNumeralOperand2+i))))
    {
      return false;
    }
  }
  return true;
}

const Expression Multiplication::CreateExponent(Expression e) {
  Expression result = e.type() == ExpressionNode::Type::Power ? e.childAtIndex(1).clone() : Rational::Builder(1);
  return result;
}

bool Multiplication::TermsHaveIdenticalBase(const Expression & e1, const Expression & e2) {
  return Base(e1).isIdenticalTo(Base(e2));
}

bool Multiplication::TermsHaveIdenticalExponent(const Expression & e1, const Expression & e2) {
  /* Note: We will return false for e1=2 and e2=Pi, even though one could argue
   * that these have the same exponent whose value is 1. */
  return e1.type() == ExpressionNode::Type::Power && e2.type() == ExpressionNode::Type::Power && (e1.childAtIndex(1).isIdenticalTo(e2.childAtIndex(1)));
}

bool Multiplication::TermHasNumeralBase(const Expression & e) {
  return Base(e).isNumber();
}

bool Multiplication::TermHasNumeralExponent(const Expression & e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return true;
  }
  if (e.childAtIndex(1).isNumber()) {
    return true;
  }
  return false;
}

bool Multiplication::TermIsPowerOfRationals(const Expression & e) {
  if (e.type() != ExpressionNode::Type::Power) {
    return false;
  }
  assert(e.numberOfChildren() == 2);
  return e.childAtIndex(0).type() == ExpressionNode::Type::Rational && e.childAtIndex(1).type() == ExpressionNode::Type::Rational;
}

void Multiplication::splitIntoNormalForm(Expression & numerator, Expression & denominator, const ExpressionNode::ReductionContext& reductionContext) const {
  Multiplication mNumerator = Multiplication::Builder();
  Multiplication mDenominator = Multiplication::Builder();
  int numberOfFactorsInNumerator = 0;
  int numberOfFactorsInDenominator = 0;
  const int numberOfFactors = numberOfChildren();
  for (int i = 0; i < numberOfFactors; i++) {
    Expression factor = childAtIndex(i).clone();
    ExpressionNode::Type factorType = factor.type();
    Expression factorsNumerator;
    Expression factorsDenominator;
    if (factorType == ExpressionNode::Type::Rational) {
      Rational r = static_cast<Rational &>(factor);
      if (r.isRationalOne()) {
        // Special case: add a unary numeral factor if r = 1
        factorsNumerator = r;
      } else {
        Integer rNum = r.signedIntegerNumerator();
        if (!rNum.isOne()) {
          factorsNumerator = Rational::Builder(rNum);
        }
        Integer rDen = r.integerDenominator();
        if (!rDen.isOne()) {
          factorsDenominator = Rational::Builder(rDen);
        }
      }
    } else if (factorType == ExpressionNode::Type::Power) {
      Expression fd = factor.denominator(reductionContext);
      if (fd.isUninitialized()) {
        factorsNumerator = factor;
      } else {
        factorsDenominator = fd;
      }
    } else {
      factorsNumerator = factor;
    }
    if (!factorsNumerator.isUninitialized()) {
      mNumerator.addChildAtIndexInPlace(factorsNumerator, numberOfFactorsInNumerator, numberOfFactorsInNumerator);
      numberOfFactorsInNumerator++;
    }
    if (!factorsDenominator.isUninitialized()) {
      mDenominator.addChildAtIndexInPlace(factorsDenominator, numberOfFactorsInDenominator, numberOfFactorsInDenominator);
      numberOfFactorsInDenominator++;
    }
  }
  if (numberOfFactorsInNumerator) {
    numerator = mNumerator.squashUnaryHierarchyInPlace();
  }
  if (numberOfFactorsInDenominator) {
    denominator = mDenominator.squashUnaryHierarchyInPlace();
  }
}

const Expression Multiplication::Base(const Expression e) {
  if (e.type() == ExpressionNode::Type::Power) {
    return e.childAtIndex(0);
  }
  return e;
}

template MatrixComplex<float> MultiplicationNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>, Preferences::ComplexFormat);
template MatrixComplex<double> MultiplicationNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>, Preferences::ComplexFormat);

template Complex<float> MultiplicationNode::computeOnComplex<float>(const std::complex<float>, const std::complex<float>, Preferences::ComplexFormat);
template Complex<double> MultiplicationNode::computeOnComplex<double>(const std::complex<double>, const std::complex<double>, Preferences::ComplexFormat);

template void Multiplication::computeOnArrays<double>(double * m, double * n, double * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);

}
