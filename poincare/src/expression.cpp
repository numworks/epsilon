#include <poincare/addition.h>
#include <poincare/based_integer.h>
#include <poincare/code_point_layout.h>
#include <poincare/complex_cartesian.h>
#include <poincare/constant.h>
#include <poincare/decimal.h>
#include <poincare/dependency.h>
#include <poincare/derivative.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/expression.h>
#include <poincare/expression_node.h>
#include <poincare/float.h>
#include <poincare/ghost.h>
#include <poincare/imaginary_part.h>
#include <poincare/list.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/piecewise_operator.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/real_part.h>
#include <poincare/solver.h>
#include <poincare/store.h>
#include <poincare/string_layout.h>
#include <poincare/string_layout.h>
#include <poincare/subtraction.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <poincare/variable_context.h>
#include "parsing/parser.h"
#include <ion.h>
#include <ion/unicode/utf8_helper.h>
#include <cmath>
#include <float.h>
#include <utility>

namespace Poincare {

static bool s_approximationEncounteredComplex = false;
static bool s_reductionEncounteredUndistributedList = false;

/* Constructor & Destructor */

Expression Expression::clone() const { TreeHandle c = TreeHandle::clone(); return static_cast<Expression&>(c); }

Expression Expression::Parse(char const * string, Context * context, bool addParentheses, bool parseForAssignment) {
  if (string[0] == 0) {
    return Expression();
  }
  Parser p(string, context, nullptr, parseForAssignment ? ParsingContext::ParsingMethod::Assignment : ParsingContext::ParsingMethod::Classic);
  Expression expression = p.parse();
  if (p.getStatus() != Parser::Status::Success) {
    expression = Expression();
  }
  if (!expression.isUninitialized() && addParentheses) {
    expression = expression.addMissingParentheses();
  }
  return expression;
}

Expression Expression::ExpressionFromAddress(const void * address, size_t size) {
  if (address == nullptr || size == 0) {
    return Expression();
  }
  // Build the Expression in the Tree Pool
  return Expression(static_cast<ExpressionNode *>(TreePool::sharedPool->copyTreeFromAddress(address, size)));
}

/* Hierarchy */

Expression Expression::childAtIndex(int i) const {
  assert(i >= 0 && i < numberOfChildren());
  TreeHandle c = TreeHandle::childAtIndex(i);
  return static_cast<Expression &>(c);
}

/* Properties */

bool Expression::isRationalOne() const {
  return type() == ExpressionNode::Type::Rational && convert<const Rational>().isOne();
}

bool Expression::isOne() const {
  return isRationalOne() || (type() == ExpressionNode::Type::BasedInteger && convert<const BasedInteger>().integer().isOne());
}

bool Expression::isMinusOne() const {
  return (type() == ExpressionNode::Type::BasedInteger && convert<const BasedInteger>().integer().isMinusOne())
      || (type() == ExpressionNode::Type::Opposite && childAtIndex(0).isOne())
      || (type() == ExpressionNode::Type::Rational && convert<const Rational>().isMinusOne());
}

bool Expression::isInteger() const {
  return (type() == ExpressionNode::Type::BasedInteger)
      || (type() == ExpressionNode::Type::Opposite && childAtIndex(0).isInteger())
      || (type() == ExpressionNode::Type::Rational && convert<const Rational>().isInteger());
}

bool Expression::recursivelyMatches(ExpressionTrinaryTest test, Context * context, SymbolicComputation replaceSymbols, void * auxiliary) const {
  if (!context) {
    replaceSymbols = SymbolicComputation::DoNotReplaceAnySymbol;
  }
  TrinaryBoolean testResult = test(*this, context, auxiliary);
  if (testResult == TrinaryBoolean::True) {
    return true;
  } else if (testResult == TrinaryBoolean::False) {
    return false;
  }
  assert(testResult == TrinaryBoolean::Unknown && !isUninitialized());

  // Handle dependencies, symbols and functions
  ExpressionNode::Type t = type();
  if (t == ExpressionNode::Type::Dependency) {
    Expression e = *this;
    return static_cast<Dependency &>(e).dependencyRecursivelyMatches(test, context, replaceSymbols, auxiliary);
  }
  if (t == ExpressionNode::Type::Symbol || t == ExpressionNode::Type::Function) {
    assert(replaceSymbols == SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
        || replaceSymbols == SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
        || replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol); // We need only those cases for now

    if (replaceSymbols == SymbolicComputation::DoNotReplaceAnySymbol
        || (replaceSymbols == SymbolicComputation::ReplaceDefinedFunctionsWithDefinitions
          && t == ExpressionNode::Type::Symbol))
    {
      return false;
    }
    assert(replaceSymbols == SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition
        || t == ExpressionNode::Type::Function);
      return SymbolAbstract::matches(convert<const SymbolAbstract>(), test, context, auxiliary);
  }

  const int childrenCount = this->numberOfChildren();
  // Run loop backwards to find lists and matrices quicker in NAry expressions
  for (int i = childrenCount - 1; i >= 0; i--) {
    if (childAtIndex(i).recursivelyMatches(test, context, replaceSymbols, auxiliary)) {
      return true;
    }
  }
  return false;
}

bool Expression::recursivelyMatches(ExpressionTest test, Context * context, SymbolicComputation replaceSymbols) const {
  ExpressionTrinaryTest ternary = [](const Expression e, Context * context, void * auxiliary) {
    ExpressionTest * trueTest = static_cast<ExpressionTest *>(auxiliary);
    return (*trueTest)(e, context) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
  };
  return recursivelyMatches(ternary, context, replaceSymbols, &test);
}

bool Expression::recursivelyMatches(ExpressionTestAuxiliary test, Context * context, SymbolicComputation replaceSymbols, void * auxiliary) const {
  struct Pack {
    ExpressionTestAuxiliary * test;
    void * auxiliary;
  };
  ExpressionTrinaryTest ternary = [](const Expression e, Context * context, void * pack) {
    ExpressionTestAuxiliary * trueTest = static_cast<ExpressionTestAuxiliary *>(static_cast<Pack *>(pack)->test);
    return (*trueTest)(e, context, static_cast<Pack *>(pack)->auxiliary) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
  };
  struct Pack pack { &test, auxiliary };
  return recursivelyMatches(ternary, context, replaceSymbols, &pack);
}

bool Expression::deepIsMatrix(Context * context, bool canContainMatrices, bool isReduced) const {
  if (!canContainMatrices) {
    return false;
  }
  return recursivelyMatches([](const Expression e, Context * context, void * auxiliary) {
        if (IsMatrix(e, context)) {
          return TrinaryBoolean::True;
        }
        if (IsNAry(e, context) && e.numberOfChildren() > 0) {
          bool * isReduced = static_cast<bool *>(auxiliary);
          /* If reduction didn't fail, the children were sorted so any
           * expression which is a matrix (deeply) would be at the end.
           * Otherwise, it could be at any index. */
          int firstChildToCheck = e.numberOfChildren() - 1;
          int lastChildToCheck = *isReduced ? firstChildToCheck : 0;
          for (int i = firstChildToCheck; i >= lastChildToCheck; i--) {
            if (e.childAtIndex(i).deepIsMatrix(context)) {
              return TrinaryBoolean::True;
            }
          }
        }
        // Dependency are matrices only if their first child is a matrix
        if (e.type() == ExpressionNode::Type::Dependency) {
           return e.childAtIndex(0).deepIsMatrix(context) ? TrinaryBoolean::True : TrinaryBoolean::False;
        }
        // These types are matrices if one of their children is one
        if (e.isOfType({ExpressionNode::Type::Power, ExpressionNode::Type::Opposite, ExpressionNode::Type::Sum, ExpressionNode::Type::Product})) {
          return TrinaryBoolean::Unknown;
        }
        // Any other type is not a matrix
        return TrinaryBoolean::False;
      },
      context,
      SymbolicComputation::ReplaceAllDefinedSymbolsWithDefinition,
      &isReduced);
 }

bool Expression::deepIsList(Context * context) const {
  return recursivelyMatches([](const Expression e, Context * context, void *) {
      switch (e.type()) {
      /* These expressions are always lists. */
      case ExpressionNode::Type::List:
      case ExpressionNode::Type::ListElement:
      case ExpressionNode::Type::ListSlice:
      case ExpressionNode::Type::ListSort:
      case ExpressionNode::Type::RandintNoRepeat:
        return TrinaryBoolean::True;

      /* These expressions have a list as argument but are never lists, we
       * must stop the search. */
      case ExpressionNode::Type::Dimension:
      case ExpressionNode::Type::ListMaximum:
      case ExpressionNode::Type::ListMean:
      case ExpressionNode::Type::ListMedian:
      case ExpressionNode::Type::ListMinimum:
      case ExpressionNode::Type::ListProduct:
      case ExpressionNode::Type::ListSampleStandardDeviation:
      case ExpressionNode::Type::ListStandardDeviation:
      case ExpressionNode::Type::ListSum:
      case ExpressionNode::Type::ListVariance:
        return TrinaryBoolean::False;

      /* Other expressions may be lists if their children are lists. */
      default:
        return TrinaryBoolean::Unknown;
      }
  }, context);
}

bool Expression::IsApproximate(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Decimal || e.type() == ExpressionNode::Type::Float || e.type() == ExpressionNode::Type::Double;
}

bool Expression::IsRandom(const Expression e, Context * context) {
  return e.isRandom();
}

bool Expression::IsNAry(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Addition || e.type() == ExpressionNode::Type::Multiplication;
}

bool Expression::IsMatrix(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Matrix
    /* A Dimension is a matrix unless its child is a list. */
    || (e.type() == ExpressionNode::Type::Dimension && !e.childAtIndex(0).deepIsList(context))
    || e.type() == ExpressionNode::Type::MatrixInverse
    || e.type() == ExpressionNode::Type::MatrixIdentity
    || e.type() == ExpressionNode::Type::MatrixTranspose
    || e.type() == ExpressionNode::Type::MatrixRowEchelonForm
    || e.type() == ExpressionNode::Type::MatrixReducedRowEchelonForm
    || e.type() == ExpressionNode::Type::VectorCross;
}

bool Expression::IsInfinity(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::Infinity;
}

bool Expression::IsPercent(const Expression e, Context * context) {
  return e.type() == ExpressionNode::Type::PercentSimple || e.type() == ExpressionNode::Type::PercentAddition;
}

bool Expression::IsDiscontinuous(const Expression e, Context * context) {
  return e.isRandom()
      || e.type() == ExpressionNode::Type::PiecewiseOperator
      || ((e.type() == ExpressionNode::Type::Floor
          || e.type() == ExpressionNode::Type::Round
          || e.type() == ExpressionNode::Type::Ceiling
          || e.type() == ExpressionNode::Type::FracPart
          || e.type() == ExpressionNode::Type::AbsoluteValue)
        && e.recursivelyMatches([](const Expression e, Context * context) {
            return e.type() == ExpressionNode::Type::Symbol;
          }, context));
}

bool Expression::DeepIsSymbolic(const Expression e, Context * context, SymbolicComputation replaceSymbols) {
  return e.recursivelyMatches(
    [] (const Expression e, Context * context, void * auxiliary) {
      if (e.isParameteredExpression()) {
        // Do not check IsSymbolic for parametered child.
        bool n = e.numberOfChildren();
        SymbolicComputation * replaceSymbols = reinterpret_cast<SymbolicComputation *>(auxiliary);
        for (int i = 0; i < n; i++) {
          if (i == ParameteredExpression::ParameterChildIndex()) {
            continue;
          }
          Expression childToAnalyze = e.childAtIndex(i);
          if (i == ParameteredExpression::ParameteredChildIndex()) {
            // Replace the parameter with '0' to analyze the child ignoring it
            childToAnalyze = e.childAtIndex(i).clone();
            Expression parameter = e.childAtIndex(ParameteredExpression::ParameterChildIndex());
            assert(parameter.type() == ExpressionNode::Type::Symbol);
            childToAnalyze = childToAnalyze.replaceSymbolWithExpression(static_cast<Symbol&>(parameter), Rational::Builder(0));
          }
          if (DeepIsSymbolic(childToAnalyze, context, *replaceSymbols)) {
            return TrinaryBoolean::True;
          }
        }
        return TrinaryBoolean::False;
      }
      return IsSymbolic(e, context) ? TrinaryBoolean::True : TrinaryBoolean::Unknown;
    },
    context,
    replaceSymbols,
    reinterpret_cast<void *>(&replaceSymbols)
  );
}

bool Expression::IsSymbolic(const Expression e, Context * context) {
  return e.isOfType({ ExpressionNode::Type::Symbol, ExpressionNode::Type::Function, ExpressionNode::Type::Sequence });
}

bool Expression::IsRationalFraction(const Expression& e, Context * context, const char * symbol) {
  if (e.type() != ExpressionNode::Type::Multiplication && e.type() != ExpressionNode::Type::Power) {
    return false;
  }

  ReductionContext reductionContext = ReductionContext::DefaultReductionContextForAnalysis(context);

  Expression numerator, denominator;

  if (e.type() == ExpressionNode::Type::Power) {
    denominator = e.denominator(reductionContext);
    if (denominator.isUninitialized()) {
      numerator = e;
    }
  } else {
    assert(e.type() == ExpressionNode::Type::Multiplication);
    static_cast<const Multiplication&>(e).splitIntoNormalForm(numerator, denominator, reductionContext);
  }

  int numeratorDegree = numerator.isUninitialized() ? 0 : numerator.polynomialDegree(context, symbol);
  int denominatorDegree = denominator.isUninitialized() ? 0 : denominator.polynomialDegree(context, symbol);
  return denominatorDegree >= 0 && numeratorDegree >= 0;
}

bool Expression::isLinearCombinationOfFunction(Context * context, PatternTest testFunction, const char * symbol) const {
  if (testFunction(*this, context, symbol) || polynomialDegree(context, symbol) == 0) {
    return true;
  }
  if (type() == ExpressionNode::Type::Addition) {
    int n = numberOfChildren();
    assert(n > 0);
    for (int i = 0; i < n; i++) {
      if (!childAtIndex(i).isLinearCombinationOfFunction(context, testFunction, symbol)) {
        return false;
      }
    }
    return true;
  }
  if (type() == ExpressionNode::Type::Multiplication) {
    int n = numberOfChildren();
    assert(n > 0);
    bool patternHasAlreadyBeenDetected = false;
    for (int i = 0; i < n; i++) {
      Expression currentChild = childAtIndex(i);
      bool childIsConstant = currentChild.polynomialDegree(context, symbol) == 0;
      bool childIsPattern = currentChild.isLinearCombinationOfFunction(context, testFunction, symbol);
      if (!childIsConstant && (!childIsPattern || patternHasAlreadyBeenDetected)) {
        /* The coefficient must have a degree 0 if it's not the pattern
         * and there can't be a multiplication of the pattern with itself */
        return false;
      }
      patternHasAlreadyBeenDetected = patternHasAlreadyBeenDetected || (childIsPattern && !childIsConstant);
    }
    return patternHasAlreadyBeenDetected;
  }
  return false;
}

bool containsVariables(const Expression e, char * variables, int maxVariableSize) {
  if (e.type() == ExpressionNode::Type::Symbol) {
    int index = 0;
    while (variables[index*maxVariableSize] != 0) {
      if (strcmp(static_cast<const Symbol&>(e).name(), &variables[index*maxVariableSize]) == 0) {
        return true;
      }
      index++;
    }
  }
  const int childrenCount = e.numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (containsVariables(e.childAtIndex(i), variables, maxVariableSize)) {
      return true;
    }
  }
  return false;
}

bool Expression::getLinearCoefficients(char * variables, int maxVariableSize, Expression coefficients[], Expression * constant, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation) const {
  assert(!recursivelyMatches(IsMatrix, context, symbolicComputation));
  // variables is in fact of type char[k_maxNumberOfVariables][maxVariableSize]
  int numberOfVariables = 0;
  while (numberOfVariables < k_maxNumberOfVariables && variables[numberOfVariables*maxVariableSize] != 0) {
    int degree = polynomialDegree(context, &variables[numberOfVariables*maxVariableSize]);
    if (degree > 1 || degree < 0) {
      return false;
    }
    numberOfVariables++;
  }
  Expression equation = *this;
  Expression polynomialCoefficients[k_maxNumberOfPolynomialCoefficients];
  for (int index = 0; index < numberOfVariables; index++) {
    assert(variables[index*maxVariableSize] != 0);
    int degree = equation.getPolynomialReducedCoefficients(&variables[index*maxVariableSize], polynomialCoefficients, context, complexFormat, angleUnit, unitFormat, symbolicComputation);
    switch (degree) {
      case 0:
        coefficients[index] = Rational::Builder(0);
        break;
      case 1:
        coefficients[index] = polynomialCoefficients[1];
        break;
      default:
        /* degree is supposed to be 0 or 1. Otherwise, it means that equation
         * is 'undefined' due to the reduction of 0*inf for example.
         * (ie, x*y*inf = 0) */
        assert(!recursivelyMatches([](const Expression e, Context * context) { return e.isUndefined(); }, context));
        return false;
    }
    /* The equation is can be written: a_1*x+a_0 with a_1 and a_0 x-independent.
     * The equation supposed to be linear in all variables, so we can look for
     * the coefficients linked to the other variables in a_0. */
    equation = polynomialCoefficients[0];
  }
  *constant = Opposite::Builder(equation).cloneAndReduce(ReductionContext(context, complexFormat, angleUnit, unitFormat, ReductionTarget::SystemForApproximation, symbolicComputation));
  /* The expression can be linear on all coefficients taken one by one but
   * non-linear (ex: xy = 2). We delete the results and return false if one of
   * the coefficients contains a variable. */
  bool isMultivariablePolynomial = containsVariables(*constant, variables, maxVariableSize);
  for (int i = 0; i < numberOfVariables; i++) {
    if (isMultivariablePolynomial) {
      break;
    }
    isMultivariablePolynomial = containsVariables(coefficients[i], variables, maxVariableSize);
  }
  return !isMultivariablePolynomial;
}

bool Expression::allChildrenAreUndefined() {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (!childAtIndex(i).isUndefined()) {
      return false;
    }
  }
  return true;
}

bool Expression::isBasedIntegerCappedBy(const char * stringInteger) const {
  return type() == ExpressionNode::Type::BasedInteger && (Integer::NaturalOrder(convert<BasedInteger>().integer(), Integer(stringInteger)) < 0);
}

bool Expression::isDivisionOfIntegers() const {
  return type() == ExpressionNode::Type::Division && childAtIndex(0).type() == ExpressionNode::Type::BasedInteger && childAtIndex(1).type() == ExpressionNode::Type::BasedInteger;
}

bool Expression::isAlternativeFormOfRationalNumber() const {
  return type() == ExpressionNode::Type::Rational
        || type() == ExpressionNode::Type::BasedInteger
        || type() == ExpressionNode::Type::Decimal
        || (type() == ExpressionNode::Type::Division && childAtIndex(0).isAlternativeFormOfRationalNumber() && childAtIndex(1).isAlternativeFormOfRationalNumber())
        || (type() == ExpressionNode::Type::Opposite && childAtIndex(0).isAlternativeFormOfRationalNumber());
}

bool Expression::hasDefinedComplexApproximation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, float * returnRealPart, float * returnImagPart) const {
  if (complexFormat == Preferences::ComplexFormat::Real) {
    return false;
  }
  /* We return true when both real and imaginary approximation are defined and
   * imaginary part is not null. */
  Evaluation<float> approximation = node()->approximate(float(), ApproximationContext(context, complexFormat, angleUnit));
  if (approximation.type() != EvaluationNode<float>::Type::Complex) {
    return false;
  }
  Complex<float> z = static_cast<Complex<float> &>(approximation);
  float b = z.imag();
  if (b == 0.0f || std::isinf(b) || std::isnan(b)) {
    return false;
  }
  float a = z.real();
  if (std::isinf(a) || std::isnan(a)) {
    return false;
  }
  if (returnRealPart) {
    *returnRealPart = a;
  }
  if (returnImagPart) {
    *returnImagPart = b;
  }
  return true;
}

bool Expression::involvesDiscontinuousFunction(Context * context) const {
  return recursivelyMatches(IsDiscontinuous, context);
}

bool Expression::isDiscontinuousBetweenValuesForSymbol(const char * symbol, float x1, float x2, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  if (isRandom()) {
    return true;
  }
  bool isDiscontinuous = false;
  if (type() == ExpressionNode::Type::Ceiling || type() == ExpressionNode::Type::Floor || type() == ExpressionNode::Type::Round) {
    // is discontinuous if it changes value
    isDiscontinuous = approximateWithValueForSymbol<float>(symbol, x1, context, complexFormat, angleUnit) != approximateWithValueForSymbol<float>(symbol, x2, context, complexFormat, angleUnit);
  } else if (type() == ExpressionNode::Type::FracPart) {
    // is discontinuous if the child changes int value
    isDiscontinuous = std::floor(childAtIndex(0).approximateWithValueForSymbol<float>(symbol, x1, context, complexFormat, angleUnit)) != std::floor(childAtIndex(0).approximateWithValueForSymbol<float>(symbol, x2, context, complexFormat, angleUnit));
  } else if (type() == ExpressionNode::Type::AbsoluteValue) {
    // is discontinuous if the child changes sign
    isDiscontinuous = (childAtIndex(0).approximateWithValueForSymbol<float>(symbol, x1, context, complexFormat, angleUnit) > 0.0) != (childAtIndex(0).approximateWithValueForSymbol<float>(symbol, x2, context, complexFormat, angleUnit) > 0.0);
  } else if (type() == ExpressionNode::Type::PiecewiseOperator) {
    PiecewiseOperator pieceWiseExpression = convert<PiecewiseOperator>();
    isDiscontinuous = pieceWiseExpression.indexOfFirstTrueConditionWithValueForSymbol(symbol, x1, context, complexFormat, angleUnit) != pieceWiseExpression.indexOfFirstTrueConditionWithValueForSymbol(symbol, x2, context, complexFormat, angleUnit);
  }
  if (isDiscontinuous) {
    return true;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndex(i).isDiscontinuousBetweenValuesForSymbol(symbol, x1, x2, context, complexFormat, angleUnit)) {
      return true;
    }
  }
  return false;
}

bool Expression::hasBooleanValue() const {
  return type() == ExpressionNode::Type::Boolean
         || type() == ExpressionNode::Type::Comparison
         || type() == ExpressionNode::Type::LogicalOperatorNot
         || type() == ExpressionNode::Type::BinaryLogicalOperator;
}

bool Expression::derivate(const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  return node()->derivate(reductionContext, symbol, symbolValue);
}

void Expression::derivateChildAtIndexInPlace(int index, const ReductionContext& reductionContext, Symbol symbol, Expression symbolValue) {
  if (!childAtIndex(index).derivate(reductionContext, symbol, symbolValue)) {
    replaceChildAtIndexInPlace(index, Derivative::Builder(childAtIndex(index), symbol.clone().convert<Symbol>(), symbolValue.clone()));
  }
}

// Private

void Expression::shallowAddMissingParenthesis() {
  if (isUninitialized()) {
    return;
  }
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression child = childAtIndex(i);
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      replaceChildAtIndexInPlace(i, Parenthesis::Builder(child));
    }
  }
}

Expression Expression::addMissingParentheses() {
  const int childrenCount = numberOfChildren();
  for (int i = 0; i < childrenCount; i++) {
    Expression child = childAtIndex(i).addMissingParentheses();
    if (node()->childAtIndexNeedsUserParentheses(child, i)) {
      child = Parenthesis::Builder(child);
    }
    replaceChildAtIndexInPlace(i, child);
  }
  return *this;
}

Expression Expression::shallowReduceUsingApproximation(const ReductionContext& reductionContext) {
  double approx = node()->approximate(double(), ApproximationContext(reductionContext, true)).toScalar();
  /* If approx is capped by the largest integer such as all smaller integers can
   * be exactly represented in IEEE754, approx is the exact result (no
   * precision were loss). */
  if (!std::isnan(approx) && std::fabs(approx) <= k_largestExactIEEE754Integer) {
    Expression result = Decimal::Builder(approx);
    replaceWithInPlace(result);
    result = result.shallowReduce(reductionContext);
    assert(result.type() == ExpressionNode::Type::Rational);
    return result;
  }
  return *this;
}

Expression Expression::parent() const {
  TreeHandle p = TreeHandle::parent();
  return static_cast<Expression &>(p);
}

Expression Expression::replaceWithUndefinedInPlace() {
  Expression result = Undefined::Builder();
  replaceWithInPlace(result);
  return result;
}

void Expression::defaultSetChildrenInPlace(Expression other) {
  const int childrenCount = numberOfChildren();
  assert(childrenCount == other.numberOfChildren());
  for (int i = 0; i < childrenCount; i++) {
    replaceChildAtIndexInPlace(i, other.childAtIndex(i));
  }
}

Expression Expression::defaultReplaceReplaceableSymbols(Context * context, TrinaryBoolean * isCircular, int parameteredAncestorsCount, SymbolicComputation symbolicComputation) {
  int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    childAtIndex(i).deepReplaceReplaceableSymbols(context, isCircular, parameteredAncestorsCount, symbolicComputation);
    if (*isCircular == TrinaryBoolean::True) {
      // the expression is circularly defined, escape
      return *this;
    }
  }
  return *this;
}

Expression Expression::makePositiveAnyNegativeNumeralFactor(const ReductionContext& reductionContext) {
  // The expression is a negative number
  if (isNumber() && isPositive(reductionContext.context()) == TrinaryBoolean::False) {
    return setSign(true, reductionContext);
  }
  // The expression is a multiplication whose numeral factor is negative
  if (type() == ExpressionNode::Type::Multiplication && numberOfChildren() > 0 && childAtIndex(0).isNumber() && childAtIndex(0).isPositive(reductionContext.context()) == TrinaryBoolean::False) {
    Multiplication m = convert<Multiplication>();
    if (m.childAtIndex(0).type() == ExpressionNode::Type::Rational && m.childAtIndex(0).convert<Rational>().isMinusOne()) {
      // The negative numeral factor is -1, we just remove it
      m.removeChildAtIndexInPlace(0);
      // The multiplication can have only one child after removing -1
      return m.squashUnaryHierarchyInPlace();
    } else {
      // Otherwise, we make it positive
      m.childAtIndex(0).setSign(true, reductionContext);
    }
    return std::move(m);
  }
  return Expression();
}

template<typename U>
Evaluation<U> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  s_approximationEncounteredComplex = false;
  Evaluation<U> e = node()->approximate(U(), ApproximationContext(context, complexFormat, angleUnit, withinReduce));
  if (complexFormat == Preferences::ComplexFormat::Real && s_approximationEncounteredComplex) {
    e = Complex<U>::Undefined();
  }
  return e;
}

Expression Expression::defaultReplaceSymbolWithExpression(const SymbolAbstract & symbol, const Expression expression) {
  /* In this case, replacing a symbol does not alter the number of children,
   * since no other operation (e.g. reduction) is applied. */
  const int nbChildren = numberOfChildren();
  for (int i = 0; i < nbChildren; i++) {
    assert(nbChildren == numberOfChildren());
    childAtIndex(i).replaceSymbolWithExpression(symbol, expression);
  }
  return *this;
}

int Expression::defaultGetPolynomialCoefficients(Context * context, const char * symbol, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbol);
  if (deg == 0) {
    coefficients[0] = clone();
    return 0;
  }
  return -1;
}

int Expression::getPolynomialReducedCoefficients(const char * symbolName, Expression coefficients[], Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation, bool keepDependencies) const {
  int degree = getPolynomialCoefficients(context, symbolName, coefficients);
  for (int i = 0; i <= degree; i++) {
    coefficients[i] = coefficients[i].cloneAndReduce(ReductionContext(context, complexFormat, angleUnit, unitFormat, ReductionTarget::SystemForApproximation, symbolicComputation));
    if (!keepDependencies && coefficients[i].type() == ExpressionNode::Type::Dependency) {
      coefficients[i] = coefficients[i].childAtIndex(0);
    }
  }
  return degree;
}

/* Units */

bool Expression::hasUnit() const {
  return recursivelyMatches([](const Expression e, Context * context) { return e.type() == ExpressionNode::Type::Unit; }, nullptr, SymbolicComputation::DoNotReplaceAnySymbol);
}

bool Expression::isInRadians(Context * context) const {
  Expression thisClone = clone();
  Expression units;
  ReductionContext reductionContext;
  reductionContext.setContext(context);
  reductionContext.setUnitConversion(UnitConversion::None);
  thisClone.reduceAndRemoveUnit(reductionContext, &units);
  return !units.isUninitialized() && units.type() == ExpressionNode::Type::Unit && units.convert<Unit>().representative() == &Unit::k_angleRepresentatives[Unit::k_radianRepresentativeIndex];
}

bool Expression::isPureAngleUnit() const {
  return !isUninitialized() && type() == ExpressionNode::Type::Unit && convert<Unit>().representative()->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector();
}

/* Complex */

bool Expression::EncounteredComplex() {
  return s_approximationEncounteredComplex;
}

void Expression::SetEncounteredComplex(bool encounterComplex) {
  s_approximationEncounteredComplex = encounterComplex;
}

bool Expression::hasComplexI(Context * context, SymbolicComputation replaceSymbols) const {
  return !isUninitialized() && recursivelyMatches(
      [](const Expression e, Context * context) {
        return (e.type() == ExpressionNode::Type::ConstantMaths &&
                static_cast<const Constant &>(e).isComplexI()) ||
               (e.type() == ExpressionNode::Type::ComplexCartesian &&
                static_cast<const ComplexCartesian&>(e).imag().isNull(context) != TrinaryBoolean::True);
      },
      context, replaceSymbols);
}

bool Expression::isReal(Context * context, bool canContainMatrices) const {
  /* We could do something virtual instead of implementing a disjunction on
   * types but many expressions have the same implementation so it is easier to
   * factorize it here. */

  // These expressions are real if their children are
  if (isOfType({ExpressionNode::Type::ArcTangent, ExpressionNode::Type::Conjugate, ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine, ExpressionNode::Type::Tangent})) {
    return childAtIndex(0).isReal(context, canContainMatrices);
  }

  // These expressions are always real
  if ((isNumber() && !isUndefined()) || isOfType({ExpressionNode::Type::BinomialCoefficient, ExpressionNode::Type::Derivative, ExpressionNode::Type::DivisionQuotient, ExpressionNode::Type::DivisionRemainder, ExpressionNode::Type::GreatCommonDivisor, ExpressionNode::Type::Integral, ExpressionNode::Type::LeastCommonMultiple, ExpressionNode::Type::PermuteCoefficient, ExpressionNode::Type::Randint, ExpressionNode::Type::Random, ExpressionNode::Type::Round, ExpressionNode::Type::SignFunction, ExpressionNode::Type::Unit})) {
    return true;
  }

  // These expressions are real when they are scalar
  if (isOfType({ExpressionNode::Type::AbsoluteValue, ExpressionNode::Type::Ceiling, ExpressionNode::Type::ComplexArgument, ExpressionNode::Type::Factorial, ExpressionNode::Type::Floor, ExpressionNode::Type::FracPart, ExpressionNode::Type::ImaginaryPart, ExpressionNode::Type::RealPart})) {
    return !deepIsMatrix(context, canContainMatrices);
  }

  // NAryExpresions are real if all children are real
  if (IsNAry(*this, context)) {
    return convert<NAryExpression>().allChildrenAreReal(context, canContainMatrices) == 1;
  }

  if (type() == ExpressionNode::Type::ConstantMaths) {
    return static_cast<ConstantNode *>(node())->isReal();
  }

  if (type() == ExpressionNode::Type::Power) {
    return static_cast<PowerNode *>(node())->isReal(context, canContainMatrices);
  }

  return false;
}

void Expression::SetReductionEncounteredUndistributedList(bool encounter) {
  s_reductionEncounteredUndistributedList = encounter;
}

/* Comparison */

bool Expression::isIdenticalTo(const Expression e) const {
  /* We use the simplification order only because it is a already-coded total
   * order on expresssions. */
  return ExpressionNode::SimplificationOrder(node(), e.node(), true) == 0;
}

bool Expression::isIdenticalToWithoutParentheses(const Expression e) const {
  // Same as isIdenticalTo, but ignoring the parentheses.
  return ExpressionNode::SimplificationOrder(node(), e.node(), true, true) == 0;
}

bool Expression::containsSameDependency(const Expression e, const ReductionContext& reductionContext) const {
  if (isIdenticalToWithoutParentheses(e)) {
    return true;
  }
  if (e.type() == ExpressionNode::Type::Power && type() == ExpressionNode::Type::Power && e.childAtIndex(0).isIdenticalToWithoutParentheses(childAtIndex(0))) {
    Power ePower = static_cast<const Power &>(e);
    Power thisPower = static_cast<const Power &>(*this);
    Power::DependencyType depTypeOfE = ePower.typeOfDependency(reductionContext);
    Power::DependencyType depTypeOfThis = thisPower.typeOfDependency(reductionContext);
    if (depTypeOfThis == depTypeOfE || depTypeOfThis == Power::DependencyType::Both || depTypeOfE == Power::DependencyType::None) {
      return true;
    }
  }
  int n = numberOfChildren();
  for (int i = 0; i < n; i++) {
    if(childAtIndex(i).containsSameDependency(e, reductionContext)) {
      return true;
    }
  }
  return false;
}

bool Expression::ExactAndApproximateExpressionsAreEqual(Expression exactExpression, Expression approximateExpression) {
  if (approximateExpression.isAlternativeFormOfRationalNumber() && exactExpression.isAlternativeFormOfRationalNumber()) {
    /* The only case of exact and approximate expressions being different
    * but still equal, is when a rational is equal to a decimal.
    * Ex: 1/2 == 0.5 */
    ReductionContext reductionContext = ReductionContext();
    Expression exp0 = exactExpression.clone().deepReduce(reductionContext);
    Expression exp1 = approximateExpression.clone().deepReduce(reductionContext);
    return exp0.isIdenticalTo(exp1);
  }
  /* Check deeply for equality, because the expression can be a list, a matrix
   * or a complex composed of rationals.
   * Ex: i/2 == 0.5i */
  if (exactExpression.type() == approximateExpression.type() && exactExpression.numberOfChildren() == approximateExpression.numberOfChildren()) {
    int nChildren = exactExpression.numberOfChildren();
    if (nChildren == 0) {
      return true;
    }
    for (int i = 0; i < nChildren; i++) {
      if (!ExactAndApproximateExpressionsAreEqual(exactExpression.childAtIndex(i), approximateExpression.childAtIndex(i))) {
        return false;
      }
    }
    return true;
  }
  return false;
}

/* Layout Helper */

static bool LayoutHasStringWithThousandSeparator(Layout l) {
  if (l.type() == LayoutNode::Type::StringLayout) {
    return static_cast<StringLayoutNode *>(l.node())->numberOfThousandsSeparators() > 0;
  }
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (LayoutHasStringWithThousandSeparator(l.childAtIndex(i))) {
      return true;
    }
  }
  return false;
}

static bool LayoutHasLockedMargins(Layout l) {
  if (l.node()->marginIsLocked()) {
    return true;
  }
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    if (LayoutHasLockedMargins(l.childAtIndex(i))) {
      return true;
    }
  }
  return false;
}

static void StripMarginFromLayout(Layout l) {
  l.node()->setMargin(false);
  int n = l.numberOfChildren();
  for (int i = 0; i < n; i++) {
    StripMarginFromLayout(l.childAtIndex(i));
  }
}

Layout Expression::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context, bool forceStripMargin, bool nested) const {
  if (isUninitialized()) {
    return Layout();
  }
  Layout l = node()->createLayout(floatDisplayMode, numberOfSignificantDigits, context);
  assert(!l.isUninitialized());
  if (forceStripMargin || !(nested || LayoutHasLockedMargins(l) || LayoutHasStringWithThousandSeparator(l))) {
    StripMarginFromLayout(l);
  }
  return l;
}

int Expression::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return isUninitialized() ? 0 : node()->serialize(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

/* Simplification */

Expression Expression::ParseAndSimplify(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation, UnitConversion unitConversion, bool * reductionFailure) {
  Expression exp = Parse(text, context, false);
  if (exp.isUninitialized()) {
    return Undefined::Builder();
  }
  complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(complexFormat, exp, context);
  exp = exp.cloneAndSimplify(ReductionContext(context, complexFormat, angleUnit, unitFormat, ReductionTarget::User, symbolicComputation, unitConversion), reductionFailure);
  assert(!exp.isUninitialized());
  return exp;
}

void Expression::ParseAndSimplifyAndApproximate(const char * text, Expression * parsedExpression, Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation, UnitConversion unitConversion) {
  assert(parsedExpression && simplifiedExpression);
  Expression exp = Parse(text, context, false);
  *parsedExpression = exp;
  complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(complexFormat, exp, context);
  if (exp.isUninitialized()) {
    *simplifiedExpression = Undefined::Builder();
    *approximateExpression = Undefined::Builder();
    return;
  }
  exp.cloneAndSimplifyAndApproximate(simplifiedExpression, approximateExpression, context, complexFormat, angleUnit, unitFormat, symbolicComputation, unitConversion);
  assert(!simplifiedExpression->isUninitialized() && (!approximateExpression || !approximateExpression->isUninitialized()));
}

Expression Expression::cloneAndSimplify(ReductionContext reductionContext, bool * reductionFailure) {
  bool reduceFailure = false;
  Expression e = cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reductionFailure) {
    *reductionFailure = reduceFailure;
  }
  if (reduceFailure || (type() == ExpressionNode::Type::Store && !static_cast<const Store *>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    return e;
  }
  return e.deepBeautify(reductionContext);
}

void makePositive(Expression * e, bool * isNegative) {
  if (e->type() == ExpressionNode::Type::Opposite) {
    *isNegative = true;
    *e = e->childAtIndex(0);
  }
}

void Expression::beautifyAndApproximateScalar(Expression * simplifiedExpression, Expression * approximateExpression, ReductionContext userReductionContext) {
  bool hasUnits = hasUnit();
  Context * context = userReductionContext.context();
  Preferences::AngleUnit angleUnit = userReductionContext.angleUnit();
  Preferences::ComplexFormat complexFormat = userReductionContext.complexFormat();
  /* Case 1: the reduced expression is ComplexCartesian or pure real, we can
   * take into account the complex format to display a+i*b or r*e^(i*th) */
  if ((type() == ExpressionNode::Type::ComplexCartesian || isReal(context)) && !hasUnits && !s_reductionEncounteredUndistributedList) {
    ComplexCartesian ecomplex = type() == ExpressionNode::Type::ComplexCartesian ? convert<ComplexCartesian>() : ComplexCartesian::Builder(*this, Rational::Builder(0));
    if (approximateExpression) {
      /* Step 1: Approximation
       * We compute the approximate expression from the Cartesian form to avoid
       * unprecision. For example, if the result is the ComplexCartesian(a,b),
       * the final expression is going to be sqrt(a^2+b^2)*exp(i*atan(b/a)...
       * in Polar ComplexFormat. If we approximate this expression instead of
       * ComplexCartesian(a,b), we are going to loose precision on the resulting
       * complex.*/
      // Clone the ComplexCartesian to use it to compute the approximation
      ComplexCartesian ecomplexClone = ecomplex.clone().convert<ComplexCartesian>();
      // To minimize the error on the approximation, we reduce the number of nodes in the expression by beautifying
      ecomplexClone.real().deepBeautify(userReductionContext);
      ecomplexClone.imag().deepBeautify(userReductionContext);
      *approximateExpression = ecomplexClone.approximate<double>(context, complexFormat, angleUnit);
    }
    // Step 2: create the simplified expression with the required complex format
    Expression ra = complexFormat == Preferences::ComplexFormat::Polar ?
      ecomplex.clone().convert<ComplexCartesian>().norm(userReductionContext).shallowReduce(userReductionContext) :
      ecomplex.real();
    Expression tb = complexFormat == Preferences::ComplexFormat::Polar ?
      ecomplex.argument(userReductionContext).shallowReduce(userReductionContext) :
      ecomplex.imag();
    ra = ra.deepBeautify(userReductionContext);
    tb = tb.deepBeautify(userReductionContext);
    bool raIsNegative = false;
    bool tbIsNegative = false;
    makePositive(&ra, &raIsNegative);
    makePositive(&tb, &tbIsNegative);
    *simplifiedExpression = CreateComplexExpression(ra, tb, complexFormat, ra.isUndefined() || tb.isUndefined(), IsZero(ra), IsOne(ra), IsZero(tb), IsOne(tb), raIsNegative, tbIsNegative);
  } else {
    // Step 1: beautifying
    *simplifiedExpression = deepBeautify(userReductionContext);
    // Step 2: approximation
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximateKeepingUnits<double>(userReductionContext);
    }
  }
}

void Expression::SimplifyAndApproximateChildren(Expression input, Expression * simplifiedOutput, Expression * approximateOutput, const ReductionContext& reductionContext) {
  assert(input.type() == ExpressionNode::Type::Matrix || input.type() == ExpressionNode::Type::List);
  List simplifiedChildren = List::Builder(), approximatedChildren = List::Builder();
  int n = input.numberOfChildren();
  for (int i = 0; i < n; i++) {
    Expression simplifiedChild, approximateChild;
    Expression * approximateChildAddress = approximateOutput ? &approximateChild : nullptr;
    Expression childI = input.childAtIndex(i);
    childI.beautifyAndApproximateScalar(&simplifiedChild, approximateChildAddress, reductionContext);
    simplifiedChildren.addChildAtIndexInPlace(simplifiedChild, i, i);
    if (approximateOutput) {
      assert(!approximateChild.isUninitialized());
      /* Clone the child in case it was set to the same node as simplified
       * child. This can happen when beautifying an unreduced matrix. */
      approximatedChildren.addChildAtIndexInPlace(approximateChild.clone(), i, i);
    }
  }

  if (input.type() == ExpressionNode::Type::List) {
    *simplifiedOutput = simplifiedChildren;
    if (approximateOutput) {
      *approximateOutput = approximatedChildren;
    }
    return;
  }

  assert(input.type() == ExpressionNode::Type::Matrix);
  Matrix simplifiedMatrix = Matrix::Builder(), approximateMatrix = Matrix::Builder();
  for (int i = 0; i < n; i++) {
    simplifiedMatrix.addChildAtIndexInPlace(simplifiedChildren.childAtIndex(i), i, i);
    if (approximateOutput) {
      approximateMatrix.addChildAtIndexInPlace(approximatedChildren.childAtIndex(i), i, i);
    }
  }
  Matrix m = static_cast<Matrix &>(input);
  simplifiedMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
  *simplifiedOutput = simplifiedMatrix;
  if (approximateOutput) {
    approximateMatrix.setDimensions(m.numberOfRows(), m.numberOfColumns());
    *approximateOutput = approximateMatrix;
  }
}

void Expression::cloneAndSimplifyAndApproximate(Expression * simplifiedExpression, Expression * approximateExpression, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation, UnitConversion unitConversion) const {
  assert(simplifiedExpression && simplifiedExpression->isUninitialized());
  assert(!approximateExpression || approximateExpression->isUninitialized());

  s_reductionEncounteredUndistributedList = false;

  // Step 1: we reduce the expression
  ReductionContext userReductionContext = ReductionContext(context, complexFormat, angleUnit, unitFormat, ReductionTarget::User, symbolicComputation, unitConversion);
  ReductionContext reductionContext = userReductionContext;
  bool reduceFailure = false;
  Expression e = cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
  if (reduceFailure || (type() == ExpressionNode::Type::Store && !static_cast<const Store *>(this)->isTrulyReducedInShallowReduce())) {
    // We can't beautify unreduced expression
    *simplifiedExpression = e;
    if (approximateExpression) {
      *approximateExpression = simplifiedExpression->approximate<double>(context, complexFormat, angleUnit);
    }
    return;
  }

  // Step 2: we approximate and beautify the reduced expression
  /* Case 1: the reduced expression is a matrix or a list : We scan the
   * children to beautify them with the right complex format. */
  if (e.type() == ExpressionNode::Type::Matrix || e.type() == ExpressionNode::Type::List) {
    SimplifyAndApproximateChildren(e, simplifiedExpression, approximateExpression, userReductionContext);
  } else {
    /* Case 2: the reduced expression is scalar or too complex to respect the
     * complex format. */
    e.beautifyAndApproximateScalar(simplifiedExpression, approximateExpression, userReductionContext);
  }
}

Expression Expression::ExpressionWithoutSymbols(Expression e, Context * context, SymbolicComputation symbolicComputation) {
  if (e.isUninitialized()) {
    return e;
  }

  // Replace all the symbols in depth.
  TrinaryBoolean isCircular = TrinaryBoolean::Unknown;
  e = e.deepReplaceReplaceableSymbols(context, &isCircular, 0, symbolicComputation);
  if (isCircular != TrinaryBoolean::True) {
    return e;
  }
  /* Symbols are defined circularly (or likely to be if we made too many
   * replacements), in which case we return an uninitialized expression. */
  return Expression();
}

Expression Expression::radianToAngleUnit(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*180/Pi
    return Multiplication::Builder(*this, Rational::Builder(180), Power::Builder(Constant::PiBuilder(), Rational::Builder(-1)));
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*200/Pi
    return Multiplication::Builder(*this, Rational::Builder(200), Power::Builder(Constant::PiBuilder(), Rational::Builder(-1)));
  }
  return *this;
}

Expression Expression::angleUnitToRadian(Preferences::AngleUnit angleUnit) {
  if (angleUnit == Preferences::AngleUnit::Degree) {
    // e*Pi/180
    return Multiplication::Builder(*this, Rational::Builder(1, 180), Constant::PiBuilder());
  }
  else if (angleUnit == Preferences::AngleUnit::Gradian) {
    // e*Pi/200
    return Multiplication::Builder(*this, Rational::Builder(1, 200), Constant::PiBuilder());
  }
  return *this;
}

Expression Expression::reduceAndRemoveUnit(const ReductionContext& reductionContext, Expression * Unit) {
  /* RemoveUnit has to be called on reduced expression. reduce method is called
   * instead of deepReduce to catch interrupted simplification. */
  return deepReduce(reductionContext).removeUnit(Unit);
}

Expression Expression::cloneAndDeepReduceWithSystemCheckpoint(ReductionContext * reductionContext, bool * reduceFailure) const {
  /* We tried first with the supplied ReductionTarget. If the reduction failed
   * without any user interruption (too many nodes were generated), we try again
   * with ReductionTarget::SystemForApproximation. */
  *reduceFailure = false;
#if __EMSCRIPTEN__
  Expression e = clone().deepReduce(*reductionContext);
  {
    if (ExceptionCheckpoint::HasBeenInterrupted()) {
      ExceptionCheckpoint::ClearInterruption();
      if (reductionContext->target() == ReductionTarget::SystemForApproximation) {
        goto failure;
      }
      reductionContext->setTarget(ReductionTarget::SystemForApproximation);
      e = clone().deepReduce(*reductionContext);
      if (ExceptionCheckpoint::HasBeenInterrupted()) {
        ExceptionCheckpoint::ClearInterruption();
failure:
        *reduceFailure = true;
      }
    }
  }
#else
  Expression e;
  {
    char * treePoolCursor = TreePool::sharedPool->cursor();
    ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      e = clone().deepReduce(*reductionContext);
    } else {
      /* We don't want to tidy all the Pool in the case we are in a nested
       * cloneAndDeepReduceWithSystemCheckpoint: cleaning all the pool might
       * discard ExpressionHandles that are used by parent
       * cloneAndDeepReduceWithSystemCheckpoint. */
      reductionContext->context()->tidyDownstreamPoolFrom(treePoolCursor);
      if (reductionContext->target() != ReductionTarget::SystemForApproximation) {
        // System interruption, try again with another ReductionTarget
        reductionContext->setTarget(ReductionTarget::SystemForApproximation);
        e = clone().deepReduce(*reductionContext);
      } else {
        *reduceFailure = true;
      }
    }
  }
#endif
  if (*reduceFailure) {
    // Cloning outside of ecp's scope in case it raises an exception
    e = clone();
  }
  e = e.deepRemoveUselessDependencies(*reductionContext);
  assert(!e.isUninitialized());
  return e;
}

Expression Expression::cloneAndReduce(ReductionContext reductionContext) const {
  // TODO: Ensure all cloneAndReduce usages handle reduction failure.
  bool reduceFailure;
  return cloneAndDeepReduceWithSystemCheckpoint(&reductionContext, &reduceFailure);
}

Expression Expression::deepReduce(ReductionContext reductionContext) {
  /* WARNING: This condition is to prevent logarithm of being expanded and
   * create more complex expressions that either could not be integrated
   * because it create terms of sums that are too big, or generate
   * exponentially complex derivative when derivating multiple time.
   * This can be set for Derivative and Integral because the exact result
   * of these expression is never displayed to the user.
   * Examples where it's useful:
   *  - d^4/dx^4(log(x), x, x)
   *  - int(ln(1.0025)*e^(x*ln(1.0025)+ln(200), x, 0, 1000)
   * TODO: This solution is obviously not ideal and the simplification
   * of logarithm should be reworked. */
  if (type() == ExpressionNode::Type::Derivative || type() == ExpressionNode::Type::Integral) {
    reductionContext.setExpandLogarithm(false);
  }
  deepReduceChildren(reductionContext);
  return shallowReduce(reductionContext);
}

Expression Expression::deepRemoveUselessDependencies(const ReductionContext& reductionContext) {
  Expression result = *this;
  if (type() == ExpressionNode::Type::Dependency) {
    Dependency depThis = static_cast<Dependency &>(*this);
    result = depThis.removeUselessDependencies(reductionContext);
  }
  for(int i = 0; i < result.numberOfChildren(); i++) {
    result.childAtIndex(i).deepRemoveUselessDependencies(reductionContext);
  }
  return result;
}

Expression Expression::setSign(bool positive, const ReductionContext& reductionContext) {
  if (isNumber()) {
    // Needed to avoid infinite loop in Multiplication::shallowReduce
    Number thisNumber = static_cast<Number &>(*this);
    return thisNumber.setSign(positive);
  }
  TrinaryBoolean currentSignPositive = isPositive(reductionContext.context());
  assert(currentSignPositive != TrinaryBoolean::Unknown);
  if (BinaryToTrinaryBool(positive) == currentSignPositive) {
    return *this;
  }
  Multiplication revertedSign = Multiplication::Builder(Rational::Builder(-1));
  replaceWithInPlace(revertedSign);
  revertedSign.addChildAtIndexInPlace(*this, 1, 1);
  return revertedSign.shallowReduce(reductionContext);
}

int Expression::lengthOfListChildren() const {
  int lastLength = k_noList;
  int n = numberOfChildren();
  bool isNAry = IsNAry(*this, nullptr);
  for (int i = n - 1; i >= 0; i--) {
    if (isNAry && childAtIndex(i).type() < ExpressionNode::Type::List) {
      return lastLength;
    }
    if (childAtIndex(i).type() == ExpressionNode::Type::List) {
      int length = childAtIndex(i).numberOfChildren();
      if (lastLength == k_noList) {
        lastLength = length;
      } else if (lastLength != length) {
        return k_mismatchedLists;
      }
    }
  }
  return lastLength;
}

/* Evaluation */

template<typename U>
Expression Expression::approximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  return isUninitialized() ? Undefined::Builder() : approximateToEvaluation<U>(context, complexFormat, angleUnit, withinReduce).complexToExpression(complexFormat);
}

template<typename U> Expression Expression::approximateKeepingUnits(const ReductionContext& reductionContext) const {
  // Unit need to be extracted before approximating.
  Expression units;
  Expression expressionWithoutUnits = *this;
  if (hasUnit()) {
    ReductionContext childContext = reductionContext;
    childContext.setUnitConversion(UnitConversion::None);
    expressionWithoutUnits = clone().reduceAndRemoveUnit(childContext, &units);
  }
  Expression approximationWithoutUnits = expressionWithoutUnits.approximate<U>(reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit());
  if (units.isUninitialized()) {
    return approximationWithoutUnits;
  }
  return Multiplication::Builder(approximationWithoutUnits, units);
}

template<typename U>
U Expression::approximateToScalar(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const {
  return approximateToEvaluation<U>(context, complexFormat, angleUnit, withinReduce).toScalar();
}

template<typename U>
U Expression::ParseAndSimplifyAndApproximateToScalar(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation) {
  Expression exp = ParseAndSimplify(text, context, complexFormat, angleUnit, unitFormat, symbolicComputation);
  complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(complexFormat, exp, context);
  assert(!exp.isUninitialized());
  return exp.approximateToScalar<U>(context, complexFormat, angleUnit);
}

template<typename U>
U Expression::approximateWithValueForSymbol(const char * symbol, U x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  VariableContext variableContext = VariableContext(symbol, context);
  variableContext.setApproximationForVariable<U>(x);
  return approximateToScalar<U>(&variableContext, complexFormat, angleUnit);
}

/* Builder */

bool Expression::IsZero(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isZero();
}
bool Expression::IsOne(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isOne();
}
bool Expression::IsMinusOne(const Expression e) {
  return e.type() == ExpressionNode::Type::Rational && static_cast<const Rational &>(e).isMinusOne();
}

Expression Expression::CreateComplexExpression(Expression ra, Expression tb, Preferences::ComplexFormat complexFormat, bool undefined, bool isZeroRa, bool isOneRa, bool isZeroTb, bool isOneTb, bool isNegativeRa, bool isNegativeTb) {
  if (undefined) {
    return Undefined::Builder();
  }
  List dependencies = List::Builder();
  if (ra.type() == ExpressionNode::Type::Dependency) {
    ra = static_cast<Dependency &>(ra).extractDependencies(dependencies);
  }
  if (tb.type() == ExpressionNode::Type::Dependency) {
    tb = static_cast<Dependency &>(tb).extractDependencies(dependencies);
  }
  Expression result;
  switch (complexFormat) {
    case Preferences::ComplexFormat::Real:
    case Preferences::ComplexFormat::Cartesian:
    {
      Expression real;
      Expression imag;
      if (!isZeroRa || isZeroTb) {
        if (isNegativeRa) {
          real = Opposite::Builder(ra);
          real.shallowAddMissingParenthesis();
        } else {
          real = ra;
        }
      }
      if (!isZeroTb) {
        if (isOneTb) {
          imag = Constant::ComplexIBuilder();
        } else {
          imag = Multiplication::Builder(tb, Constant::ComplexIBuilder());
          imag.shallowAddMissingParenthesis();
        }
      }
      if (imag.isUninitialized()) {
        result = real;
        break;
      } else if (real.isUninitialized()) {
        if (isNegativeTb) {
          result = Opposite::Builder(imag);
        } else {
          result = imag;
          break;
        }
      } else if (isNegativeTb) {
        result = Subtraction::Builder(real, imag);
      } else {
        result = Addition::Builder(real, imag);
      }
      result.shallowAddMissingParenthesis();
      break;
    }
    default:
    {
      assert(complexFormat == Preferences::ComplexFormat::Polar);
      Expression norm;
      Expression exp;
      if (!isOneRa || isZeroTb) {
        /* Norm cannot be negative but can be preceded by a negative sign (for
         * instance "-log(0.3)") which would lead to isNegativeRa = True. */
        if (isNegativeRa) {
          norm = Opposite::Builder(ra);
        } else {
          norm = ra;
        }
      }
      if (!isZeroRa && !isZeroTb) {
        Expression arg;
        if (isOneTb) {
          arg = Constant::ComplexIBuilder();
        } else {
          arg = Multiplication::Builder(tb, Constant::ComplexIBuilder());
        }
        if (isNegativeTb) {
          arg = Opposite::Builder(arg);
        }
        arg.shallowAddMissingParenthesis();
        exp = Power::Builder(Constant::ExponentialEBuilder(), arg);
        exp.shallowAddMissingParenthesis();
      }
      if (exp.isUninitialized()) {
        result = norm;
      } else if (norm.isUninitialized()) {
        result = exp;
      } else {
        result = Multiplication::Builder(norm, exp);
        result.shallowAddMissingParenthesis();
      }
    }
  }

  if (dependencies.numberOfChildren() > 0) {
    result = Dependency::Builder(result, dependencies);
  }
  return result;
}

static Expression maker(Expression children, int nbChildren, TreeNode::Initializer initializer, size_t size) {
  assert(children.type() == ExpressionNode::Type::List);
  TreeHandle handle = TreeHandle::Builder(initializer, size, nbChildren);
  Expression result = static_cast<Expression &>(handle);
  for (size_t i = 0; i < static_cast<size_t>(nbChildren); i++) {
    result.replaceChildAtIndexInPlace(i, children.childAtIndex(i));
  }
  return result;
}

Expression Expression::FunctionHelper::build(Expression children) const {
  if (m_untypedBuilder) {
    return (*m_untypedBuilder)(children);
  }
  int numberOfChildren = children.numberOfChildren();
  assert(numberOfChildren >= m_minNumberOfChildren && numberOfChildren <= m_maxNumberOfChildren);
  return maker(children, numberOfChildren, m_initializer, m_size);
}

int ExpressionNode::numberOfNumericalValues() const {
  constexpr int k_error = -1;
  assert(type() != ExpressionNode::Type::Dependency);
  if (isRandom() || type() == ExpressionNode::Type::Symbol) {
    return k_error;
  }
  if (isNumber()) {
    return std::isfinite(static_cast<const NumberNode*>(this)->doubleApproximation()) ? 1 : k_error;
  }
  if (type() == ExpressionNode::Type::Power) {
    int base = childAtIndex(0)->numberOfNumericalValues();
    int exponent = childAtIndex(1)->numberOfNumericalValues();
    if (base == k_error || exponent == k_error) {
      return k_error;
    }
    return base != 0 ? base : exponent;
  }
  if (type() == ExpressionNode::Type::ConstantMaths) {
    const ConstantNode * constant = static_cast<const ConstantNode*>(this);
    // We decide that e is not a constant for e^2 to generalize as e^x
    return !constant->isExponentialE() && !constant->isComplexI() ;
  }
  int n = 0;
  for (ExpressionNode * child : children()) {
    int childValue = child->numberOfNumericalValues();
    if (childValue == k_error) {
      return k_error;
    }
    n += childValue;
  }
  return n;
}

void Expression::replaceNumericalValuesWithSymbol(Symbol x) {
  if (isNumber()) {
    return replaceWithInPlace(x);
  }
  if (type() == ExpressionNode::Type::ConstantMaths && !convert<Constant>().isExponentialE() && !convert<Constant>().isComplexI()) {
    return replaceWithInPlace(x);
  }
  if (type() == ExpressionNode::Type::Power) {
    if (childAtIndex(0).numberOfNumericalValues() == 0) {
      // replace exponent
      childAtIndex(1).replaceNumericalValuesWithSymbol(x);
    } else {
      // replace base
      childAtIndex(0).replaceNumericalValuesWithSymbol(x);
    }
    return;
  }
  for (int i = 0; i < numberOfChildren(); i++) {
    childAtIndex(i).replaceNumericalValuesWithSymbol(x);
  }
}

float Expression::getNumericalValue() {
  assert(numberOfNumericalValues() <= 1);
  if (isNumber()) {
    return convert<Number>().doubleApproximation();
  }
  if (type() == ExpressionNode::Type::ConstantMaths && !convert<Constant>().isExponentialE() && !convert<Constant>().isComplexI()) {
    return convert<Constant>().constantInfo().m_value;
  }
  Expression result = clone();
  for (int i = 0; i < numberOfChildren(); i++) {
    float result = childAtIndex(i).getNumericalValue();
    if (!std::isnan(result)) {
      return result;
    }
  }
  return NAN;
}

template Expression Expression::approximate<float>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template Expression Expression::approximate<double>(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template double Expression::approximateToScalar(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::ParseAndSimplifyAndApproximateToScalar<float>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation);
template double Expression::ParseAndSimplifyAndApproximateToScalar<double>(const char * text, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, Preferences::UnitFormat unitFormat, SymbolicComputation symbolicComputation);

template Evaluation<float> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;
template Evaluation<double> Expression::approximateToEvaluation(Context * context, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit, bool withinReduce) const;

template float Expression::approximateWithValueForSymbol(const char * symbol, float x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;
template double Expression::approximateWithValueForSymbol(const char * symbol, double x, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const;

template Expression Expression::approximateKeepingUnits<double>(const ReductionContext& reductionContext) const;
}
