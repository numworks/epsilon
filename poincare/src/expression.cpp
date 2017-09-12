#include <poincare/expression.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>
#include <poincare/static_hierarchy.h>
#include <poincare/list_data.h>
#include <poincare/matrix_data.h>
#include <poincare/evaluation.h>
#include <cmath>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"
extern "C" {
#include <assert.h>
}

//#include "simplify/rules.h"

int poincare_expression_yyparse(Poincare::Expression ** expressionOutput);

namespace Poincare {

static Expression::CircuitBreaker sCircuitBreaker = nullptr;

#include <stdio.h>

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

  if (expression) {
    expression->setParentPointer(nullptr);
  }
  return expression;
}

void Expression::setCircuitBreaker(CircuitBreaker cb) {
  sCircuitBreaker = cb;
}

bool Expression::shouldStopProcessing() {
  if (sCircuitBreaker == nullptr) {
    return false;
  }
  return sCircuitBreaker();
}

bool Expression::hasValidNumberOfArguments() const {
  for (int i = 0; i < numberOfOperands(); i++) {
    if (!operand(i)->hasValidNumberOfArguments()) {
      return false;
    }
  }
  return true;
}

ExpressionLayout * Expression::createLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  switch (floatDisplayMode) {
    case FloatDisplayMode::Default:
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

void Expression::sort() {
  if (this->type() == Type::Complex) {
    // TODO: this case should be useless once complex is a leaf expression!
    return;
  }
  for (int i = 0; i < numberOfOperands(); i++) {
    ((Expression *)operand(i))->sort(); // TODO: implement an editatble operand?
  }
}

int Expression::comparesTo(const Expression * e) const {
  if (this->nodeComparesTo(e) != 0) {
    return this->nodeComparesTo(e);
  }
  for (int i = 0; i < this->numberOfOperands(); i++) {
    // The NULL node is the least node type.
    if (e->numberOfOperands() <= i) {
      return 1;
    }
    if (this->operand(i)->comparesTo(e->operand(i)) != 0) {
      return this->operand(i)->comparesTo(e->operand(i));
    }
  }
  // The NULL node is the least node type.
  if (e->numberOfOperands() > numberOfOperands()) {
    return -1;
  }
  return 0;
}

template<typename T> Evaluation<T> * Expression::evaluate(Context& context, AngleUnit angleUnit) const {
  switch (angleUnit) {
    case AngleUnit::Default:
      return privateEvaluate(T(), context, Preferences::sharedPreferences()->angleUnit());
    default:
      return privateEvaluate(T(), context, angleUnit);
  }
}

template<typename T> T Expression::approximate(Context& context, AngleUnit angleUnit) const {
  Evaluation<T> * evaluation = evaluate<T>(context, angleUnit);
  T result = evaluation->toScalar();
  delete evaluation;
  return result;
}

template<typename T> T Expression::approximate(const char * text, Context& context, AngleUnit angleUnit) {
  Expression * exp = parse(text);
  if (exp == nullptr) {
    return NAN;
  }
  Evaluation<T> * evaluation = exp->evaluate<T>(context, angleUnit);
  delete exp;
  T result = evaluation->toScalar();
  delete evaluation;
  return result;
}

/*Expression * Expression::simplify() const {
  // pre-process:
  // - remonter les noeuds de matrices en root ou les faire disparaitre
  // - division and subtraction are turned into multiplication and addition
  // - oppostive are turned into multiplication
  // - associative expression are collapsed
  // Simplify:
  // - see Romain notes
  // Post-process:
  // - pattern a+(-1)*b -> a-b
  // - pattern a*b^(-1) -> a/b
  // - pattern (-1)*a -> -a



  * We make sure that the simplification is deletable.
   * Indeed, we don't want an expression with some parts deletable and some not
   *

  // If we have a leaf node nothing can be simplified.
  if (this->numberOfOperands()==0) {
    return this->clone();
  }

  Expression * result = this->clone();
  Expression * tmp = nullptr;

  bool simplification_pass_was_useful = true;
  while (simplification_pass_was_useful) {
    * We recursively simplify the children expressions.
     * Note that we are sure to get the samne number of children as we had before
     *
    Expression ** simplifiedOperands = new Expression * [result->numberOfOperands()];
    for (int i = 0; i < result->numberOfOperands(); i++) {
      simplifiedOperands[i] = result->operand(i)->simplify();
    }

    * Note that we don't need to clone the simplified children because they are
     * already cloned before. *
    tmp = result->cloneWithDifferentOperands(simplifiedOperands, result->numberOfOperands(), false);
    delete result;
    result = tmp;

    // The table is no longer needed.
    delete [] simplifiedOperands;

    simplification_pass_was_useful = false;
    for (int i=0; i<knumberOfSimplifications; i++) {
      const Simplification * simplification = (simplifications + i); // Pointer arithmetics
      Expression * simplified = simplification->simplify(result);
      if (simplified != nullptr) {
        simplification_pass_was_useful = true;
        delete result;
        result = simplified;
        break;
      }
    }
  }

  return result;
}*/

template<typename T> T Expression::epsilon() {
  static T epsilon = sizeof(T) == sizeof(double) ? 1E-15 : 1E-7f;
  return epsilon;
}

int Expression::nodeComparesTo(const Expression * e) const {
  if (e->type() == this->type()) {
    return 0;
  }
  if (e->type() > this->type()) {
    return 1;
  }
  return -1;
}

void Expression::setParentPointer(Expression * parent) {
  if (this == parent) {
    // TODO: this case should be useless once complex is a leaf expression!
    return;
  }
  m_parent = parent;
  for (int i=0; i<numberOfOperands(); i++) {
    ((Expression *)operand(i))->setParentPointer(this);
  }
}

}

template Poincare::Evaluation<double> * Poincare::Expression::evaluate<double>(Context& context, AngleUnit angleUnit) const;
template Poincare::Evaluation<float> * Poincare::Expression::evaluate<float>(Context& context, AngleUnit angleUnit) const;
template double Poincare::Expression::approximate<double>(char const*, Poincare::Context&, Poincare::Expression::AngleUnit);
template float Poincare::Expression::approximate<float>(char const*, Poincare::Context&, Poincare::Expression::AngleUnit);
template double Poincare::Expression::approximate<double>(Poincare::Context&, Poincare::Expression::AngleUnit) const;
template float Poincare::Expression::approximate<float>(Poincare::Context&, Poincare::Expression::AngleUnit) const;
template double Poincare::Expression::epsilon<double>();
template float Poincare::Expression::epsilon<float>();
