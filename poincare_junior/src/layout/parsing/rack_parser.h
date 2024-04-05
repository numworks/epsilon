#ifndef POINCARE_JUNIOR_LAYOUT_PARSING_PARSER_H
#define POINCARE_JUNIOR_LAYOUT_PARSING_PARSER_H

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

// #include "helper.h"
#include <poincare_junior/src/expression/builtin.h>

#include "parsing_context.h"
#include "tokenizer.h"

namespace PoincareJ {

/* TODO_PCJ : This class closely follows Poincare::Parser. Last update was on
 * 5fa0f7c. When Poincare::Parser is removed, updated  with latest changes and
 * remove this comment. */

class RackParser {
 public:
  enum class Status { Success, Progress, Error };

  /* Set the context to nullptr if the expression has already been parsed
   * Setting the context to nullptr removes some ambiguous cases like:
   * - f(x) will always be parsed as f(x) and not f*(x)
   * - u{n} will always be parsed as u_n and not u*{n}
   * - abc will always be parsed as abc and not a*b*c
   * The same is true if you set parseForAssignment = true
   * but the parser will set parseForAssignment = false when it encounters a
   * "=". (so that f(x)=xy is parsed as f(x)=x*y, and not as f*(x)=x*y or as
   * f(x)=xy) */
  RackParser(const Tree* node, Poincare::Context* context, int textEnd = -1,
             ParsingContext::ParsingMethod parsingMethod =
                 ParsingContext::ParsingMethod::Classic)
      : m_parsingContext(context, parsingMethod),
        m_tokenizer(node, &m_parsingContext, 0, textEnd),
        m_currentToken(Token(Token::Type::Undefined)),
        m_nextToken(Token(Token::Type::Undefined)),
        m_pendingImplicitOperator(false),
        m_waitingSlashForMixedFraction(false),
        m_root(node) {}

  Tree* parse();

 private:
  Tree* parseUntil(Token::Type stoppingType,
                   EditionReference leftHandSide = EditionReference());
  Tree* parseExpressionWithRightwardsArrow(size_t rightwardsArrowPosition);
  Tree* initializeFirstTokenAndParseUntilEnd();

  // Methods on Tokens
  void popToken();
  bool popTokenIfType(Token::Type type);
  bool nextTokenHasPrecedenceOver(Token::Type stoppingType);

  void isThereImplicitOperator();
  Token::Type implicitOperatorType();

  // Specific Token parsers
  void parseUnexpected(EditionReference& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0);
  void parseNumber(EditionReference& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseConstant(EditionReference& leftHandSide,
                     Token::Type stoppingType = (Token::Type)0);
  void parseUnit(EditionReference& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseReservedFunction(EditionReference& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseSpecialIdentifier(EditionReference& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  void parseCustomIdentifier(EditionReference& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseMatrix(EditionReference& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseLeftParenthesis(EditionReference& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseBang(EditionReference& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parsePercent(EditionReference& leftHandSide,
                    Token::Type stoppingType = (Token::Type)0);
  void parsePlus(EditionReference& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseMinus(EditionReference& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseTimes(EditionReference& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseSlash(EditionReference& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseImplicitTimes(EditionReference& leftHandSide,
                          Token::Type stoppingType = (Token::Type)0);
  void parseImplicitAdditionBetweenUnits(
      EditionReference& leftHandSide,
      Token::Type stoppingType = (Token::Type)0);
  void parseCaret(EditionReference& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseComparisonOperator(EditionReference& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAssignmentEqual(EditionReference& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLogicalOperatorNot(EditionReference& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAndOperator(EditionReference& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalAnd, leftHandSide, stoppingType);
  }
  void parseNandOperator(EditionReference& leftHandSide,
                         Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalNand, leftHandSide, stoppingType);
  }
  void parseOrOperator(EditionReference& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalOr, leftHandSide, stoppingType);
  }
  void parseXorOperator(EditionReference& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalXor, leftHandSide, stoppingType);
  }
  void parseNorOperator(EditionReference& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(Type::LogicalNor, leftHandSide, stoppingType);
  }

  void parseRightwardsArrow(EditionReference& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLeftSuperscript(EditionReference& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseList(EditionReference& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseNorthEastArrow(EditionReference& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseSouthEastArrow(EditionReference& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseLayout(EditionReference& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseSuperscript(EditionReference& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0);
  void parsePrefixSuperscript(EditionReference& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  // Parsing helpers
  void privateParsePlusAndMinus(EditionReference& leftHandSide, bool plus,
                                Token::Type stoppingType = (Token::Type)0);
  void privateParseEastArrow(EditionReference& leftHandSide, bool north,
                             Token::Type stoppingType = (Token::Type)0);
  bool mergeIntoPercentAdditionIfNeeded(EditionReference& leftHandSide,
                                        EditionReference& rightHandSide,
                                        bool plus);
  void parseBinaryLogicalOperator(Type operatorType,
                                  EditionReference& leftHandSide,
                                  Token::Type stoppingType);
  void parseBinaryOperator(const EditionReference& leftHandSide,
                           EditionReference& rightHandSide,
                           Token::Type stoppingType);
  Tree* parseVector();
  // Return nullptr if parentheses could not be parsed
  Tree* tryParseFunctionParameters();
  Tree* parseFunctionParameters();
  Tree* parseCommaSeparatedList(bool isFirstToken = false);
  void privateParseTimes(EditionReference& leftHandSide,
                         Token::Type stoppingType);
  void privateParseReservedFunction(EditionReference& leftHandSide,
                                    const Builtin* builtin);
  void privateParseCustomIdentifier(EditionReference& leftHandSide,
                                    const char* name, size_t length,
                                    Token::Type stoppingType);
  bool privateParseCustomIdentifierWithParameters(
      EditionReference& leftHandSide, const char* name, size_t length,
      Token::Type stoppingType, Poincare::Context::SymbolAbstractType idType,
      bool parseApostropheAsDerivative);
  void parseSequence(EditionReference& leftHandSide, const char* name,
                     Token::Type rightDelimiter);
  EditionReference parseIntegerCaretForFunction(bool allowParenthesis,
                                                int* caretIntegerValue);
  bool generateMixedFractionIfNeeded(EditionReference& leftHandSide);

  // Save and restore parser state
  struct State {
    Tokenizer::State tokenizerState;
    Token currentToken;
    Token nextToken;
    bool pendingImplicitOperator;
    bool waitingSlashForMixedFraction;
  };
  State currentState() {
    return State{
        .tokenizerState = m_tokenizer.currentState(),
        .currentToken = m_currentToken,
        .nextToken = m_nextToken,
        .pendingImplicitOperator = m_pendingImplicitOperator,
        .waitingSlashForMixedFraction = m_waitingSlashForMixedFraction};
  }
  void setState(State state);

  // Data members
  ParsingContext m_parsingContext;
  Tokenizer m_tokenizer;
  Token m_currentToken;
  Token m_nextToken;
  bool m_pendingImplicitOperator;
  bool m_waitingSlashForMixedFraction;
  const Tree* m_root;
};

}  // namespace PoincareJ

#endif
