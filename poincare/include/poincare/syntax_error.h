#pragma once

#include <stdint.h>

namespace Poincare {

/* Error values are used in SyntaxError specifications, please do not change
 * them without updating the specifications files. */

enum class SyntaxError : uint8_t {
  None = 0,                                   // None
  Default = 1,                                // Default
  ParametricMustDependOnVariableNatural = 2,  // In natural writing format
  RightSideMustBeSymbolOrFunction = 3,
  SomethingWasExpected = 4,
  IncompleteExpression = 5,
  InvalidParenthesis = 6,
  TooManyParameters = 7,
  ExponentTooBig = 8,
  ForbiddenImplicitMultiplication = 9,
  CannotParsePercentOperator = 10,
  MissingLeftExpression1 = 11,
  BooleansNotImplemented1 = 12,
  UnitConversionFailure = 13,  // Used Internally, must be caught and handled
  NoBooleansBeforeNot = 14,
  BooleansNotImplemented2 = 15,
  MissingLeftExpression2 = 16,
  MissingLeftExpression3 = 17,
  Only2DPointsHandled = 18,
  MissingRightParenthesis1 = 19,
  MissingFactorialParameterOnLeft = 20,
  MissingPercentParameterOnLeft = 21,
  PhysicalConstantUnitsNotImplemented = 22,
  UnitNotImplemented = 23,
  ParametricMustDependOnVariableLinear = 24,  // In linear writing format
  LogBaseMustHaveOneParameter = 25,
  FunctionHasNoInverse = 26,
  FunctionCannotBePowered = 27,
  TooFewParameters = 28,
  VariableNameTooLong1 = 29,
  VariableNameTooLong2 = 30,
  MissingRightParenthesis2 = 31,
  CannotFindSequenceIndex = 32,
  FunctionMustHaveOneParameter = 33,
  FunctionAndVariableMustHaveDistinctNames = 34,
  MissingRightParenthesis3 = 35,
  MissingParenthesis = 36,
  InvalidMatrixDimension = 37,
  EmptyMatrix = 38,
  MatrixNotImplemented = 39,
  MissingLeftBracket = 40,
  EmptyVectorNotHandled = 41,
  MissingRightBracket = 42,
  MissingClosingBrace = 43,
  ListNotImplemented = 44,
  InvalidNumberOfParameters = 45,  // Should be either 1 or 2
  ExponentCannotExistAlone = 46,
  NLLogBaseShouldBeLeftOfLog = 47,
  OnlyLogShouldUsePrefixLogBase1 = 48,
  OnlyLogShouldUsePrefixLogBase2 = 49,
  MissingParenthesisInFunctionExponent = 50,
  NumberFollowsValidExpressionDirectly = 51,
  NonAcceptableExpression = 52,  // Used in Epsilon
  DataNotSuitable = 53,          // Used in Epsilon
};

}  // namespace Poincare
