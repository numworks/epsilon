#include "rack_parser.h"

#include <ion/unicode/utf8_decoder.h>
#include <omgpj/unicode_helper.h>
#include <poincare/empty_context.h>
#include <poincare/symbol_abstract.h>
#include <poincare/variable_context.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/binary.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/integer.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/list.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/expression/unit.h>
#include <poincare_junior/src/layout/parser.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>
#include <stdlib.h>

#include <algorithm>
#include <utility>

// #include "helper.h"

namespace PoincareJ {

Tree *RackParser::parse() {
  ExceptionTry {
    for (int i = 0; const Tree *child : m_root->children()) {
      if (child->treeIsIdenticalTo(KCodePointL<UCodePointRightwardsArrow>())) {
        return parseExpressionWithRightwardsArrow(i);
      }
      i++;
    }
    Tree *result = initializeFirstTokenAndParseUntilEnd();
    // Only 1 tree has been created.
    assert(result &&
           result->nextTree()->block() == SharedEditionPool->lastBlock());
    return result;
  }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      ExceptionCheckpoint::Raise(type);
    }
    return nullptr;
  }
}

static inline void turnIntoBinaryNode(const Tree *node,
                                      EditionReference &leftHandSide,
                                      EditionReference &rightHandSide) {
  assert(leftHandSide->nextTree() == static_cast<Tree *>(rightHandSide));
  CloneNodeAtNode(leftHandSide, node);
}

Tree *RackParser::parseExpressionWithRightwardsArrow(
    size_t rightwardsArrowPosition) {
  /* If the string contains an arrow, try to parse as unit conversion first.
   * We have to do this here because the parsing of the leftSide and the one
   * of the rightSide are both impacted by the fact that it is a unitConversion
   *
   * Example: if you stored 5 in the variable m, "3m" is understood as "3*5"
   * in the expression "3m->x", but it's understood as "3 meters" in the
   * expression "3m->km".
   *
   * If the parsing of unit conversion fails, retry with but this time, parse
   * right side of expression first.
   *
   * Even undefined function "plouf(x)" should be interpreted as function and
   * not as a multiplication. This is done by setting the parsingMethod to
   * Assignment (see Parser::popToken())
   *
   * We parse right side before left to ensure that:
   * - 4m->f(m) is understood as 4x->f(x)
   *   but 4m->x is understood as 4meters->x
   * - abc->f(abc) is understood as x->f(x)
   *   but abc->x is understood as a*b*c->x
   * */

  // Step 1. Parse as unitConversion
  m_parsingContext.setParsingMethod(
      ParsingContext::ParsingMethod::UnitConversion);
  State previousState = currentState();
  ExceptionTry { return initializeFirstTokenAndParseUntilEnd(); }
  ExceptionCatch(type) {
    if (type != ExceptionType::ParseFail) {
      ExceptionCheckpoint::Raise(type);
    }

    // Failed to parse as unit conversion
    setState(previousState);
  }

  // Step 2. Parse as assignment, starting with rightHandSide.
  m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Assignment);
  m_tokenizer.goToPosition(rightwardsArrowPosition + 1);
  EditionReference rightHandSide = initializeFirstTokenAndParseUntilEnd();
  if (m_nextToken.is(Token::Type::EndOfStream) &&
      !rightHandSide.isUninitialized() &&
      (rightHandSide
           ->isUserSymbol() ||  // RightHandSide must be symbol or function.
       (rightHandSide->isUserFunction() &&
        rightHandSide->child(0)->isUserSymbol()))) {
    setState(previousState);
    m_parsingContext.setParsingMethod(ParsingContext::ParsingMethod::Classic);
    Poincare::EmptyContext tempContext = Poincare::EmptyContext();
    // This is instantiated outside the condition so that the pointer is not
    // lost.
    Poincare::VariableContext assignmentContext("", &tempContext);
    if (rightHandSide->isUserFunction() && m_parsingContext.context()) {
      /* If assigning a function, set the function parameter in the context
       * for parsing leftHandSide.
       * This is to ensure that 3g->f(g) is correctly parsed */
      EditionReference functionParameter = rightHandSide->child(0);
      assignmentContext = Poincare::VariableContext(
          Symbol::GetName(functionParameter), m_parsingContext.context());
      m_parsingContext.setContext(&assignmentContext);
    }
    // Parse leftHandSide
    m_nextToken = m_tokenizer.popToken();
    EditionReference leftHandSide = parseUntil(Token::Type::RightwardsArrow);
    leftHandSide->swapWithTree(rightHandSide);
    turnIntoBinaryNode(KStore, leftHandSide, rightHandSide);
    return leftHandSide;
  }
  ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
}

Tree *RackParser::initializeFirstTokenAndParseUntilEnd() {
  m_nextToken = m_tokenizer.popToken();
  EditionReference result;
  if (m_parsingContext.parsingMethod() ==
      ParsingContext::ParsingMethod::CommaSeparatedList) {
    result = parseCommaSeparatedList(true);
  } else {
    result = parseUntil(Token::Type::EndOfStream);
  }
  return result;
}
// Private

Tree *RackParser::parseUntil(Token::Type stoppingType,
                             EditionReference leftHandSide) {
  typedef void (RackParser::*TokenParser)(EditionReference & leftHandSide,
                                          Token::Type stoppingType);
  constexpr static TokenParser tokenParsers[] = {
      &RackParser::parseUnexpected,          // Token::Type::EndOfStream
      &RackParser::parseRightwardsArrow,     // Token::Type::RightwardsArrow
      &RackParser::parseAssignmentEqual,     // Token::Type::AssignmentEqual
      &RackParser::parseUnexpected,          // Token::Type::RightBracket
      &RackParser::parseUnexpected,          // Token::Type::RightParenthesis
      &RackParser::parseUnexpected,          // Token::Type::RightBrace
      &RackParser::parseUnexpected,          // Token::Type::Comma
      &RackParser::parseNorOperator,         // Token::Type::Nor
      &RackParser::parseXorOperator,         // Token::Type::Xor
      &RackParser::parseOrOperator,          // Token::Type::Or
      &RackParser::parseNandOperator,        // Token::Type::Nand
      &RackParser::parseAndOperator,         // Token::Type::And
      &RackParser::parseLogicalOperatorNot,  // Token::Type::Not
      &RackParser::parseComparisonOperator,  // Token::Type::ComparisonOperator
      &RackParser::parseNorthEastArrow,      // Token::Type::NorthEastArrow
      &RackParser::parseSouthEastArrow,      // Token::Type::SouthEastArrow
      &RackParser::parsePlus,                // Token::Type::Plus
      &RackParser::parseMinus,               // Token::Type::Minus
      &RackParser::parseTimes,               // Token::Type::Times
      &RackParser::parseSlash,               // Token::Type::Slash
      &RackParser::parseImplicitTimes,       // Token::Type::ImplicitTimes
      &RackParser::parsePercent,             // Token::Type::Percent
      &RackParser::parseCaret,               // Token::Type::Caret
      &RackParser::parseBang,                // Token::Type::Bang
      &RackParser::
          parseImplicitAdditionBetweenUnits,  // Token::Type::ImplicitAdditionBetweenUnits
      &RackParser::parseMatrix,               // Token::Type::LeftBracket
      &RackParser::parseLeftParenthesis,    // Token::Type::LeftParenthesis
      &RackParser::parseList,               // Token::Type::LeftBrace
      &RackParser::parseConstant,           // Token::Type::Constant
      &RackParser::parseNumber,             // Token::Type::Number
      &RackParser::parseNumber,             // Token::Type::BinaryNumber
      &RackParser::parseNumber,             // Token::Type::HexadecimalNumber
      &RackParser::parseUnit,               // Token::Type::Unit
      &RackParser::parseReservedFunction,   // Token::Type::ReservedFunction
      &RackParser::parseSpecialIdentifier,  // Token::Type::SpecialIdentifier
      &RackParser::parseCustomIdentifier,   // Token::Type::CustomIdentifier
      &RackParser::parseLayout,             // Token::Type::Layout
      &RackParser::parseUnexpected,         // Token::Type::Subscript
      &RackParser::parseSuperscript,        // Token::Type::Superscript
      &RackParser::parsePrefixSuperscript,  // Token::Type::PrefixSuperscript
      &RackParser::parseUnexpected          // Token::Type::Undefined
  };
#define assert_order(token, function)                                 \
  static_assert(                                                      \
      &RackParser::function == tokenParsers[static_cast<int>(token)], \
      "Wrong order of TokenParsers");
  assert_order(Token::Type::EndOfStream, parseUnexpected);
  assert_order(Token::Type::RightwardsArrow, parseRightwardsArrow);
  assert_order(Token::Type::RightBracket, parseUnexpected);
  assert_order(Token::Type::RightParenthesis, parseUnexpected);
  assert_order(Token::Type::RightBrace, parseUnexpected);
  assert_order(Token::Type::Comma, parseUnexpected);
  assert_order(Token::Type::Nor, parseNorOperator);
  assert_order(Token::Type::Xor, parseXorOperator);
  assert_order(Token::Type::Or, parseOrOperator);
  assert_order(Token::Type::Nand, parseNandOperator);
  assert_order(Token::Type::And, parseAndOperator);
  assert_order(Token::Type::Not, parseLogicalOperatorNot);
  assert_order(Token::Type::ComparisonOperator, parseComparisonOperator);
  assert_order(Token::Type::NorthEastArrow, parseNorthEastArrow);
  assert_order(Token::Type::SouthEastArrow, parseSouthEastArrow);
  assert_order(Token::Type::Plus, parsePlus);
  assert_order(Token::Type::Minus, parseMinus);
  assert_order(Token::Type::Times, parseTimes);
  assert_order(Token::Type::Slash, parseSlash);
  assert_order(Token::Type::ImplicitTimes, parseImplicitTimes);
  assert_order(Token::Type::Percent, parsePercent);
  assert_order(Token::Type::Caret, parseCaret);
  assert_order(Token::Type::Bang, parseBang);
  assert_order(Token::Type::ImplicitAdditionBetweenUnits,
               parseImplicitAdditionBetweenUnits);
  assert_order(Token::Type::LeftBracket, parseMatrix);
  assert_order(Token::Type::LeftParenthesis, parseLeftParenthesis);
  assert_order(Token::Type::LeftBrace, parseList);
  assert_order(Token::Type::Constant, parseConstant);
  assert_order(Token::Type::Number, parseNumber);
  assert_order(Token::Type::BinaryNumber, parseNumber);
  assert_order(Token::Type::HexadecimalNumber, parseNumber);
  assert_order(Token::Type::Unit, parseUnit);
  assert_order(Token::Type::ReservedFunction, parseReservedFunction);
  assert_order(Token::Type::SpecialIdentifier, parseSpecialIdentifier);
  assert_order(Token::Type::CustomIdentifier, parseCustomIdentifier);
  assert_order(Token::Type::Layout, parseLayout);
  assert_order(Token::Type::Subscript, parseUnexpected);
  assert_order(Token::Type::Superscript, parseSuperscript);
  assert_order(Token::Type::PrefixSuperscript, parsePrefixSuperscript);
  assert_order(Token::Type::Undefined, parseUnexpected);

  do {
    popToken();
    (this->*(tokenParsers[static_cast<int>(m_currentToken.type())]))(
        leftHandSide, stoppingType);
  } while (nextTokenHasPrecedenceOver(stoppingType));
  return leftHandSide;
}

void RackParser::popToken() {
  if (m_pendingImplicitOperator) {
    m_currentToken = Token(implicitOperatorType());
    m_pendingImplicitOperator = false;
  } else {
    m_currentToken = m_nextToken;
    if (m_currentToken.is(Token::Type::EndOfStream)) {
      /* Avoid reading out of buffer (calling popToken would read the character
       * after EndOfStream) */
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    } else {
      m_nextToken = m_tokenizer.popToken();
    }
  }
}

bool RackParser::popTokenIfType(Token::Type type) {
  /* The method called with the Token::Types
   * (Left and Right) Braces, Bracket, Parenthesis and Comma.
   * Never with Token::Type::ImplicitTimes.
   * If this assumption is not satisfied anymore, change the following to handle
   * ImplicitTimes. */
  assert(type != Token::Type::ImplicitTimes && !m_pendingImplicitOperator);
  bool tokenTypesCoincide = m_nextToken.is(type);
  if (tokenTypesCoincide) {
    popToken();
  }
  return tokenTypesCoincide;
}

bool RackParser::nextTokenHasPrecedenceOver(Token::Type stoppingType) {
  Token::Type nextTokenType =
      (m_pendingImplicitOperator) ? implicitOperatorType() : m_nextToken.type();
  if (m_waitingSlashForMixedFraction && nextTokenType == Token::Type::Slash) {
    /* When parsing a mixed fraction, we cannot parse until a token type
     * with lower precedence than slash, but we still need not to stop on the
     * middle slash.
     * Ex:
     * 1 2/3/4 = (1 2/3)/4
     * 1 2/3^2 = (1 2/3)^2 */
    m_waitingSlashForMixedFraction = false;
    return true;
  }
  return nextTokenType > stoppingType;
}

void RackParser::isThereImplicitOperator() {
  /* This function is called at the end of
   * parseNumber, parseSpecialIdentifier, parseReservedFunction, parseUnit,
   * parseFactorial, parseMatrix, parseLeftParenthesis, parseCustomIdentifier
   * in order to check whether it should be followed by a
   * Token::Type::ImplicitTimes. In that case, m_pendingImplicitOperator is set
   * to true, so that popToken, popTokenIfType, nextTokenHasPrecedenceOver can
   * handle implicit multiplication. */
  m_pendingImplicitOperator =
      ((m_nextToken.is(Token::Type::Layout) &&
        m_nextToken.firstLayout()->layoutType() !=
            LayoutType::VerticalOffset) ||
       m_nextToken.is(Token::Type::Number) ||
       m_nextToken.is(Token::Type::Constant) ||
       m_nextToken.is(Token::Type::Unit) ||
       m_nextToken.is(Token::Type::ReservedFunction) ||
       m_nextToken.is(Token::Type::SpecialIdentifier) ||
       m_nextToken.is(Token::Type::CustomIdentifier) ||
       m_nextToken.is(Token::Type::LeftParenthesis) ||
       m_nextToken.is(Token::Type::LeftBracket) ||
       m_nextToken.is(Token::Type::LeftBrace) ||
       m_nextToken.is(Token::Type::ImplicitAdditionBetweenUnits));
}

Token::Type RackParser::implicitOperatorType() {
  return m_parsingContext.parsingMethod() == ParsingContext::ParsingMethod::
                                                 ImplicitAdditionBetweenUnits &&
                 m_currentToken.type() == Token::Type::Unit
             ? Token::Type::Plus
             : Token::Type::ImplicitTimes;
}

void RackParser::parseUnexpected(EditionReference &leftHandSide,
                                 Token::Type stoppingType) {
  ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
}

void RackParser::parseNumber(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  /* TODO: RackLayoutDecoder could be implemented without mainLayout, start,
           m_root and parentOfDescendant() call wouldn't be needed. */
  int start = 0;
  const Tree *rack =
      m_root->parentOfDescendant(m_currentToken.firstLayout(), &start);
  size_t end = start + m_currentToken.length();
  OMG::Base base(OMG::Base::Decimal);
  if (m_currentToken.type() == Token::Type::HexadecimalNumber ||
      m_currentToken.type() == Token::Type::BinaryNumber) {
    start += 2;  // Skip 0b / 0x prefix
    base = m_currentToken.type() == Token::Type::HexadecimalNumber
               ? OMG::Base::Hexadecimal
               : OMG::Base::Binary;
    RackLayoutDecoder decoder(rack, start, end);
    leftHandSide = Integer::Push(decoder, base);
  } else {
    // the tokenizer have already ensured the float is syntactically correct
    RackLayoutDecoder decoder(rack, start, end);
    size_t decimalPoint = OMG::CodePointSearch(&decoder, '.');
    if (decimalPoint == end) {
      /* continue with the same decoder since E should be after the decimal
       * point, except when there is no point */
      decoder.setPosition(start);
    }
    size_t smallE =
        OMG::CodePointSearch(&decoder, UCodePointLatinLetterSmallCapitalE);

    RackLayoutDecoder integerDigits(rack, start,
                                    std::min(smallE, decimalPoint));
    RackLayoutDecoder fractionalDigits(rack, decimalPoint + 1, smallE);
    RackLayoutDecoder exponent(rack, smallE + 1,
                               end);  // TODO may have a minus sign

    if (decimalPoint == end || smallE == decimalPoint + 1) {
      // Decimal integer
      leftHandSide = Integer::Push(integerDigits, OMG::Base::Decimal);
    } else {
      int offset = smallE - decimalPoint - 1;
      assert(offset > 0);
      // Decimal<offset>(integerDigits * 10^offset + fractionalDigits)
      leftHandSide = SharedEditionPool->push<Type::Decimal, int8_t>(offset);
      Tree *child =
          IntegerHandler::Power(IntegerHandler(10), IntegerHandler(offset));
      child->moveTreeOverTree(IntegerHandler::Multiplication(
          Integer::Handler(child), IntegerHandler::Parse(integerDigits, base)));
      child->moveTreeOverTree(IntegerHandler::Addition(
          Integer::Handler(child),
          IntegerHandler::Parse(fractionalDigits, base)));
    }
    if (smallE != end) {
      // Decimal * 10^exponent
      Tree *mult = SharedEditionPool->push<Type::Multiplication>(1);
      SharedEditionPool->push(Type::Power);
      (10_e)->clone();
      Integer::Push(exponent, base);
      leftHandSide->moveTreeAtNode(mult);
      NAry::SetNumberOfChildren(leftHandSide, 2);
    }
  }

  if (generateMixedFractionIfNeeded(leftHandSide)) {
    return;
  }
  if (m_nextToken.isNumber()  // No implicit multiplication between two numbers
                              // No implicit multiplication between a
                              // hexadecimal number and an identifier (avoid
                              // parsing 0x2abch as 0x2ABC*h)
      || (m_currentToken.is(Token::Type::HexadecimalNumber) &&
          (m_nextToken.is(Token::Type::CustomIdentifier) ||
           m_nextToken.is(Token::Type::SpecialIdentifier) ||
           m_nextToken.is(Token::Type::ReservedFunction)))) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
}

void RackParser::parsePlus(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, true, stoppingType);
}

void RackParser::parseMinus(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  privateParsePlusAndMinus(leftHandSide, false, stoppingType);
}

void RackParser::privateParsePlusAndMinus(EditionReference &leftHandSide,
                                          bool plus, Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // +2 = 2, -2 = -2
    leftHandSide = parseUntil(std::max(stoppingType, Token::Type::Minus));
    if (!plus) {
      CloneNodeAtNode(leftHandSide, KOpposite);
    }
    return;
  }
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, plus)) {
    return;
  }
  assert(leftHandSide->nextTree() == static_cast<Tree *>(rightHandSide));
  if (!plus) {
    CloneNodeAtNode(leftHandSide, KTree<Type::Subtraction>());
    return;
  }
  if (leftHandSide->isAddition()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + 1);
  } else {
    CloneNodeAtNode(leftHandSide, KAdd.node<2>);
  }
}

void RackParser::parseNorthEastArrow(EditionReference &leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, true, stoppingType);
}

void RackParser::parseSouthEastArrow(EditionReference &leftHandSide,
                                     Token::Type stoppingType) {
  privateParseEastArrow(leftHandSide, false, stoppingType);
}

void RackParser::privateParseEastArrow(EditionReference &leftHandSide,
                                       bool north, Token::Type stoppingType) {
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Minus);
  if (!mergeIntoPercentAdditionIfNeeded(leftHandSide, rightHandSide, north)) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
}

bool RackParser::mergeIntoPercentAdditionIfNeeded(
    EditionReference &leftHandSide, EditionReference &rightHandSide,
    bool north) {
  /* The condition checks if the percent does not contain a percent because
   * "4+3%%" should be parsed as "4+((3/100)/100)" rather than "4↗0.03%" */
  if (!rightHandSide->isPercentSimple() ||
      rightHandSide->child(0)->isPercentSimple()) {
    return false;
  }
  CloneNodeAtNode(leftHandSide, KPercentAddition);
  MoveNodeOverNode(rightHandSide, rightHandSide->child(0));
  if (!north) {
    CloneNodeAtNode(rightHandSide, KOpposite);
  }
  return true;
}

void RackParser::parseTimes(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::Times);
}

void RackParser::parseImplicitTimes(EditionReference &leftHandSide,
                                    Token::Type stoppingType) {
  privateParseTimes(leftHandSide, Token::Type::ImplicitTimes);
}

void RackParser::parseImplicitAdditionBetweenUnits(
    EditionReference &leftHandSide, Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  assert(m_parsingContext.parsingMethod() !=
         ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  /* We parse the string again, but this time with
   * ParsingMethod::ImplicitAdditionBetweenUnits. */
#if 0
  // TODO_PCJ we need to be able to set the initial parsing position for this
  Parser p(m_currentToken.text(), m_parsingContext.context(),
           m_currentToken.text() + m_currentToken.length(),
           ParsingContext::ParsingMethod::ImplicitAdditionBetweenUnits);
  leftHandSide = p.parse();
  if (leftHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
#else
  ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
#endif
}

void RackParser::parseSlash(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::Slash);
  CloneNodeAtNode(leftHandSide, KDiv);
}

void RackParser::privateParseTimes(EditionReference &leftHandSide,
                                   Token::Type stoppingType) {
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, stoppingType);
  if (leftHandSide->isMultiplication()) {
    NAry::SetNumberOfChildren(leftHandSide,
                              leftHandSide->numberOfChildren() + 1);
  } else {
    CloneNodeAtNode(leftHandSide, KMult.node<2>);
  }
}

void RackParser::parseCaret(EditionReference &leftHandSide,
                            Token::Type stoppingType) {
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide, Token::Type::ImplicitTimes);
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
}

void RackParser::parseComparisonOperator(EditionReference &leftHandSide,
                                         Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Comparison operator must have a left operand
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  EditionReference rightHandSide;
  Type operatorType;
  size_t operatorLength;
  bool check = Binary::IsComparisonOperatorString(
      reinterpret_cast<const CPL *>(m_currentToken.firstLayout()),
      m_currentToken.length(), &operatorType, &operatorLength);
  assert(check);
  assert(m_currentToken.length() == operatorLength);
  (void)check;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::ComparisonOperator);
  if (leftHandSide->isComparison()) {
    /* TODO a < b = c was parsed in Comparison[<,=](a,b,c)
     * It is now parsed as (a < b and b = c) to simplify code. */
    /* TODO PCJ: fix code with a < b < c < d */
    CloneTreeAtNode(rightHandSide, leftHandSide->child(1));
    Tree *comparison = SharedEditionPool->push(operatorType);
    MoveNodeAtNode(rightHandSide, comparison);
    CloneNodeAtNode(leftHandSide, KLogicalAnd);
  } else {
    Tree *comparison = SharedEditionPool->push(operatorType);
    MoveNodeAtNode(leftHandSide, comparison);
  }
}

void RackParser::parseAssignmentEqual(EditionReference &leftHandSide,
                                      Token::Type stoppingType) {
#if 0
  EditionReference rightHandSide;
  parseBinaryOperator(leftHandSide, rightHandSide,
                      Token::Type::AssignmentEqual);
  leftHandSide = Comparison::Builder(
      leftHandSide, ComparisonNode::OperatorType::Equal, rightHandSide);
#else
  // FIXME
  ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
#endif
}

void RackParser::parseRightwardsArrow(EditionReference &leftHandSide,
                                      Token::Type stoppingType) {
  /* Rightwards arrow can either be UnitConvert or Store.
   * The expression 3a->m is a store of 3*a into the variable m
   * The expression 3mm->m is a unit conversion of 3mm into meters
   *
   * When the text contains a RightwardsArrow, we always first parse as
   * unit conversion (see Parser::parse()) to be sure not to misinterpret
   * units as variables (see IdentifierTokenizer::stringTokenType()).
   *
   * The expression is a unit conversion if the left side has units and the
   * rightside has ONLY units.
   *
   * If it fails, the whole string is reparsed, in a special way
   * (see Parser::parseExpressionWithRightwardsArrow)
   * If you arrive here, you should always be in a unit conversion.
   * */

  if (leftHandSide.isUninitialized() ||
      m_parsingContext.parsingMethod() !=
          ParsingContext::ParsingMethod::UnitConversion) {
    // Left-hand side missing or not in a unit conversion
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }

  EditionReference rightHandSide = parseUntil(stoppingType);
  if (!m_nextToken.is(Token::Type::EndOfStream) ||
      rightHandSide.isUninitialized() ||
      !Units::IsCombinationOfUnits(rightHandSide) ||
      (!Units::HasUnit(leftHandSide) &&
       !Units::IsPureAngleUnit(rightHandSide))) {
    // UnitConvert expect a unit on the right and an expression with units on
    // the left
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  turnIntoBinaryNode(KUnitConversion, leftHandSide, rightHandSide);
}

void RackParser::parseLogicalOperatorNot(EditionReference &leftHandSide,
                                         Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // Left-hand side should be empty
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Parse until Not so that not A and B = (not A) and B
  EditionReference rightHandSide =
      parseUntil(std::max(stoppingType, Token::Type::Not));
  if (rightHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  CloneNodeAtNode(rightHandSide, KLogicalNot);
  leftHandSide = rightHandSide;
}

void RackParser::parseBinaryLogicalOperator(Type operatorType,
                                            EditionReference &leftHandSide,
                                            Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  /* And and Nand have same precedence
   * Or, Nor and Xor have same precedence */
  Token::Type newStoppingType;
  if (operatorType == Type::LogicalAnd || operatorType == Type::LogicalNand) {
    static_assert(Token::Type::Nand < Token::Type::And,
                  "Wrong And/Nand precedence.");
    newStoppingType = Token::Type::And;
  } else {
    assert(operatorType == Type::LogicalOr ||
           operatorType == Type::LogicalNor ||
           operatorType == Type::LogicalXor);
    static_assert(Token::Type::Nor < Token::Type::Or &&
                      Token::Type::Xor < Token::Type::Or,
                  "Wrong Or/Nor/Xor precedence.");
    newStoppingType = Token::Type::Or;
  }
  EditionReference rightHandSide =
      parseUntil(std::max(stoppingType, newStoppingType));
  if (rightHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  Tree *node = SharedEditionPool->push(operatorType);
  leftHandSide->moveNodeAtNode(node);
}

void RackParser::parseBinaryOperator(const EditionReference &leftHandSide,
                                     EditionReference &rightHandSide,
                                     Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  rightHandSide = parseUntil(stoppingType);
  assert(!rightHandSide.isUninitialized());
}

void RackParser::parseLeftParenthesis(EditionReference &leftHandSide,
                                      Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  Token::Type endToken = Token::Type::RightParenthesis;

  EditionReference list = parseCommaSeparatedList();
  if (!list.isUninitialized() && list->numberOfChildren() == 2) {
    CloneNodeOverNode(list, KPoint);
    leftHandSide = list;
  } else if (!list.isUninitialized() && list->numberOfChildren() == 1) {
    CloneNodeOverNode(list, KParenthesis);
    leftHandSide = list;
  } else {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    return;
  }

  if (!popTokenIfType(endToken)) {
    // Right parenthesis missing.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  isThereImplicitOperator();
}

void RackParser::parseBang(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  } else {
    CloneNodeAtNode(leftHandSide, KFact);
  }
  isThereImplicitOperator();
}

void RackParser::parsePercent(EditionReference &leftHandSide,
                              Token::Type stoppingType) {
  if (leftHandSide.isUninitialized()) {
    // Left-hand side missing
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  CloneNodeAtNode(leftHandSide, KPercentSimple);
  isThereImplicitOperator();
}

void RackParser::parseConstant(EditionReference &leftHandSide,
                               Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  int index = Constant::ConstantIndex(
      CPL::FromRack(m_root, m_root->indexOfChild(m_currentToken.firstLayout())),
      m_currentToken.length());
  if (index >= 0) {
    leftHandSide =
        SharedEditionPool->push<Type::PhysicalConstant>(uint8_t(index));
  } else {
    assert(m_currentToken.length() == 1);
    leftHandSide = SharedEditionPool->push(
        m_currentToken.toDecoder(m_root).nextCodePoint() == 'e'
            ? Type::ExponentialE
            : Type::Pi);
  }
  isThereImplicitOperator();
}

void RackParser::parseUnit(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Units::Representative *unitRepresentative = nullptr;
  const Units::Prefix *unitPrefix = nullptr;
  RackLayoutDecoder decoder = m_currentToken.toDecoder(m_root);
  if (!Units::Unit::CanParse(&decoder, &unitRepresentative, &unitPrefix)) {
    // Unit does not exist
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  leftHandSide = Units::Unit::Push(unitRepresentative, unitPrefix);
  isThereImplicitOperator();
}

void RackParser::parseReservedFunction(EditionReference &leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  RackLayoutDecoder decoder = m_currentToken.toDecoder(m_root);
  const Builtin *builtin = Builtin::GetReservedFunction(&decoder);
  assert(builtin);
  privateParseReservedFunction(leftHandSide, builtin);
  isThereImplicitOperator();
}

static void PromoteBuiltin(EditionReference &parameterList,
                           const Builtin *builtin) {
  TypeBlock type = builtin->blockType();
  if (!type.isNAry() &&
      parameterList->numberOfChildren() < TypeBlock::NumberOfChildren(type)) {
    // Add default parameters
    if (type == Type::Round) {
      NAry::AddChild(parameterList, (0_e)->clone());
    }
    if (type == Type::RandInt) {
      NAry::AddChildAtIndex(parameterList, (1_e)->clone(), 0);
    }
    if (type.isListStatWithCoefficients()) {
      NAry::AddChild(parameterList, (1_e)->clone());
    }
  }
  MoveNodeOverNode(parameterList,
                   builtin->pushNode(parameterList->numberOfChildren()));
  if (TypeBlock(type).isParametric()) {
    // Move sub-expression at the end
    parameterList->nextTree()->moveTreeBeforeNode(parameterList->child(0));
  }
}

void RackParser::privateParseReservedFunction(EditionReference &leftHandSide,
                                              const Builtin *builtin) {
  const Aliases *aliasesList = builtin->aliases();
  if (aliasesList->contains("log") && popTokenIfType(Token::Type::Subscript)) {
    // Special case for the log function (e.g. "log₂(8)")
    EditionReference base = Parser::Parse(
        m_currentToken.firstLayout()->child(0), m_parsingContext.context());
    EditionReference parameter = parseFunctionParameters();
    if (parameter->numberOfChildren() != 1) {
      // Unexpected number of many parameters.
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    } else {
      MoveTreeOverTree(parameter, parameter->child(0));
      base->swapWithTree(parameter);
      leftHandSide = parameter;
      CloneNodeAtNode(leftHandSide, KLogarithm);
    }
    return;
  }

// Parse cos^n(x)
#if 0
  bool powerFunction = false;
  int powerValue;
  EditionReference power = parseIntegerCaretForFunction(false, &powerValue);
  if (!power.isUninitialized()) {
    assert(power.isInteger());
    if (powerValue == -1) {
      // Detect cos^-1(x) --> arccos(x)
      const char *mainAlias = aliasesList.mainAlias();
      functionHelper =
          ParsingHelper::GetInverseFunction(mainAlias, strlen(mainAlias));
      if (!functionHelper) {
        // This function has no inverse
        ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
      }
      aliasesList = (**functionHelper).aliasesList();
    } else {
      // Detect cos^n(x) with n!=-1 --> (cos(x))^n
      if (!ParsingHelper::IsPowerableFunction(*functionHelper)) {
        // This function can't be powered
        ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
      }
      powerFunction = true;
    }
  }

  if (m_parsingContext.context() &&
      ParsingHelper::IsParameteredExpression(*functionHelper)) {
    //  We must make sure that the parameter is parsed as a single variable.
    const char *parameterText;
    size_t parameterLength;
    if (ParameteredExpression::ParameterText(
            m_currentToken.text() + m_currentToken.length() + 1, &parameterText,
            &parameterLength)) {
      Poincare::Context *oldContext = m_parsingContext.context();
      Poincare::VariableContext parameterContext(
          Poincare::Symbol::Builder(parameterText, parameterLength),
          oldContext);
      m_parsingContext.setContext(&parameterContext);
      leftHandSide = parseFunctionParameters();
      m_parsingContext.setContext(oldContext);
    } else {
      leftHandSide = parseFunctionParameters();
    }
  } else {
    leftHandSide = parseFunctionParameters();
  }
#else
  leftHandSide = parseFunctionParameters();
#endif

  /* The following lines are there because some functions have the same name
   * but not same number of parameters.
   * This is currently only useful for "sum" which can be sum({1,2,3}) or
   * sum(1/k, k, 1, n) */
  assert(!leftHandSide.isUninitialized());
  int numberOfParameters = leftHandSide->numberOfChildren();
#if 0
  if ((**functionHelper).minNumberOfChildren() >= 0) {
    while (numberOfParameters > (**functionHelper).maxNumberOfChildren()) {
      functionHelper++;
      if (functionHelper >= ParsingHelper::ReservedFunctionsUpperBound() ||
          !(**functionHelper).aliasesList().isEquivalentTo(aliasesList)) {
        // Too many parameters provided.
        ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
      }
    }
  }
#endif

  if (numberOfParameters == 1 && builtin->blockType() == Type::Logarithm) {
    builtin = Builtin::GetReservedFunction(KLog);
  } else if (numberOfParameters == 2 && builtin->blockType() == Type::Log) {
    builtin = Builtin::GetReservedFunction(KLogarithm);
  } else if (numberOfParameters == 1 && builtin->blockType() == Type::Sum) {
    builtin = Builtin::GetReservedFunction(KListSum);
  } else if (numberOfParameters == 4 &&
             builtin->blockType() == Type::Derivative) {
    builtin = Builtin::GetReservedFunction(KNthDiff);
  }
  assert(builtin);

  if (!builtin->checkNumberOfParameters(numberOfParameters)) {
    // Too few or too many parameters provided.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }

  PromoteBuiltin(leftHandSide, builtin);
  if (leftHandSide.isUninitialized()) {
    // Incorrect parameter type or too few args
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
#if 0
  if (powerFunction) {
    leftHandSide = Power::Builder(leftHandSide, power);
  }
#endif
}

void RackParser::parseSequence(EditionReference &leftHandSide, const char *name,
                               Token::Type rightDelimiter) {
  // assert(m_nextToken.type() ==
  // ((rightDelimiter == Token::Type::RightSystemBrace)
  // ? Token::Type::LeftSystemBrace
  // : Token::Type::LeftParenthesis));
  popToken();  // Pop the left delimiter
#if 0
  EditionReference rank =
#endif
  parseUntil(rightDelimiter);
  if (!popTokenIfType(rightDelimiter)) {
    // Right delimiter missing
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  } else {
    assert(false);  // Not implemented
    // leftHandSide = Sequence::Builder(name, 1, rank);
  }
}

void RackParser::parseSpecialIdentifier(EditionReference &leftHandSide,
                                        Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  RackLayoutDecoder decoder = m_currentToken.toDecoder(m_root);
  const Builtin *builtin = Builtin::GetSpecialIdentifier(&decoder);
  assert(builtin);
  leftHandSide = builtin->pushNode(0);
  assert(leftHandSide->numberOfChildren() == 0);
  isThereImplicitOperator();
}

void RackParser::parseCustomIdentifier(EditionReference &leftHandSide,
                                       Token::Type stoppingType) {
  assert(leftHandSide.isUninitialized());
  const Tree *node = m_currentToken.firstLayout();
  size_t length = m_currentToken.length();
  constexpr int bufferSize = sizeof(CodePoint) * Symbol::k_maxNameSize;
  char buffer[bufferSize];
  char *end = buffer + bufferSize;
  char *buf = buffer;
  while (length--) {
    assert(node->isCodePointLayout());
    buf = CodePointLayout::CopyName(node, buf, end - buf);
    node = node->nextTree();
  }
  privateParseCustomIdentifier(leftHandSide, buffer, m_currentToken.length(),
                               stoppingType);
  isThereImplicitOperator();
}

void RackParser::privateParseCustomIdentifier(EditionReference &leftHandSide,
                                              const char *name, size_t length,
                                              Token::Type stoppingType) {
  if (!Poincare::SymbolAbstractNode::NameLengthIsValid(name, length)) {
    // Identifier name too long.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }

  /* Check the context: if the identifier does not already exist as a function,
   * seq or list, interpret it as a symbol, even if there are parentheses
   * afterwards.
   * If there is no context, f(x) is always parsed as a function and u{n} as
   * a sequence*/
  Poincare::Context::SymbolAbstractType idType =
      Poincare::Context::SymbolAbstractType::None;
  if (m_parsingContext.context() &&
      m_parsingContext.parsingMethod() !=
          ParsingContext::ParsingMethod::Assignment) {
    idType =
        m_parsingContext.context()->expressionTypeForIdentifier(name, length);
    if (idType != Poincare::Context::SymbolAbstractType::Function &&
        idType != Poincare::Context::SymbolAbstractType::Sequence &&
        idType != Poincare::Context::SymbolAbstractType::List) {
      leftHandSide =
          SharedEditionPool->push<Type::UserSymbol>(name, length + 1);
      return;
    }
  }

#if 0
  if (idType == Poincare::Context::SymbolAbstractType::Sequence ||
      (idType == Poincare::Context::SymbolAbstractType::None &&
       m_nextToken.type() == Token::Type::LeftSystemBrace)) {
    /* If the user is not defining a variable and the identifier is already
     * known to be a sequence, or has an unknown type and is followed
     * by braces, it's a sequence call. */
    if (m_nextToken.type() != Token::Type::LeftSystemBrace &&
        m_nextToken.type() != Token::Type::LeftParenthesis) {
      /* If the identifier is a sequence but not followed by braces, it can
       * also be followed by parenthesis. If not, it's a syntax error. */
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
    parseSequence(leftHandSide, name,
                  m_nextToken.type() == Token::Type::LeftSystemBrace
                      ? Token::Type::RightSystemBrace
                      : Token::Type::RightParenthesis);
    return;
  }

  State previousState = currentState();
  // Try to parse aspostrophe as derivative
  if (privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                                 stoppingType, idType, true)) {
    return;
  }
  // Parse aspostrophe as unit (default parsing)
  setState(previousState);
#endif
  privateParseCustomIdentifierWithParameters(leftHandSide, name, length,
                                             stoppingType, idType, false);
}

bool RackParser::privateParseCustomIdentifierWithParameters(
    EditionReference &leftHandSide, const char *name, size_t length,
    Token::Type stoppingType, Poincare::Context::SymbolAbstractType idType,
    bool parseApostropheAsDerivative) {
#if 0
  int derivativeOrder = 0;
#endif
  if (parseApostropheAsDerivative) {
#if 0
    // Case 1: parse f'''(x)
    while (m_nextToken.length() == 1 &&
           (m_nextToken.text()[0] == '\'' || m_nextToken.text()[0] == '\"')) {
      popToken();
      derivativeOrder += m_currentToken.text()[0] == '\'' ? 1 : 2;
    }
    // Case 2: parse f^(3)(x)
    if (derivativeOrder == 0) {
      EditionReference base =
          parseIntegerCaretForFunction(true, &derivativeOrder);
      if (base.isUninitialized() || derivativeOrder < 0) {
        return false;
      }
    }
#else
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
#endif
  }

  // If the identifier is not followed by parentheses, it is a symbol
  EditionReference parameter = tryParseFunctionParameters();
  if (!parameter) {
#if 0
    if (derivativeOrder > 0) {
      return false;
    }
#endif
    leftHandSide = SharedEditionPool->push<Type::UserSymbol>(name, length + 1);
    return true;
  }

  /* The identifier is followed by parentheses. It can be:
   * - a function call
   * - an access to a list element   */
  int numberOfParameters = parameter->numberOfChildren();
  EditionReference result;
  if (numberOfParameters == 2) {
#if 0
    if (derivativeOrder > 0) {
      return false;
    }
    /* If you change how list accesses are parsed, change it also in parseList
     * or factorize it. */
    result = ListSlice::Builder(parameter->child(0), parameter->child(1),
                                Symbol::Builder(name, length));
#else
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
#endif
  } else if (numberOfParameters == 1) {
    MoveTreeOverTree(parameter, parameter->child(0));
    if (parameter->type() == Type::UserSymbol &&
        strncmp(Symbol::GetName(parameter), name, length) == 0) {
      // Function and variable must have distinct names.
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    } else if (idType == Poincare::Context::SymbolAbstractType::List) {
#if 0
      if (derivativeOrder > 0) {
        return false;
      }
      result = ListElement::Builder(parameter, Symbol::Builder(name, length));
#else
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
#endif
    } else {
#if 0
      if (derivativeOrder > 0) {
        EditionReference derivand =
            Function::Builder(name, length, Symbol::SystemSymbol());
        result =
            Derivative::Builder(derivand, Symbol::SystemSymbol(), parameter,
                                BasedInteger::Builder(derivativeOrder));
      } else {
#endif
      result = SharedEditionPool->push<Type::UserFunction>(name, length + 1);
      parameter->moveNodeBeforeNode(result);
      assert(result->child(0) == parameter);
#if 0
      }
#endif
    }
  } else {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }

  if (result->type() == Type::UserFunction &&
      parameter->type() == Type::UserSymbol &&
      m_nextToken.type() == Token::Type::AssignmentEqual &&
      m_parsingContext.context()) {
    /* Set the parameter in the context to ensure that f(t)=t is not
     * understood as f(t)=1_t
     * If we decide that functions can be assigned with any parameter,
     * this will ensure that f(abc)=abc is understood like f(x)=x
     */
    Poincare::Context *previousContext = m_parsingContext.context();
    Poincare::VariableContext functionAssignmentContext(
        Symbol::GetName(parameter), m_parsingContext.context());
    m_parsingContext.setContext(&functionAssignmentContext);
    // We have to parseUntil here so that we do not lose the
    // functionAssignmentContext pointer.
    leftHandSide = parseUntil(stoppingType, result);
    m_parsingContext.setContext(previousContext);
    return true;
  }
  leftHandSide = result;
  return true;
}

Tree *RackParser::tryParseFunctionParameters() {
  bool parenthesisIsLayout = m_nextToken.is(Token::Type::Layout) &&
                             m_nextToken.firstLayout()->isParenthesisLayout();
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::LeftParenthesis)) {
    // Left parenthesis missing.
    return nullptr;
  }
  if (!parenthesisIsLayout && popTokenIfType(Token::Type::RightParenthesis)) {
    // The function has no parameter.
    return List::PushEmpty();
  }
  Tree *commaSeparatedList = parseCommaSeparatedList();
  if (!parenthesisIsLayout && !popTokenIfType(Token::Type::RightParenthesis)) {
    // Right parenthesis missing
    commaSeparatedList->removeTree();
    return nullptr;
  }
  return commaSeparatedList;
}

Tree *RackParser::parseFunctionParameters() {
  Tree *commaSeparatedList = tryParseFunctionParameters();
  if (!commaSeparatedList) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  return commaSeparatedList;
}

void RackParser::parseMatrix(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  uint8_t numberOfRows = 0;
  uint8_t numberOfColumns = 0;
  Tree *matrix =
      SharedEditionPool->push<Type::Matrix>(numberOfRows, numberOfColumns);
  while (!popTokenIfType(Token::Type::RightBracket)) {
    EditionReference row = parseVector();
    if (numberOfRows > 0 && numberOfColumns != row->numberOfChildren()) {
      // Incorrect matrix.
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
    numberOfColumns = row->numberOfChildren();
    Matrix::SetNumberOfColumns(matrix, numberOfColumns);
    Matrix::SetNumberOfRows(matrix, ++numberOfRows);
    row->removeNode();
  }
  if (numberOfRows == 0 || numberOfColumns == 0) {
    // Empty matrix
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    matrix->removeTree();
  } else {
    leftHandSide = matrix;
  }
  isThereImplicitOperator();
}

Tree *RackParser::parseVector() {
  if (!popTokenIfType(Token::Type::LeftBracket)) {
    // Left bracket missing.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  EditionReference commaSeparatedList = parseCommaSeparatedList();
  if (!commaSeparatedList || commaSeparatedList->numberOfChildren() == 0) {
    // Empty vectors are not handled
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (!popTokenIfType(Token::Type::RightBracket)) {
    // Right bracket missing.
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  return commaSeparatedList;
}

Tree *RackParser::parseCommaSeparatedList(bool isFirstToken) {
  // First rack's layout cannot be a comma separated list.
  if (!isFirstToken && m_nextToken.is(Token::Type::Layout) &&
      m_nextToken.firstLayout()->isParenthesisLayout()) {
    assert(m_nextToken.firstLayout()->nextNode()->isRackLayout());
    // Parse the RackLayout as a comma separated list.
    RackParser subParser(m_nextToken.firstLayout()->nextNode(),
                         m_parsingContext.context(), -1,
                         ParsingContext::ParsingMethod::CommaSeparatedList);
    popToken();
    return subParser.parse();
  }
  EditionReference list = List::PushEmpty();
  if (m_nextToken.is(Token::Type::EndOfStream)) {
    return list;
  }
  int length = 0;
  do {
    if (parseUntil(Token::Type::Comma)) {
      length++;
      NAry::SetNumberOfChildren(list, length);
    }
  } while (popTokenIfType(Token::Type::Comma));
  return list;
}

void RackParser::parseList(EditionReference &leftHandSide,
                           Token::Type stoppingType) {
  if (!leftHandSide.isUninitialized()) {
    // FIXME
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (!popTokenIfType(Token::Type::RightBrace)) {
    leftHandSide = parseCommaSeparatedList(true);
    if (!popTokenIfType(Token::Type::RightBrace)) {
      // Right brace missing.
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
  } else {
    leftHandSide = List::PushEmpty();
  }
  if (popTokenIfType(Token::Type::LeftParenthesis)) {
    EditionReference parameter = parseCommaSeparatedList();
    if (!popTokenIfType(Token::Type::RightParenthesis)) {
      // Right parenthesis missing.
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
    int numberOfParameters = parameter->numberOfChildren();
    if (numberOfParameters == 2) {
      CloneNodeAtNode(leftHandSide, KListSlice);
    } else if (numberOfParameters == 1) {
      CloneNodeAtNode(leftHandSide, KListElement);
    } else {
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
    parameter->removeNode();
  }
  isThereImplicitOperator();
}

void RackParser::parseLayout(EditionReference &leftHandSide,
                             Token::Type stoppingType) {
  // if (!leftHandSide.isUninitialized()) {
  // ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  // }
  assert(m_currentToken.length() == 1);
  /* Parse standalone layouts */
  leftHandSide =
      Parser::Parse(m_currentToken.firstLayout(), m_parsingContext.context());
  isThereImplicitOperator();
}

void RackParser::parseSuperscript(EditionReference &leftHandSide,
                                  Token::Type stoppingType) {
  const Tree *layout = m_currentToken.firstLayout();
  if (leftHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  EditionReference rightHandSide =
      Parser::Parse(layout->child(0), m_parsingContext.context());
  if (rightHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  if (popTokenIfType(Token::Type::Superscript)) {
    // a^b^c -> a^(b^c)
    parseSuperscript(rightHandSide);
  }
  turnIntoBinaryNode(KPow, leftHandSide, rightHandSide);
  isThereImplicitOperator();
}

void RackParser::parsePrefixSuperscript(EditionReference &leftHandSide,
                                        Token::Type stoppingType) {
  // Only used for NL-logarithm
  if (!leftHandSide.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  const Tree *layout = m_currentToken.firstLayout();
  EditionReference base =
      Parser::Parse(layout->child(0), m_parsingContext.context());
  if (base.isUninitialized()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  popToken();
  EditionReference log;
  parseReservedFunction(log, Token::Type::ImplicitTimes);
  if (log.isUninitialized() || !log->isLog()) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  // Turn log into logarithm
  leftHandSide = PatternMatching::Create(KLogarithm(KA, KB),
                                         {.KA = log->firstChild(), .KB = base});
  base->removeTree();
  log->removeTree();
}

bool IsIntegerBaseTenOrEmptyExpression(const Tree *e) {
  // TODO PCJ: enforce a decimal base
  /* TODO PCJ: the OrEmpty part was used to parsed a mixed fraction with three
   * empty squares inserted from the toolbox: make sure it works by inserting a
   * layout directly and remove this part. */
  return e->isInteger();
}

#if 0
EditionReference Parser::parseIntegerCaretForFunction(bool allowParenthesis,
                                                int *caretIntegerValue) {
  // Parse f^n(x)
  Token::Type endDelimiterOfPower;
  if (popTokenIfType(Token::Type::CaretWithParenthesis)) {
    endDelimiterOfPower = Token::Type::RightSystemParenthesis;
  } else if (popTokenIfType(Token::Type::Caret)) {
    endDelimiterOfPower = Token::Type::RightParenthesis;
    if (!popTokenIfType(Token::Type::LeftParenthesis)) {
      // Exponent should be parenthesed
      // TODO: allow without parenthesis?
      ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
    }
  } else {
    return Expression();
  }
  Expression base = parseUntil(endDelimiterOfPower);
  if (!popTokenIfType(endDelimiterOfPower)) {
    ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  }
  bool isSymbol;
  assert(caretIntegerValue);
  Expression result =
      allowParenthesis && base.type() == ExpressionNode::Type::Parenthesis
          ? base.child(0)
          : base;
  if (SimplificationHelper::extractInteger(result, caretIntegerValue,
                                           &isSymbol) &&
      !isSymbol) {
    return result;
  }
  ExceptionCheckpoint::Raise(ExceptionType::ParseFail);
  return Expression();
}
#endif

bool RackParser::generateMixedFractionIfNeeded(EditionReference &leftHandSide) {
  if (false /*m_parsingContext.context() &&
             !Preferences::SharedPreferences()->mixedFractionsAreEnabled()*/) {
    /* If m_context == nullptr, the expression has already been parsed.
     * We do not escape here because we want to parse it the same way it was
     * parsed the first time.
     * It can for example be a mixed fraction inputed earlier with a different
     * country preference.
     * There is no risk of confusion with a multiplication since a parsed
     * multiplication between an integer and a fraction will be beautified
     * by adding a multiplication symbol between the two. */
    return false;
  }
  State previousState = currentState();
  // Check for mixed fraction. There is a mixed fraction if :
  if (IsIntegerBaseTenOrEmptyExpression(leftHandSide)
      // The next token is either a number or empty
      && m_nextToken.is(Token::Type::Number)) {
    // TODO PCJ: or popToken is a FractionLayout
    m_waitingSlashForMixedFraction = true;
    EditionReference rightHandSide = parseUntil(Token::Type::LeftBrace);
    m_waitingSlashForMixedFraction = false;
    if (!rightHandSide.isUninitialized() && rightHandSide->isDivision() &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(0)) &&
        IsIntegerBaseTenOrEmptyExpression(rightHandSide->child(1))) {
      // The following expression looks like "int/int" -> it's a mixedFraction
      CloneNodeAtNode(leftHandSide, KMixedFraction);
      return true;
    }
  }
  setState(previousState);
  return false;
}

void RackParser::setState(State state) {
  m_tokenizer.setState(state.tokenizerState);
  m_currentToken = state.currentToken;
  m_nextToken = state.nextToken;
  m_pendingImplicitOperator = state.pendingImplicitOperator;
  m_waitingSlashForMixedFraction = state.waitingSlashForMixedFraction;
}

}  // namespace PoincareJ
