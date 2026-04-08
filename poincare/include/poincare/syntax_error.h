#pragma once

#include <stdint.h>

namespace Poincare {

/* Error values are used in SyntaxError specifications, please do not change
 * them without updating the specifications files. */

enum class SyntaxError : uint8_t {
  None = 0,     // None
  Default = 1,  // Default
  ParametricMustDependOnVariableNatural =
      2,  // Parametric must depend on a variable (natural)
  RightSideMustBeSymbolOrFunction = 3,  // Right side must be symbol or function
  SomethingWasExpected = 4,             // Something was expected
  IncompleteExpression = 5,             // Incomplete expression
  InvalidParenthesis = 6,               // Invalid use of parenthesis
  // Unused = 7,
  ExponentTooBig = 8,                   // Exponent is too big
  ForbiddenImplicitMultiplication = 9,  // Forbidden implicit multiplication
  CannotParsePercentOperator = 10,      // Can't parse percent operator
  MissingLeftExpression1 = 11,          // Missing left expression 1
  BooleansNotImplemented1 = 12,         // Boolean are not implemented 1
  InternalUnitConversion = 13,   // Internal (trying to parse unit conversion)
  NoBooleansBeforeNot = 14,      // No booleans before not
  BooleansNotImplemented2 = 15,  // Boolean are not implemented 2
  MissingLeftExpression2 = 16,   // Missing left expression 2
  MissingLeftExpression3 = 17,   // Missing left expression 3
  OnlyTwoDimensionalPointsHandled = 18,  // Only 2D points are handled
  MissingRightParenthesis1 = 19,         // Missing right parenthesis 1
  MissingFactorialParameterOnLeft = 20,  // Missing ! parameter on the left
  MissingPercentParameterOnLeft = 21,    // Missing % parameter on the left
  PhysicalConstantUnitsNotImplemented =
      22,                   // Units are not implemented (physical constant)
  UnitNotImplemented = 23,  // Units are not implemented (unit)
  ParametricMustDependOnVariableLinear =
      24,  // Parametric must depend on a variable (linear)
  LogBaseMustHaveOneParameter = 25,  // LogBase must have only 1 parameter
  FunctionHasNoInverse = 26,         // This function has no inverse
  FunctionCannotBePowered = 27,      // This function can't be powered
  TooFewOrTooManyParameters = 28,    // Too few or too many parameters provided.
  VariableNameTooLong1 = 29,         // Variable name is too long 1
  VariableNameTooLong2 = 30,         // Variable name is too long 2
  MissingRightParenthesis2 = 31,     // Missing right parenthesis 2
  CannotFindSequenceIndex = 32,      // Cannot find the index of the sequence
  FunctionMustHaveOneParameter = 33,  // Function only have 1 parameter
  FunctionAndVariableMustHaveDistinctNames =
      34,  // Function and variable must have distinct names
  MissingRightParenthesis3 = 35,  // Missing right parenthesis 3
  MissingParenthesis = 36,        // Missing parenthesis
  InvalidMatrixDimension = 37,    // Invalid matrix dimension
  EmptyMatrix = 38,               // Empty matrix
  MatrixNotImplemented = 39,      // Matrix not implemented
  MissingLeftBracket = 40,        // Left bracket missing
  EmptyVectorNotHandled = 41,     // Empty vectors are not handled
  MissingRightBracket = 42,       // Right bracket missing
  MissingClosingBrace = 43,       // Missing }
  ListNotImplemented = 44,        // List not implemented
  InvalidNumberOfParameters =
      45,  // Invalid number of parameters, should be either 1 or 2
  ExponentCannotExistAlone =
      46,  // Syntax error // An exponent cannot exist on its own.
  LogBaseShouldBeLeftOfLog = 47,  // NL Log base should be left of log
  OnlyLogShouldUsePrefixLogBase1 =
      48,  // Only log should use the prefix log base 1
  OnlyLogShouldUsePrefixLogBase2 =
      49,  // Only log should use the prefix log base 2
  MissingParenthesisInFunctionExponent =
      50,  // Missing parenthesis in function exponent
};

}  // namespace Poincare
