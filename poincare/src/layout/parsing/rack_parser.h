#pragma once

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

#include <poincare/helpers/parser.h>
#include <poincare/src/expression/builtin.h>
#include <poincare/src/memory/tree_stack_checkpoint.h>
#include <poincare/syntax_error.h>

#include "parsing_context.h"
#include "tokenizer.h"

namespace Poincare::Internal::Parser {

class RackParser {
 public:
  enum class Status { Success, Progress, Error };

  /* Set the context to nullptr if the expression has already been parsed
   * Setting the context to nullptr removes some ambiguous cases like:
   * - f(x) will always be parsed as f(x) and not f*(x)
   * - u{n} will always be parsed as u_n and not u*{n}
   * - abc will always be parsed as abc and not a*b*c
   * The same is true if you set isAssignment = true
   * but the parser will not use the isAssignment anymore when it encounters a
   * "=". (so that f(x)=xy is parsed as f(x)=x*y, and not as f*(x)=x*y or as
   * f(x)=xy) */
  RackParser(const Tree* rack, ParsingContext parsingContext, int textStart = 0,
             int textEnd = -1)
      : m_parsingContext(parsingContext),
        m_tokenizer(Rack::From(rack), &m_parsingContext, textStart, textEnd),
        m_currentToken(Token(Token::Type::Undefined)),
        m_nextToken(Token(Token::Type::Undefined)),
        m_pendingImplicitOperator(false),
        m_waitingSlashForMixedFraction(false),
        m_root(rack) {}

  /* All the parseSomething methods may raise ParseFail and should return
   * a valid expression otherwise (not nullptr). */
  Tree* parseRack();
  Tree* parseTopLevelRack();
  static void RaiseParseFail(SyntaxError error) {
    TreeStackCheckpoint::Raise(ExceptionType::ParseFail,
                               static_cast<uint8_t>(error));
  }

 private:
  Tree* parseUntil(Token::Type stoppingType, Tree* leftHandSide = nullptr);
  Tree* parseExpressionWithRightwardsArrow(size_t rightwardsArrowPosition);
  Tree* initializeFirstTokenAndParseUntilEnd();

  // Methods on Tokens
  void popToken();
  bool popTokenIfType(Token::Type type);
  bool nextTokenHasPrecedenceOver(Token::Type stoppingType);

  void isThereImplicitOperator();
  Token::Type implicitOperatorType();

  // Specific Token parsers
  void parseUnexpected(Tree*& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0);
  void parseNumber(Tree*& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseConstant(Tree*& leftHandSide,
                     Token::Type stoppingType = (Token::Type)0);
  void parseUnit(Tree*& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseReservedFunction(Tree*& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseSpecialIdentifier(Tree*& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  void parseCustomIdentifier(Tree*& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseMatrix(Tree*& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseLeftParenthesis(Tree*& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseBang(Tree*& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parsePercent(Tree*& leftHandSide,
                    Token::Type stoppingType = (Token::Type)0);
  void parsePlus(Tree*& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseMinus(Tree*& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseTimes(Tree*& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseSlash(Tree*& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseImplicitTimes(Tree*& leftHandSide,
                          Token::Type stoppingType = (Token::Type)0);
  void parseImplicitAdditionBetweenUnits(
      Tree*& leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseEuclideanDivision(Tree*& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  void parseCaret(Tree*& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseComparisonOperator(Tree*& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAssignmentEqual(Tree*& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLogicalOperatorNot(Tree*& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAndOperator(Tree*& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalAnd, leftHandSide, stoppingType);
  }
  void parseNandOperator(Tree*& leftHandSide,
                         Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalNand, leftHandSide, stoppingType);
  }
  void parseOrOperator(Tree*& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalOr, leftHandSide, stoppingType);
  }
  void parseXorOperator(Tree*& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalXor, leftHandSide, stoppingType);
  }
  void parseNorOperator(Tree*& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalNor, leftHandSide, stoppingType);
  }

  void parseUnitConversion(Tree*& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseLeftSuperscript(Tree*& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseList(Tree*& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseNorthEastArrow(Tree*& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseSouthEastArrow(Tree*& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseLayout(Tree*& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseSuperscript(Tree*& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0);
  void parsePrefixSuperscript(Tree*& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  // Parsing helpers
  void privateParsePlusAndMinus(Tree*& leftHandSide, bool plus,
                                Token::Type stoppingType = (Token::Type)0);
  void privateParseEastArrow(Tree*& leftHandSide, bool north,
                             Token::Type stoppingType = (Token::Type)0);
  bool mergeIntoPercentAdditionIfNeeded(Tree*& leftHandSide,
                                        Tree*& rightHandSide, bool plus);
  void parseBinaryLogicalOperator(Type operatorType, Tree*& leftHandSide,
                                  Token::Type stoppingType);
  void parseBinaryOperator(const Tree* leftHandSide, Tree*& rightHandSide,
                           Token::Type stoppingType);
  Tree* parseVector();
  // Return nullptr if parentheses could not be parsed
  Tree* tryParseFunctionParameters(bool isBuiltinFunction = false);
  Tree* parseFunctionParameters();
  void parseListParameters(Tree*& leftHandSide);
  Tree* parseCommaSeparatedList(bool isFirstToken = false);
  Tree* parseCommaSeparatedListInChildRack();
  void privateParseTimes(Tree*& leftHandSide, Token::Type stoppingType);
  void privateParseReservedFunction(Tree*& leftHandSide,
                                    const Builtin* builtin);
  void privateParseCustomIdentifier(Tree*& leftHandSide, const char* name,
                                    size_t length, Token::Type stoppingType);
  bool privateParseCustomIdentifierWithParameters(
      Tree*& leftHandSide, const char* name, size_t length,
      Token::Type stoppingType, bool parseApostropheAsDerivative);
  bool parseApostropheDerivationOrder(int* derivationOrder);
  bool parseIntegerCaretForFunction(bool allowParenthesis,
                                    int* caretIntegerValue);
  bool generateMixedFractionIfNeeded(Tree*& leftHandSide);

  // Save and restore parser state
  struct State {
    ParsingContext parsingContext;
    Tokenizer::State tokenizerState;
    Token currentToken;
    Token nextToken;
    bool pendingImplicitOperator;
    bool waitingSlashForMixedFraction;
  };
  State currentState() const {
    return State{
        .parsingContext = m_parsingContext,
        .tokenizerState = m_tokenizer.currentState(),
        .currentToken = m_currentToken,
        .nextToken = m_nextToken,
        .pendingImplicitOperator = m_pendingImplicitOperator,
        .waitingSlashForMixedFraction = m_waitingSlashForMixedFraction};
  }
  void setState(const State& state);

  // Data members
  ParsingContext m_parsingContext;
  Tokenizer m_tokenizer;
  Token m_currentToken;
  Token m_nextToken;
  bool m_pendingImplicitOperator;
  bool m_waitingSlashForMixedFraction;
  const Tree* m_root;
};

}  // namespace Poincare::Internal::Parser
