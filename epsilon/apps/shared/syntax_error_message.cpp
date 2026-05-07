
#include "syntax_error_message.h"

#include <assert.h>

namespace Shared {

I18n::Message MessageForSyntaxError(Poincare::SyntaxError error) {
  switch (error) {
    case Poincare::SyntaxError::DataNotSuitable:
      return I18n::Message::DataNotSuitable;
    case Poincare::SyntaxError::EmptyMatrix:
      return I18n::Message::EmptyMatrixNotHandled;
    case Poincare::SyntaxError::EmptyVectorNotHandled:
      return I18n::Message::EmptyVectorsNotHandled;
    case Poincare::SyntaxError::ExponentTooBig:
      return I18n::Message::ExponentTooBig;
    case Poincare::SyntaxError::ForbiddenImplicitMultiplication:
      return I18n::Message::ForbiddenImplicitMultiplication;
    case Poincare::SyntaxError::FunctionAndVariableMustHaveDistinctNames:
      return I18n::Message::FunctionAndVariableMustHaveDistinctNames;
    case Poincare::SyntaxError::InvalidNumberOfParameters:
      return I18n::Message::FunctionMustHave1Or2Parameters;
    case Poincare::SyntaxError::LogBaseMustHaveOneParameter:
    case Poincare::SyntaxError::FunctionMustHaveOneParameter:
      return I18n::Message::FunctionMustHaveOnly1Parameter;
    case Poincare::SyntaxError::InvalidMatrixDimension:
      return I18n::Message::InvalidMatrixDimension;
    case Poincare::SyntaxError::EmptyParentheses:
      return I18n::Message::EmptyParentheses;
    case Poincare::SyntaxError::MissingLeftBracket:
      return I18n::Message::MissingLeftBracket;
    case Poincare::SyntaxError::MissingFactorialParameterOnLeft:
    case Poincare::SyntaxError::MissingPercentParameterOnLeft:
      return I18n::Message::MissingParameterOnTheLeft;
    case Poincare::SyntaxError::MissingParentheses:
      return I18n::Message::MissingParentheses;
    case Poincare::SyntaxError::MissingParenthesisInFunctionExponent:
      return I18n::Message::MissingParenthesisInFunctionExponent;
    case Poincare::SyntaxError::MissingRightBracket:
    case Poincare::SyntaxError::MissingClosingBrace:
      return I18n::Message::MissingRightBracket;
    case Poincare::SyntaxError::MissingRightParenthesis1:
    case Poincare::SyntaxError::MissingRightParenthesis2:
    case Poincare::SyntaxError::MissingRightParenthesis3:
      return I18n::Message::MissingRightParenthesis;
    case Poincare::SyntaxError::CannotFindSequenceIndex:
      return I18n::Message::MissingSequenceIndex;
    case Poincare::SyntaxError::RightSideMustBeSymbolOrFunction:
      return I18n::Message::RightSideMustBeSymbolOrFunction;
    case Poincare::SyntaxError::SomethingWasExpected:
    case Poincare::SyntaxError::MissingLeftExpression1:
    case Poincare::SyntaxError::MissingLeftExpression2:
    case Poincare::SyntaxError::MissingLeftExpression3:
      return I18n::Message::SomethingExpected;
    case Poincare::SyntaxError::FunctionCannotBePowered:
      return I18n::Message::ThisFunctionCantBePowered;
    case Poincare::SyntaxError::FunctionHasNoInverse:
      return I18n::Message::ThisFunctionHasNoInverse;
    case Poincare::SyntaxError::VariableNameTooLong1:
    case Poincare::SyntaxError::VariableNameTooLong2:
      return I18n::Message::VariableNameTooLong;
    case Poincare::SyntaxError::ParametricMustDependOnVariableNatural:
    case Poincare::SyntaxError::ParametricMustDependOnVariableLinear:
      return I18n::Message::ParametricMustDependOnVariable;
    case Poincare::SyntaxError::IncompleteExpression:
      return I18n::Message::IncompleteExpression;
    case Poincare::SyntaxError::CannotParsePercentOperator:
      return I18n::Message::CannotParsePercentOperator;
    case Poincare::SyntaxError::NoBooleansBeforeNot:
      return I18n::Message::NoBooleansBeforeNot;
    case Poincare::SyntaxError::TooManyParametersInParentheses:
    case Poincare::SyntaxError::Only2DPointsHandled:
      return I18n::Message::Only2DPointsHandled;
    case Poincare::SyntaxError::TooManyParameters:
      return I18n::Message::TooManyParameters;
    case Poincare::SyntaxError::TooFewParameters:
      return I18n::Message::TooFewParameters;
    case Poincare::SyntaxError::NLLogBaseShouldBeLeftOfLog:
      return I18n::Message::NLLogBaseShouldBeLeftOfLog;
    case Poincare::SyntaxError::OnlyLogShouldUsePrefixLogBase1:
    case Poincare::SyntaxError::OnlyLogShouldUsePrefixLogBase2:
      return I18n::Message::OnlyLogShouldUsePrefixLogBase;
    case Poincare::SyntaxError::NumberFollowsValidExpressionDirectly:
      return I18n::Message::NumberFollowsValidExpressionDirectly;
    case Poincare::SyntaxError::NonAcceptableExpression:
      return I18n::Message::NonAcceptableExpression;
    case Poincare::SyntaxError::ExponentCannotExistAlone:
      return I18n::Message::ExponentCannotExistAlone;
    case Poincare::SyntaxError::None:
      assert(false);
    default:
      return I18n::Message::SyntaxError;
  }
}

}  // namespace Shared
