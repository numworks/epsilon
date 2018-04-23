#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/dynamic_hierarchy.h>
#include <poincare/static_hierarchy.h>
#include <poincare/list_data.h>
#include <poincare/matrix_data.h>
#include <poincare/undefined.h>
#include <poincare/simplification_root.h>
#include <poincare/rational.h>
#include <poincare/matrix.h>
#include <poincare/decimal.h>
#include <poincare/ieee754.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/subtraction.h>
#include <poincare/power.h>
#include <poincare/opposite.h>
#include <ion.h>
#include <cmath>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"

int poincare_expression_yyparse(Poincare::Expression ** expressionOutput);

namespace Poincare {

#include <stdio.h>

/* Constructor & Destructor */

Expression * Expression::parse(char const * string) {
  if (string[0] == 0) {
    return nullptr;
  }
  YY_BUFFER_STATE buf = poincare_expression_yy_scan_string(string);
  Expression * expression = 0;
  if (poincare_expression_yyparse(&expression) != 0) {
    // Parsing failed because of invalid input or memory exhaustion
    if (expression != nullptr) {
      delete expression;
      expression = nullptr;
    }
  }
  poincare_expression_yy_delete_buffer(buf);

  return expression;
}

void Expression::ReplaceSymbolWithExpression(Expression ** expressionAddress, char symbol, Expression * expression) {
  SimplificationRoot root(*expressionAddress);
  root.editableOperand(0)->replaceSymbolWithExpression(symbol, expression);
  *expressionAddress = root.editableOperand(0);
}

Expression * Expression::replaceSymbolWithExpression(char symbol, Expression * expression) {
  for (int i = 0; i < numberOfOperands(); i++) {
    editableOperand(i)->replaceSymbolWithExpression(symbol, expression);
  }
  return this;
}

/* Circuit breaker */

static Expression::CircuitBreaker sCircuitBreaker = nullptr;
static bool sSimplificationHasBeenInterrupted = false;

void Expression::setCircuitBreaker(CircuitBreaker cb) {
  sCircuitBreaker = cb;
}

bool Expression::shouldStopProcessing() {
  if (sCircuitBreaker == nullptr) {
    return false;
  }
  if (sCircuitBreaker()) {
    sSimplificationHasBeenInterrupted = true;
    return true;
  }
  return false;
}

/* Hierarchy */

const Expression * Expression::operand(int i) const {
  assert(i >= 0);
  assert(i < numberOfOperands());
  assert(operands()[i]->parent() == nullptr || operands()[i]->parent() == this);
  return operands()[i];
}

Expression * Expression::replaceWith(Expression * newOperand, bool deleteAfterReplace) {
  assert(m_parent != nullptr);
  m_parent->replaceOperand(this, newOperand, deleteAfterReplace);
  return newOperand;
}

void Expression::replaceOperand(const Expression * oldOperand, Expression * newOperand, bool deleteOldOperand) {
  assert(newOperand != nullptr);
  // Caution: handle the case where we replace an operand with a descendant of ours.
  if (newOperand->hasAncestor(this)) {
    newOperand->parent()->detachOperand(newOperand);
  }
  Expression ** op = const_cast<Expression **>(operands());
  for (int i=0; i<numberOfOperands(); i++) {
    if (op[i] == oldOperand) {
      if (oldOperand != nullptr && oldOperand->parent() == this) {
        const_cast<Expression *>(oldOperand)->setParent(nullptr);
      }
      if (deleteOldOperand) {
        delete oldOperand;
      }
      if (newOperand != nullptr) {
        const_cast<Expression *>(newOperand)->setParent(this);
      }
      op[i] = newOperand;
      break;
    }
  }
}

void Expression::detachOperand(const Expression * e) {
  Expression ** op = const_cast<Expression **>(operands());
  for (int i=0; i<numberOfOperands(); i++) {
    if (op[i] == e) {
      detachOperandAtIndex(i);
    }
  }
}

void Expression::detachOperands() {
  for (int i=0; i<numberOfOperands(); i++) {
    detachOperandAtIndex(i);
  }
}

void Expression::detachOperandAtIndex(int i) {
  Expression ** op = const_cast<Expression **>(operands());
  // When detachOperands is called, it's very likely that said operands have been stolen
  if (op[i] != nullptr && op[i]->parent() == this) {
    const_cast<Expression *>(op[i])->setParent(nullptr);
  }
  op[i] = nullptr;
}

void Expression::swapOperands(int i, int j) {
  assert(i >= 0 && i < numberOfOperands());
  assert(j >= 0 && j < numberOfOperands());
  Expression ** op = const_cast<Expression **>(operands());
  Expression * temp = op[i];
  op[i] = op[j];
  op[j] = temp;
}

bool Expression::hasAncestor(const Expression * e) const {
  if (m_parent == e) {
    return true;
  }
  if (m_parent == nullptr) {
    return false;
  }
  return m_parent->hasAncestor(e);
}

/* Properties */

bool Expression::recursivelyMatches(ExpressionTest test, Context & context) const {
  if (test(this, context)) {
    return true;
  }
  for (int i = 0; i < numberOfOperands(); i++) {
    if (operand(i)->recursivelyMatches(test, context)) {
      return true;
    }
  }
  return false;
}

bool Expression::isApproximate(Context & context) const {
  return recursivelyMatches([](const Expression * e, Context & context) {
        return e->type() == Expression::Type::Decimal || Expression::IsMatrix(e, context) || (e->type() == Expression::Type::Symbol && static_cast<const Symbol *>(e)->isApproximate(context));
    }, context);
}

float Expression::characteristicXRange(Context & context, AngleUnit angleUnit) const {
  if (angleUnit == AngleUnit::Default) {
    angleUnit = Preferences::sharedPreferences()->angleUnit();
  }
  /* A expression has a characteristic range if at least one of its operand has
   * one and the other are x-independant. We keep the biggest interesting range
   * among the operand interesting ranges. */
  float range = 0.0f;
  for (int i = 0; i < numberOfOperands(); i++) {
    float opRange = operand(i)->characteristicXRange(context, angleUnit);
    if (std::isnan(opRange)) {
      return NAN;
    } else if (range < opRange) {
      range = opRange;
    }
  }
  return range;
}

bool Expression::IsMatrix(const Expression * e, Context & context) {
  return e->type() == Type::Matrix || e->type() == Type::ConfidenceInterval || e->type() == Type::MatrixDimension || e->type() == Type::PredictionInterval || e->type() == Type::MatrixInverse || e->type() == Type::MatrixTranspose || (e->type() == Type::Symbol && static_cast<const Symbol *>(e)->isMatrixSymbol());
}

int Expression::polynomialDegree(char symbolName) const {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (operand(i)->polynomialDegree(symbolName) != 0) {
      return -1;
    }
  }
  return 0;
}

bool Expression::isOfType(Type * types, int length) const {
  for (int i = 0; i < length; i++) {
    if (type() == types[i]) {
      return true;
    }
  }
  return false;
}

bool Expression::needParenthesisWithParent(const Expression * e) const {
  return false;
}

/* Comparison */

int Expression::SimplificationOrder(const Expression * e1, const Expression * e2, bool canBeInterrupted) {
  if (e1->type() > e2->type()) {
    if (canBeInterrupted && shouldStopProcessing()) {
      return 1;
    }
    return -(e2->simplificationOrderGreaterType(e1, canBeInterrupted));
  } else if (e1->type() == e2->type()) {
    return e1->simplificationOrderSameType(e2, canBeInterrupted);
  } else {
    if (canBeInterrupted && shouldStopProcessing()) {
      return -1;
    }
    return e1->simplificationOrderGreaterType(e2, canBeInterrupted);
  }
}

/* Layout */

ExpressionLayout * Expression::createLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  switch (floatDisplayMode) {
    case PrintFloat::Mode::Default:
      switch (complexFormat) {
        case ComplexFormat::Default:
          return privateCreateLayout(Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->complexFormat());
        default:
          return privateCreateLayout(Preferences::sharedPreferences()->displayMode(), complexFormat);
      }
    default:
      switch (complexFormat) {
        case ComplexFormat::Default:
          return privateCreateLayout(floatDisplayMode, Preferences::sharedPreferences()->complexFormat());
        default:
          return privateCreateLayout(floatDisplayMode, complexFormat);
      }
  }
}

/* Simplification */

Expression * Expression::ParseAndSimplify(const char * text, Context & context, AngleUnit angleUnit) {
  Expression * exp = parse(text);
  if (exp == nullptr) {
    return new Undefined();
  }
  Simplify(&exp, context, angleUnit);
  if (exp == nullptr) {
    return parse(text);
  }
  return exp;
}

void Expression::Simplify(Expression ** expressionAddress, Context & context, AngleUnit angleUnit) {
  sSimplificationHasBeenInterrupted = false;
  if (angleUnit == AngleUnit::Default) {
    angleUnit = Preferences::sharedPreferences()->angleUnit();
  }
#if MATRIX_EXACT_REDUCING
#else
  if ((*expressionAddress)->recursivelyMatches(IsMatrix, context)) {
    return;
  }
#endif
  SimplificationRoot root(*expressionAddress);
  root.editableOperand(0)->deepReduce(context, angleUnit);
  root.editableOperand(0)->deepBeautify(context, angleUnit);
  *expressionAddress = root.editableOperand(0);
  if (sSimplificationHasBeenInterrupted) {
    root.detachOperands();
    delete *expressionAddress;
    *expressionAddress = nullptr;
  }
}


void Expression::Reduce(Expression ** expressionAddress, Context & context, AngleUnit angleUnit, bool recursively) {
  SimplificationRoot root(*expressionAddress);
  if (recursively) {
    root.editableOperand(0)->deepReduce(context, angleUnit);
  } else {
    root.editableOperand(0)->shallowReduce(context,angleUnit);
  }
  *expressionAddress = root.editableOperand(0);
}

Expression * Expression::deepReduce(Context & context, AngleUnit angleUnit) {
  assert(parent() != nullptr);
  for (int i = 0; i < numberOfOperands(); i++) {
    editableOperand(i)->deepReduce(context, angleUnit);
  }
  return shallowReduce(context, angleUnit);
}

Expression * Expression::shallowReduce(Context & context, AngleUnit angleUnit) {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (editableOperand(i)->type() == Type::Undefined && this->type() != Type::SimplificationRoot) {
      return replaceWith(new Undefined(), true);
    }
  }
  return this;
}

Expression * Expression::deepBeautify(Context & context, AngleUnit angleUnit) {
  assert(parent() != nullptr);
  Expression * e = shallowBeautify(context, angleUnit);
  for (int i = 0; i < e->numberOfOperands(); i++) {
    e->editableOperand(i)->deepBeautify(context, angleUnit);
  }
  return e;
}

/* Evaluation */

template<typename T> Expression * Expression::approximate(Context& context, AngleUnit angleUnit, ComplexFormat complexFormat) const {
  if (angleUnit == AngleUnit::Default) {
    angleUnit = Preferences::sharedPreferences()->angleUnit();
  }
  if (complexFormat == ComplexFormat::Default) {
    complexFormat = Preferences::sharedPreferences()->complexFormat();
  }
  Expression * result = nullptr;
  Evaluation<T> * e = privateApproximate(T(), context, angleUnit);
  if (e->type() == Evaluation<T>::Type::Complex) {
    result = complexToExpression(*(static_cast<Complex<T> *>(e)), complexFormat);
  } else {
    MatrixComplex<T> * matrix = static_cast<MatrixComplex<T> *>(e);
    Expression ** operands = new Expression * [matrix->numberOfComplexOperands()];
    for (int i = 0; i < matrix->numberOfComplexOperands(); i++) {
      operands[i] = complexToExpression(matrix->complexOperand(i), complexFormat);
    }
    result = new Matrix(operands, matrix->numberOfRows(), matrix->numberOfColumns(), false);
    delete[] operands;
  }
  delete e;
  return result;
}

template<typename T> T Expression::approximateToScalar(Context& context, AngleUnit angleUnit, ComplexFormat complexFormat) const {
  if (angleUnit == AngleUnit::Default) {
    angleUnit = Preferences::sharedPreferences()->angleUnit();
  }
  if (complexFormat == ComplexFormat::Default) {
    complexFormat = Preferences::sharedPreferences()->complexFormat();
  }
  Evaluation<T> * evaluation = privateApproximate(T(), context, angleUnit);
  T result = evaluation->toScalar();
  /*if (evaluation->type() == Type::Matrix) {
    if (numberOfOperands() == 1) {
      result = static_cast<const Complex<T> *>(operand(0))->toScalar();
    }
  }*/
  delete evaluation;
  return result;
}

template<typename T> T Expression::approximateToScalar(const char * text, Context& context, AngleUnit angleUnit, ComplexFormat complexFormat) {
  Expression * exp = ParseAndSimplify(text, context, angleUnit);
  T result = exp->approximateToScalar<T>(context, angleUnit, complexFormat);
  delete exp;
  return result;
}

template<typename T> T Expression::epsilon() {
  static T epsilon = sizeof(T) == sizeof(double) ? 1E-15 : 1E-7f;
  return epsilon;
}

template <typename T>
Expression * Expression::CreateDecimal(T f) {
  if (std::isnan(f) || std::isinf(f)) {
    return new Undefined();
  }
  return new Decimal(f);
}

template<typename T> Expression * Expression::complexToExpression(std::complex<T> c, ComplexFormat complexFormat) {
  if (complexFormat == ComplexFormat::Default) {
    complexFormat = Preferences::sharedPreferences()->complexFormat();
  }
  if (std::isnan(c.real()) || std::isnan(c.imag()) || std::isinf(c.real()) || std::isinf(c.imag())) {
    return new Undefined();
  }

  switch (complexFormat) {
    case ComplexFormat::Cartesian:
    {
      Expression * real = nullptr;
      Expression * imag = nullptr;
      if (c.real() != 0 || c.imag() == 0) {
        real = CreateDecimal(c.real());
      }
      if (c.imag() != 0) {
        if (c.imag() == 1.0 || c.imag() == -1) {
          imag = new Symbol(Ion::Charset::IComplex);
        } else if (c.imag() > 0) {
          imag = new Multiplication(CreateDecimal(c.imag()), new Symbol(Ion::Charset::IComplex), false);
        } else {
          imag = new Multiplication(CreateDecimal(-c.imag()), new Symbol(Ion::Charset::IComplex), false);
        }
      }
      if (imag == nullptr) {
        return real;
      } else if (real == nullptr) {
        if (c.imag() > 0) {
          return imag;
        } else {
          return new Opposite(imag, false);
        }
        return imag;
      } else if (c.imag() > 0) {
        return new Addition(real, imag, false);
      } else {
        return new Subtraction(real, imag, false);
      }
    }
    default:
    {
      assert(complexFormat == ComplexFormat::Polar);
      Expression * norm = nullptr;
      Expression * exp = nullptr;
      T r = std::abs(c);
      T th = std::arg(c);
      if (r != 1 || th == 0) {
        norm = CreateDecimal(r);
      }
      if (r != 0 && th != 0) {
        Expression * arg = nullptr;
        if (th > 0) {
          arg = new Multiplication(CreateDecimal(th), new Symbol(Ion::Charset::IComplex), false);
        } else {
          arg = new Opposite(new Multiplication(CreateDecimal(-th), new Symbol(Ion::Charset::IComplex), false));
        }
        exp = new Power(new Symbol(Ion::Charset::Exponential), arg, false);
      }
      if (exp == nullptr) {
        return norm;
      } else if (norm == nullptr) {
        return exp;
      } else {
        return new Multiplication(norm, exp, false);
      }
    }
  }
}

}

template Poincare::Expression * Poincare::Expression::approximate<double>(Context& context, AngleUnit angleUnit, ComplexFormat complexFormat) const;
template Poincare::Expression * Poincare::Expression::approximate<float>(Context& context, AngleUnit angleUnit, ComplexFormat complexFormat) const;
template double Poincare::Expression::approximateToScalar<double>(char const*, Poincare::Context&, Poincare::Expression::AngleUnit, ComplexFormat complexFormat);
template float Poincare::Expression::approximateToScalar<float>(char const*, Poincare::Context&, Poincare::Expression::AngleUnit, ComplexFormat complexFormat);
template double Poincare::Expression::approximateToScalar<double>(Poincare::Context&, Poincare::Expression::AngleUnit, ComplexFormat complexFormat) const;
template float Poincare::Expression::approximateToScalar<float>(Poincare::Context&, Poincare::Expression::AngleUnit, ComplexFormat complexFormat) const;
template double Poincare::Expression::epsilon<double>();
template float Poincare::Expression::epsilon<float>();
