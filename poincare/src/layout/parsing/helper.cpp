#include "helper.h"

#include <omg/unicode_helper.h>
#include <omg/utf8_helper.h>
#include <poincare/src/expression/binary.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/trigonometry.h>

#include "tokenizer.h"

namespace Poincare::Internal {

bool ParsingHelper::IsLogicalOperator(LayoutSpan name,
                                      Token::Type* returnType) {
#if POINCARE_BOOLEAN
  if (CompareLayoutSpanWithNullTerminatedString(
          name, Binary::OperatorName(Type::LogicalNot)) == 0) {
    *returnType = Token::Type::Not;
    return true;
  }
  EnabledType operatorType;
  if (Binary::IsBinaryLogicalOperator(name, &operatorType)) {
    switch (operatorType) {
      case Type::LogicalAnd:
        *returnType = Token::Type::And;
        break;
      case Type::LogicalOr:
        *returnType = Token::Type::Or;
        break;
      case Type::LogicalXor:
        *returnType = Token::Type::Xor;
        break;
      case Type::LogicalNand:
        *returnType = Token::Type::Nand;
        break;
      case Type::LogicalNor:
        *returnType = Token::Type::Nor;
        break;
      default:
        assert(false);
    }
    return true;
  }
#endif
  return false;
}

bool ParsingHelper::ExtractInteger(const Tree* e, int* value) {
  bool isOpposite = false;
  if (e->isOpposite()) {
    e = e->child(0);
    isOpposite = true;
  }
  if (e->isRational()) {
    IntegerHandler intHandler = Integer::Handler(e);
    if (intHandler.is<int>()) {
      *value = intHandler.to<int>() * (isOpposite ? -1 : 1);
      return true;
    }
  }
  return false;
}

const Builtin* ParsingHelper::GetInverseFunction(const Builtin* builtin) {
  if (builtin->type().isDirectTrigonometryFunction()) {
    return Builtin::GetReservedFunction(
        Trigonometry::GetInverseType(builtin->type()));
  }
  return nullptr;
}

bool ParsingHelper::IsPowerableFunction(const Builtin* builtin) {
  // return TypeBlock::IsAnyTrigonometryFunction(builtin->type());
  return builtin->type().isAnyTrigonometryFunction();
}

bool ParsingHelper::ParameterText(UnicodeDecoder& varDecoder,
                                  size_t* parameterStart,
                                  size_t* parameterLength) {
  static_assert(k_indexOfParameter1D == 1,
                "ParameteredExpression::ParameterText is outdated");
  /* Find the beginning of the parameter. Count parentheses to handle the
   * presence of functions with several parameters in the parametered
   * expression. */
  CodePoint c = UCodePointUnknown;
  bool variableFound = false;
  int cursorLevel = 0;
  while (c != UCodePointNull && cursorLevel >= 0 && !variableFound) {
    c = varDecoder.nextCodePoint();
    switch (c) {
      case '(':
      case '{':
      case '[':
        cursorLevel++;
        break;
      case ')':
      case '}':
      case ']':
        cursorLevel--;
        break;
      case ',':
        // The parameter is the second argument of parametered expressions
        variableFound = cursorLevel == 0;
        break;
    }
  }
  if (!variableFound || c == UCodePointNull) {
    return false;
  }

  size_t startOfVariable = varDecoder.position();
  c = varDecoder.nextCodePoint();
  CodePoint previousC = UCodePointUnknown;
  while (c != UCodePointNull && c != ',' && c != ')') {
    previousC = c;
    c = varDecoder.nextCodePoint();
  }
  size_t endOfVariable = varDecoder.position();
  varDecoder.unsafeSetPosition(startOfVariable);
  do {
    // Skip whitespaces
    c = varDecoder.nextCodePoint();
  } while (c == ' ');
  varDecoder.previousCodePoint();
  startOfVariable = varDecoder.position();
  size_t lengthOfVariable = endOfVariable - startOfVariable - 1;

  if (!Tokenizer::CanBeCustomIdentifier(varDecoder, lengthOfVariable)) {
    return false;
  }
  varDecoder.unsafeSetPosition(startOfVariable);
  *parameterLength = lengthOfVariable;
  *parameterStart = startOfVariable;
  return true;
}

// TODO: merge with ParameterText(UnicodeDecoder&) defined above
bool ParsingHelper::ParameterText(LayoutSpanDecoder* varDecoder,
                                  const Layout** parameterStart,
                                  size_t* parameterLength) {
  static_assert(k_indexOfParameter1D == 1,
                "ParameteredExpression::ParameterText is outdated");
  /* Find the beginning of the parameter. Count parentheses to handle the
   * presence of functions with several parameters in the parametered
   * expression. */
  CodePoint c = UCodePointUnknown;
  bool variableFound = false;
  int cursorLevel = 0;
  while (c != UCodePointNull && cursorLevel >= 0 && !variableFound) {
    c = varDecoder->nextCodePoint();
    switch (c) {
      case '(':
      case '{':
      case '[':
        cursorLevel++;
        break;
      case ')':
      case '}':
      case ']':
        cursorLevel--;
        break;
      case ',':
        // The parameter is the second argument of parametered expressions
        variableFound = cursorLevel == 0;
        break;
    }
  }
  if (!variableFound || c == UCodePointNull) {
    return false;
  }

  LayoutSpanDecoder startOfVariable = *varDecoder;
  c = varDecoder->nextCodePoint();
  CodePoint previousC = UCodePointUnknown;
  while (c != UCodePointNull && c != ',' && c != ')') {
    previousC = c;
    c = varDecoder->nextCodePoint();
  }
  const Tree* endOfVariable = varDecoder->layout();
  *varDecoder = startOfVariable;
  // Skip whitespaces
  while (varDecoder->codePoint() == ' ') {
    c = varDecoder->nextCodePoint();
  }
  startOfVariable = *varDecoder;
  size_t lengthOfVariable =
      NumberOfNextTreeTo(startOfVariable.layout(), endOfVariable) - 1;

  // if (!Tokenizer::CanBeCustomIdentifier(varDecoder, lengthOfVariable)) {
  // return false;
  // }
  *varDecoder = startOfVariable;
  *parameterLength = lengthOfVariable;
  *parameterStart = startOfVariable.layout();
  return true;
}

}  // namespace Poincare::Internal
