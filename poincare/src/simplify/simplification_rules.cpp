#include "simplification_rules.h"
#include "simplification_generator.h"

const ExpressionSelector additionCommutativeWCSelector[4] = {
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 1,
  },
  {
    .m_match = ExpressionSelector::Match::WildCard,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::WildCard,
    .m_numberOfChildren = 0
  },
};

const ExpressionBuilder additionCommutativeWCBuilder[3] = {
  {
    .m_action = ExpressionBuilder::Action::BuildFromTypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
  {
    .m_action = ExpressionBuilder::Action::BringUpWildcard,
    .m_matchIndex = 2,
    .m_numberOfChildren = 0
  },
  {
    .m_action = ExpressionBuilder::Action::BringUpWildcard,
    .m_matchIndex = 3,
    .m_numberOfChildren = 0
  },
};

const ExpressionSelector multiplyCommutativeWCSelector[4] = {
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Product,
    .m_numberOfChildren = 2
  },
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Product,
    .m_numberOfChildren = 1,
  },
  {
    .m_match = ExpressionSelector::Match::WildCard,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::WildCard,
    .m_numberOfChildren = 0
  },
};

const ExpressionBuilder multiplyCommutativeWCBuilder[3] = {
  {
    .m_action = ExpressionBuilder::Action::BuildFromTypeAndValue,
    .m_expressionType = Expression::Type::Product,
    .m_numberOfChildren = 2
  },
  {
    .m_action = ExpressionBuilder::Action::BringUpWildcard,
    .m_matchIndex = 2,
    .m_numberOfChildren = 0
  },
  {
    .m_action = ExpressionBuilder::Action::BringUpWildcard,
    .m_matchIndex = 3,
    .m_numberOfChildren = 0
  },
};


const ExpressionSelector additionIntegerSelector[3] = {
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Integer,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::Type,
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

const ExpressionSelector additionIntegerWCSelector[4] = {
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 3
  },
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Integer,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::Type,
    .m_expressionType = Expression::Type::Integer,
    .m_numberOfChildren = 0
  },
  {
    .m_match = ExpressionSelector::Match::WildCard,
    .m_numberOfChildren = 0
  },
};

const ExpressionBuilder additionIntegerWCBuilder[5] = {
  {
    .m_action = ExpressionBuilder::Action::BuildFromTypeAndValue,
    .m_expressionType = Expression::Type::Addition,
    .m_numberOfChildren = 2
  },
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
  },
  {
    .m_action = ExpressionBuilder::Action::BringUpWildcard,
    .m_matchIndex = 3,
    .m_numberOfChildren = 0
  },
};

const Simplification simplifications[4] = {
  {
    .m_selector = (ExpressionSelector *)additionCommutativeWCSelector,
    .m_builder = (ExpressionBuilder *)additionCommutativeWCBuilder
  },
  {
    .m_selector = (ExpressionSelector *)multiplyCommutativeWCSelector,
    .m_builder = (ExpressionBuilder *)multiplyCommutativeWCBuilder
  },
  {
    .m_selector = (ExpressionSelector *)additionIntegerSelector,
    .m_builder = (ExpressionBuilder *)additionIntegerBuilder
  },
  {
    .m_selector = (ExpressionSelector *)additionIntegerWCSelector,
    .m_builder = (ExpressionBuilder *)additionIntegerWCBuilder
  },
};

const int knumberOfSimplifications = 4;
