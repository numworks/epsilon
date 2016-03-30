#include "simplification_rules.h"
#include "simplification_generator.h"

const ExpressionSelector additionCommutativeSelector[5] = {
  {
    .m_match = ExpressionSelector::Match::TypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
  {
    .m_match = ExpressionSelector::Match::Any,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::TypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2,
  },
  {
    .m_match = ExpressionSelector::Match::Any,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::Any,
    .m_numberOfChildren = 0
  }
};

const ExpressionBuilder additionCommutativeBuilder[4] = {
  {
    .m_action = ExpressionBuilder::Action::BuildFromTypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 3
  },
  {
    .m_action = ExpressionBuilder::Action::Clone,
    .m_matchIndex = 1,
    .m_numberOfChildren = 0
  },
  {
    .m_action = ExpressionBuilder::Action::Clone,
    .m_matchIndex = 3,
    .m_numberOfChildren = 0
  },
  {
    .m_action = ExpressionBuilder::Action::Clone,
    .m_matchIndex = 4,
    .m_numberOfChildren = 0
  }
};

const ExpressionSelector additionIntegerSelector[3] = {
  {
    .m_match = ExpressionSelector::Match::TypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
  {
    .m_match = ExpressionSelector::Match::TypeAndValue,
    .m_expressionType = Expression::Type::Integer,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::TypeAndValue,
    .m_expressionType = Expression::Type::Integer,
    .m_numberOfChildren = 0
  }
};

const ExpressionBuilder additionIntegerBuilder[3] = {
  {
    .m_action = ExpressionBuilder::Action::CallExternalGenerator,
    .m_generator = &SimplificationGenerator::AddIntegers,
    .m_numberOfChildren = 2
  },
  {
    .m_action = ExpressionBuilder::Action::Clone,
    .m_matchIndex = 1,
    .m_numberOfChildren = 0
  },
  {
    .m_action = ExpressionBuilder::Action::Clone,
    .m_matchIndex = 2,
    .m_numberOfChildren = 0
  }
};

const Simplification simplifications[2] = {
  {
    .m_selector = (ExpressionSelector *)additionCommutativeSelector,
    .m_builder = (ExpressionBuilder *)additionCommutativeBuilder
  },
  {
    .m_selector = (ExpressionSelector *)additionIntegerSelector,
    .m_builder = (ExpressionBuilder *)additionIntegerBuilder
  }
};

const int knumberOfSimplifications = 2;
