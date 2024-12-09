#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

#include "helper.h"
#include "parsing_context.h"
#include "tokenizer.h"

namespace Poincare {

class Parser {
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
  Parser(const char* text, Context* context, const char* textEnd = nullptr,
         ParsingContext::ParsingMethod parsingMethod =
             ParsingContext::ParsingMethod::Classic)
      : m_parsingContext(context, parsingMethod),
        m_status(Status::Progress),
        m_tokenizer(text, &m_parsingContext, textEnd),
        m_currentToken(Token(Token::Type::Undefined)),
        m_nextToken(Token(Token::Type::Undefined)),
        m_pendingImplicitOperator(false),
        m_waitingSlashForMixedFraction(false) {}

  OExpression parse();
  Status getStatus() const { return m_status; }

 private:
  OExpression parseUntil(Token::Type stoppingType,
                         OExpression leftHandSide = OExpression());
  OExpression parseExpressionWithRightwardsArrow(
      const char* rightwardsArrowPosition);
  OExpression initializeFirstTokenAndParseUntilEnd();

  // Methods on Tokens
  void popToken();
  bool popTokenIfType(Token::Type type);
  bool nextTokenHasPrecedenceOver(Token::Type stoppingType);

  void isThereImplicitOperator();
  Token::Type implicitOperatorType();

  // Specific Token parsers
  void parseUnexpected(OExpression& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0);
  void parseNumber(OExpression& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseConstant(OExpression& leftHandSide,
                     Token::Type stoppingType = (Token::Type)0);
  void parseUnit(OExpression& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseReservedFunction(OExpression& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseSpecialIdentifier(OExpression& leftHandSide,
                              Token::Type stoppingType = (Token::Type)0);
  void parseCustomIdentifier(OExpression& leftHandSide,
                             Token::Type stoppingType = (Token::Type)0);
  void parseEmpty(OExpression& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseMatrix(OExpression& leftHandSide,
                   Token::Type stoppingType = (Token::Type)0);
  void parseLeftParenthesis(OExpression& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLeftSystemParenthesis(OExpression& leftHandSide,
                                  Token::Type stoppingType = (Token::Type)0);
  void parseLeftSystemBrace(OExpression& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseBang(OExpression& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parsePercent(OExpression& leftHandSide,
                    Token::Type stoppingType = (Token::Type)0);
  void parsePlus(OExpression& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseMinus(OExpression& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseTimes(OExpression& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseSlash(OExpression& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseImplicitTimes(OExpression& leftHandSide,
                          Token::Type stoppingType = (Token::Type)0);
  void parseImplicitAdditionBetweenUnits(
      OExpression& leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCaret(OExpression& leftHandSide,
                  Token::Type stoppingType = (Token::Type)0);
  void parseCaretWithParenthesis(OExpression& leftHandSide,
                                 Token::Type stoppingType = (Token::Type)0);
  void parseComparisonOperator(OExpression& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAssignmentEqual(OExpression& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLogicalOperatorNot(OExpression& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  void parseAndOperator(OExpression& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::And,
                               leftHandSide, stoppingType);
  }
  void parseNandOperator(OExpression& leftHandSide,
                         Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nand,
                               leftHandSide, stoppingType);
  }
  void parseOrOperator(OExpression& leftHandSide,
                       Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Or,
                               leftHandSide, stoppingType);
  }
  void parseXorOperator(OExpression& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Xor,
                               leftHandSide, stoppingType);
  }
  void parseNorOperator(OExpression& leftHandSide,
                        Token::Type stoppingType = (Token::Type)0) {
    parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nor,
                               leftHandSide, stoppingType);
  }

  void parseRightwardsArrow(OExpression& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseLeftSuperscript(OExpression& leftHandSide,
                            Token::Type stoppingType = (Token::Type)0);
  void parseList(OExpression& leftHandSide,
                 Token::Type stoppingType = (Token::Type)0);
  void parseNorthEastArrow(OExpression& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseSouthEastArrow(OExpression& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);

  // Parsing helpers
  void privateParsePlusAndMinus(OExpression& leftHandSide, bool plus,
                                Token::Type stoppingType = (Token::Type)0);
  void privateParseEastArrow(OExpression& leftHandSide, bool north,
                             Token::Type stoppingType = (Token::Type)0);
  void parseBinaryLogicalOperator(
      BinaryLogicalOperatorNode::OperatorType operatorType,
      OExpression& leftHandSide, Token::Type stoppingType);
  bool parseBinaryOperator(const OExpression& leftHandSide,
                           OExpression& rightHandSide,
                           Token::Type stoppingType);
  OExpression parseVector();
  OExpression parseFunctionParameters();
  OExpression parseCommaSeparatedList();
  void privateParseTimes(OExpression& leftHandSide, Token::Type stoppingType);
  void privateParseReservedFunction(
      OExpression& leftHandSide,
      const OExpression::FunctionHelper* const* functionHelper);
  void privateParseCustomIdentifier(OExpression& leftHandSide, const char* name,
                                    size_t length, Token::Type stoppingType);
  bool privateParseCustomIdentifierWithParameters(
      OExpression& leftHandSide, const char* name, size_t length,
      Token::Type stoppingType, Context::UserNamedType idType,
      bool parseApostropheAsDerivative);
  void parseSequence(OExpression& leftHandSide, const char* name,
                     Token::Type rightDelimiter);
  void defaultParseLeftParenthesis(bool isSystemParenthesis,
                                   OExpression& leftHandSide,
                                   Token::Type stoppingType);
  OExpression parseIntegerCaretForFunction(bool allowParenthesis,
                                           int* caretIntegerValue);
  bool generateMixedFractionIfNeeded(OExpression& leftHandSide);

  // Save and restore parser state
  struct State {
    Status status;
    Tokenizer::State tokenizerState;
    Token currentToken;
    Token nextToken;
    bool pendingImplicitOperator;
    bool waitingSlashForMixedFraction;
  };
  State currentState() {
    return State{
        .status = m_status,
        .tokenizerState = m_tokenizer.currentState(),
        .currentToken = m_currentToken,
        .nextToken = m_nextToken,
        .pendingImplicitOperator = m_pendingImplicitOperator,
        .waitingSlashForMixedFraction = m_waitingSlashForMixedFraction};
  }
  void setState(State state);

  // Data members
  ParsingContext m_parsingContext;
  Status m_status;
  /* m_status is initialized to Status::Progress,
   * is changed to Status::Error if the Parser encounters an error,
   * and is otherwise changed Status::Success. */
  Tokenizer m_tokenizer;
  Token m_currentToken;
  Token m_nextToken;
  bool m_pendingImplicitOperator;
  bool m_waitingSlashForMixedFraction;
};

}  // namespace Poincare

#endif
