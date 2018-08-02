#ifndef POINCARE_EXPRESSION_REFERENCE_H
#define POINCARE_EXPRESSION_REFERENCE_H

#include <poincare/serializable_reference.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/expression_node.h>

#include <stdio.h>

namespace Poincare {

class Context;

class ExpressionReference : public SerializableReference {
  friend class ExpressionNode;
  friend class NAryExpressionNode;
  friend class SymbolNode;
public:
  using SerializableReference::SerializableReference;

  /* Constructor & Destructor */
  virtual ~ExpressionReference() = default;
  static ExpressionReference parse(char const * string);
  static void ReplaceSymbolWithExpression(ExpressionReference * expressionAddress, char symbol, ExpressionReference expression);
  ExpressionReference clone() const;

  /* Reference */
  ExpressionNode * node() const override { return static_cast<ExpressionNode *>(SerializableReference::node()); }

  /* Circuit breaker */
  typedef bool (*CircuitBreaker)();
  static void setCircuitBreaker(CircuitBreaker cb);
  static bool shouldStopProcessing();

  /* Properties */
  ExpressionNode::Sign sign() const { return node()->sign(); }
  ExpressionReference setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) { return node()->setSign(s, context, angleUnit); }
  bool isNumber() const { return node()->isNumber(); }
  bool isRationalZero() const;
  typedef bool (*ExpressionTest)(const ExpressionReference e, Context & context);
  bool recursivelyMatches(ExpressionTest test, Context & context) const;
  bool isApproximate(Context & context) const;
  static bool IsMatrix(const ExpressionReference e, Context & context);
  /* 'characteristicXRange' tries to assess the range on x where the expression
   * (considered as a function on x) has an interesting evolution. For example,
   * the period of the function on 'x' if it is periodic. If
   * the function is x-independent, the return value is 0.0f (because any
   * x-range is equivalent). If the function does not have an interesting range,
   * the return value is NAN.
   * NB: so far, we consider that the only way of building a periodic function
   * is to use sin/tan/cos(f(x)) with f a linear function. */
  float characteristicXRange(Context & context, Preferences::AngleUnit angleUnit) const { return this->node()->characteristicXRange(context, angleUnit); }
  /* polynomialDegree returns:
   * - (-1) if the expression is not a polynome
   * - the degree of the polynome otherwise */
  int polynomialDegree(char symbolName) const { return this->node()->polynomialDegree(symbolName); }
  /* getVariables fills the table variables with the variable present in the
   * expression and returns the number of entries in filled in variables.
   * For instance getVariables('x+y+2*w/cos(4)') would result in
   * variables = « xyw »  and would return 3. If the final number of
   * variables would overflow the maxNumberOfVariables, getVariables return -1 */
  static constexpr int k_maxNumberOfVariables = 6;
  int getVariables(ExpressionNode::isVariableTest isVariable, char * variables) const { return this->node()->getVariables(isVariable, variables); }
  /* getLinearCoefficients return false if the expression is not linear with
   * the variables hold in 'variables'. Otherwise, it fills 'coefficients' with
   * the coefficients of the variables hold in 'variables' (following the same
   * order) and 'constant' with the constant of the expression. */
  bool getLinearCoefficients(char * variables, ExpressionReference coefficients[], ExpressionReference constant[], Context & context, Preferences::AngleUnit angleUnit) const;
  /* getPolynomialCoefficients fills the table coefficients with the expressions
   * of the first 3 polynomial coefficients and return polynomialDegree.
   * coefficients has up to 3 entries. It supposed to be called on Reduced
   * expression. */
  static constexpr int k_maxPolynomialDegree = 2;
  static constexpr int k_maxNumberOfPolynomialCoefficients = k_maxPolynomialDegree+1;
  int getPolynomialCoefficients(char symbolName, ExpressionReference coefficients[], Context & context, Preferences::AngleUnit angleUnit) const;

  /* Comparison */
  /* isIdenticalTo is the "easy" equality, it returns true if both trees have
   * same structures and all their nodes have same types and values (ie,
   * sqrt(pi^2) is NOT identical to pi). */
  bool isIdenticalTo(const ExpressionReference e) const {
    /* We use the simplification order only because it is a already-coded total
     * order on expresssions. */
    return ExpressionNode::SimplificationOrder(this->node(), e.node(), true) == 0;
  }
  bool isEqualToItsApproximationLayout(ExpressionReference approximation, int bufferSize, Preferences::AngleUnit angleUnit, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context & context);

  /* Layout Engine */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const { return this->node()->createLayout(floatDisplayMode, numberOfSignificantDigits); }
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const { return this->node()->writeTextInBuffer(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits); }

  /* Simplification */
  static ExpressionReference ParseAndSimplify(const char * text, Context & context, Preferences::AngleUnit angleUnit);
  static void Simplify(ExpressionReference * expressionAddress, Context & context, Preferences::AngleUnit angleUnit);

  /* Approximation Engine */
  template<typename U> ExpressionReference approximate(Context& context, Preferences::AngleUnit angleUnit, Preferences::Preferences::ComplexFormat complexFormat) const;
  template<typename U> U approximateToScalar(Context& context, Preferences::AngleUnit angleUnit) const;
  template<typename U> static U approximateToScalar(const char * text, Context& context, Preferences::AngleUnit angleUnit);
  template<typename U> U approximateWithValueForSymbol(char symbol, U x, Context & context, Preferences::AngleUnit angleUnit) const;

  /* Expression roots/extrema solver*/
  struct Coordinate2D {
    double abscissa;
    double value;
  };
  Coordinate2D nextMinimum(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextMaximum(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  double nextRoot(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit) const;
  Coordinate2D nextIntersection(char symbol, double start, double step, double max, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression) const;

private:
  /* Hierarchy */
  ExpressionReference childAtIndex(int i) const {
    return ExpressionReference(TreeReference::treeChildAtIndex(i).node());
  }

  /* Simplification */
  ExpressionReference deepBeautify(Context & context, Preferences::AngleUnit angleUnit);
  ExpressionReference deepReduce(Context & context, Preferences::AngleUnit angleUnit);
  // TODO: should be virtual pure
  virtual ExpressionReference shallowReduce(Context & context, Preferences::AngleUnit angleUnit) { return this->node()->shallowReduce(context, angleUnit); }
  virtual ExpressionReference shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) { return this->node()->shallowBeautify(context, angleUnit); };

  /* Approximation */
  template<typename U> static U epsilon();

  /* Expression roots/extrema solver*/
  constexpr static double k_solverPrecision = 1.0E-5;
  constexpr static double k_sqrtEps = 1.4901161193847656E-8; // sqrt(DBL_EPSILON)
  constexpr static double k_goldenRatio = 0.381966011250105151795413165634361882279690820194237137864; // (3-sqrt(5))/2
  constexpr static double k_maxFloat = 1e100;
  typedef double (*EvaluationAtAbscissa)(char symbol, double abscissa, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression0, const ExpressionReference expression1);
  Coordinate2D nextMinimumOfExpression(char symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression = ExpressionReference(nullptr), bool lookForRootMinimum = false) const;
  void bracketMinimum(char symbol, double start, double step, double max, double result[3], EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression = ExpressionReference(nullptr)) const;
  Coordinate2D brentMinimum(char symbol, double ax, double bx, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression = ExpressionReference(nullptr)) const;
  double nextIntersectionWithExpression(char symbol, double start, double step, double max, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression) const;
  void bracketRoot(char symbol, double start, double step, double max, double result[2], EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression) const;
  double brentRoot(char symbol, double ax, double bx, double precision, EvaluationAtAbscissa evaluation, Context & context, Preferences::AngleUnit angleUnit, const ExpressionReference expression) const;
};



/*
class UndefinedNode : public ExpressionNode {
public:
  // ExpressionNode
  Type type() const override { return Type::Undefined; }
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined().storeEvaluation(); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined().storeEvaluation(); }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class SymbolNode : public ExpressionNode {
public:
  bool isApproximate(Context & context) const;
  bool isMatrixSymbol() const;
  char name() const;
  // ExpressionNode
  Type type() const override { return Type::Undefined; }
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined().storeEvaluation(); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined().storeEvaluation(); }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class RationalNode : public ExpressionNode {
public:
  // ExpressionNode
  Type type() const override { return Type::Rational; }
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined().storeEvaluation(); }
  Evaluation<double> * approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined().storeEvaluation(); }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};

class OppositeNode : public ExpressionNode {
public:
  // ExpressionNode
  Type type() const override { return Type::Opposite; }
  Evaluation<float> * approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>::Undefined().storeEvaluation(); }
  Evaluation<double> * approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>::Undefined().storeEvaluation(); }
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override { return sizeof(UndefinedNode); }
  const char * description() const override { return "Allocation Failed";  }
  int numberOfChildren() const override { return 0; }
  bool isAllocationFailure() const override { return true; }
};


typedef ExpressionReference UndefinedRef;
typedef ExpressionReference SymbolRef;
class RationalRef : public ExpressionReference {
public:
  RationalRef(int i) :
    ExpressionReference()
  {
  }
};

class OppositeRef : public ExpressionReference {
public:
  OppositeRef(ExpressionReference child) :
    ExpressionReference()
  {
  }
};
*/
}

#endif
