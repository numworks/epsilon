#include "helper.h"
#include "token.h"

namespace Poincare {

constexpr const Expression::FunctionHelper * ParsingHelper::s_reservedFunctions[];
constexpr const char * ParsingHelper::s_specialIdentifierNames[];

const Expression::FunctionHelper * const * ParsingHelper::GetReservedFunction(const char * name, size_t nameLength) {
  const Expression::FunctionHelper * const * reservedFunction = s_reservedFunctions;
  while (reservedFunction < s_reservedFunctionsUpperBound) {
    int nameDifference = Token::CompareNonNullTerminatedName(name, nameLength, (**reservedFunction).name());
    if (nameDifference == 0) {
      return reservedFunction;
    }
    if (nameDifference < 0) {
      break;
    }
    reservedFunction++;
  }
  return nullptr;
}

bool ParsingHelper::IsSpecialIdentifierName(const char * name, size_t nameLength) {
  for (int i = 0; i < k_numberOfSpecialIdentifiers; i++) {
    int nameDifference = Token::CompareNonNullTerminatedName(name, nameLength, s_specialIdentifierNames[i]);
    if (nameDifference == 0) {
      return true;
    }
    if (nameDifference < 0) {
      break;
    }
  }
  return false;
}

bool ParsingHelper::IsParameteredExpression(const Expression::FunctionHelper * helper) {
  return helper == &Derivative::s_functionHelper
      || helper == &Integral::s_functionHelper
      || helper == &ListSequence::s_functionHelper
      || helper == &Product::s_functionHelper
      /* The string "sum" will give off the ListSum function helper. Since we call IsParameteredExpression before parsing the parameters, we cannot distinguish between the two. We make sure in parser.cpp that being considered a parametered expression does not cause problems for the parsing of ListSum. */
      || helper == &ListSum::s_functionHelper;
}

Expression ParsingHelper::ParameteredExpressionParameter(const char * text) {
  static_assert(ParameteredExpression::ParameterChildIndex() == 1, "Index of parameter in ParameteredExpression has changed");

  /* ParameteredExpression arguments are of the form "(f(t),t,..." */
  if (text[0] != UCodePointLeftSystemParenthesis.getChar() && text[0] != '(') {
    return Expression();
  }

  /* Find the first comma, the beginning of the parameter. Count parenthesis to
   * avoid stopping on a comma from a nested expression. */
  int unmatchedLeftParenthesis = 0;
  do {
    text += 1;
    if (text[0] == '\0') {
      return Expression();
    } else if (text[0] == '(' || text[0] == UCodePointLeftSystemParenthesis.getChar() || text[0] == '{') {
      unmatchedLeftParenthesis++;
    } else if (text[0] == ')' || text[0] == UCodePointRightSystemParenthesis.getChar() || text[0] == '}') {
      if (unmatchedLeftParenthesis > 0) {
        unmatchedLeftParenthesis--;
      } else {
        return Expression();
      }
    }
  } while (text[0] != ',' || unmatchedLeftParenthesis > 0);

  /* Skip the left system parenthesis that may be present at the beginning of
   * the parameter name. */
  text += 1 + (text[1] == UCodePointLeftSystemParenthesis.getChar());

  /* Find the second comma, the end of the parameter. */
  size_t parameterLength = 0;
  while (text[parameterLength] != ',') {
    if (text[parameterLength] == '\0') {
      return Expression();
    }
    parameterLength++;
  }
  parameterLength -= text[parameterLength - 1] == UCodePointRightSystemParenthesis.getChar();

  return Symbol::Builder(text, parameterLength);
}

}
