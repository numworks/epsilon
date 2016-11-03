#ifndef POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H
#define POINCARE_SIMPLIFY_EXPRESSION_BUILDER_H

#include <poincare/expression.h>
#include "expression_match.h"
extern "C" {
#include <stdint.h>
}

class ExpressionBuilder {
public:
  static constexpr ExpressionBuilder BuildFromType(Expression::Type type, uint8_t numberOfChildren);
  static constexpr ExpressionBuilder BuildFromTypeAndValue(Expression::Type type, int32_t value, uint8_t numberOfChildren);
  static constexpr ExpressionBuilder Clone(uint8_t matchIndex, uint8_t numberOfChildren);
  static constexpr ExpressionBuilder BringUpWildcard(uint8_t matchIndex, uint8_t numberOfChildren);
  typedef Expression * (ExternalGenerator)(Expression ** parameters, int numberOfParameters);
  static constexpr ExpressionBuilder CallExternalGenerator(ExternalGenerator * generator, uint8_t numberOfChildren);
public:
  Expression * build(ExpressionMatch matches[]);

private:
  enum class Action {
    BuildFromType,
    BuildFromTypeAndValue,
    Clone,
    BringUpWildcard,
    CallExternalGenerator
  };

  constexpr ExpressionBuilder(Expression::Type type, uint8_t numberOfChildren);
  constexpr ExpressionBuilder(Expression::Type type, int32_t integerValue, uint8_t numberOfChildren);
  constexpr ExpressionBuilder(Action action, uint8_t matchIndex, uint8_t numberOfChildren);
  constexpr ExpressionBuilder(ExternalGenerator * generator, uint8_t numberOfChildren);
  ExpressionBuilder * child(int index);

  Action m_action;
  union {
    // m_action == BuildFromType and BuildFromTypeAndValue
    struct {
      Expression::Type m_expressionType;
      union {
        // m_expressionType == Integer
        int32_t m_integerValue;
        // m_expressionType == Symbol
        char m_symbolName;
      };
    };
    // m_action == Clone or BringUpWildcard
    uint8_t m_matchIndex;
    // m_action == CallExternalGenerator
    ExternalGenerator * m_generator;
  };
  uint8_t m_numberOfChildren;
};

/* Since they have to be evaluated at compile time, constexpr functions are
 * implicitely defined inline. Therefore we have to provide their implementation
 * in this header. */

constexpr ExpressionBuilder ExpressionBuilder::BuildFromType(
  Expression::Type type,
  uint8_t numberOfChildren) {
  return ExpressionBuilder(type, numberOfChildren);
}

constexpr ExpressionBuilder ExpressionBuilder::BuildFromTypeAndValue(
  Expression::Type type,
  int32_t value,
  uint8_t numberOfChildren) {
  return ExpressionBuilder(type, value, numberOfChildren);
}

constexpr ExpressionBuilder ExpressionBuilder::Clone(
  uint8_t matchIndex,
  uint8_t numberOfChildren) {
  return ExpressionBuilder(ExpressionBuilder::Action::Clone, matchIndex, numberOfChildren);
}

constexpr ExpressionBuilder ExpressionBuilder::BringUpWildcard(
  uint8_t matchIndex,
  uint8_t numberOfChildren) {
  return ExpressionBuilder(ExpressionBuilder::Action::BringUpWildcard, matchIndex, numberOfChildren);
}

constexpr ExpressionBuilder ExpressionBuilder::CallExternalGenerator(
  ExternalGenerator * generator,
  uint8_t numberOfChildren) {
  return ExpressionBuilder(generator, numberOfChildren);
}

constexpr ExpressionBuilder::ExpressionBuilder(Expression::Type type,
  uint8_t numberOfChildren)
  :
  m_action(ExpressionBuilder::Action::BuildFromType),
  m_expressionType(type),
  m_integerValue(0),
  m_numberOfChildren(numberOfChildren) {
}

constexpr ExpressionBuilder::ExpressionBuilder(Expression::Type type,
  int32_t integerValue,
  uint8_t numberOfChildren)
  :
  m_action(ExpressionBuilder::Action::BuildFromTypeAndValue),
  m_expressionType(type),
  m_integerValue(integerValue),
  m_numberOfChildren(numberOfChildren) {
}

constexpr ExpressionBuilder::ExpressionBuilder(Action action,
  uint8_t matchIndex,
  uint8_t numberOfChildren)
  :
  m_action(action),
  m_matchIndex(matchIndex),
  m_numberOfChildren(numberOfChildren) {
}

constexpr ExpressionBuilder::ExpressionBuilder(ExternalGenerator * generator,
  uint8_t numberOfChildren)
  :
  m_action(ExpressionBuilder::Action::CallExternalGenerator),
  m_generator(generator),
  m_numberOfChildren(numberOfChildren) {
}

#endif
