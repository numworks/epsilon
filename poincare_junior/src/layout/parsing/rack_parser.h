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
  RackParser(const Node* node, /*Context* context,*/ size_t textEnd = 0,
             ParsingContext::ParsingMethod parsingMethod =
                 ParsingContext::ParsingMethod::Classic)
      : m_parsingContext(/*context,*/ parsingMethod),
        m_status(Status::Progress),
        m_tokenizer(node, &m_parsingContext, textEnd),
        m_currentToken(Token(Token::Type::Undefined)),
        m_nextToken(Token(Token::Type::Undefined)),
        m_pendingImplicitOperator(false),
        m_waitingSlashForMixedFraction(false) {}

  EditionReference parse();
  Status getStatus() const { return m_status; }

 private:
  EditionReference parseUntil(
      Token::Type stoppingType,
      EditionReference leftHandSide = EditionReference());
  EditionReference parseExpressionWithRightwardsArrow(
      size_t rightwardsArrowPosition);
  EditionReference initializeFirstTokenAndParseUntilEnd();

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
  void parseAssigmentEqual(EditionReference& leftHandSide,
                           Token::Type stoppingType = (Token::Type)0);
  void parseLogicalOperatorNot(EditionReference& leftHandSide,
                               Token::Type stoppingType = (Token::Type)0);
  // void parseAndOperator(EditionReference& leftHandSide,
  // Token::Type stoppingType = (Token::Type)0) {
  // parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::And,
  // leftHandSide, stoppingType);
  // }
  // void parseNandOperator(EditionReference& leftHandSide,
  // Token::Type stoppingType = (Token::Type)0) {
  // parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nand,
  // leftHandSide, stoppingType);
  // }
  // void parseOrOperator(EditionReference& leftHandSide,
  // Token::Type stoppingType = (Token::Type)0) {
  // parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Or,
  // leftHandSide, stoppingType);
  // }
  // void parseXorOperator(EditionReference& leftHandSide,
  // Token::Type stoppingType = (Token::Type)0) {
  // parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Xor,
  // leftHandSide, stoppingType);
  // }
  // void parseNorOperator(EditionReference& leftHandSide,
  // Token::Type stoppingType = (Token::Type)0) {
  // parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nor,
  // leftHandSide, stoppingType);
  // }

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
  // Parsing helpers
  void privateParsePlusAndMinus(EditionReference& leftHandSide, bool plus,
                                Token::Type stoppingType = (Token::Type)0);
  void privateParseEastArrow(EditionReference& leftHandSide, bool north,
                             Token::Type stoppingType = (Token::Type)0);
  // void parseBinaryLogicalOperator(
  // BinaryLogicalOperatorNode::OperatorType operatorType,
  // EditionReference& leftHandSide, Token::Type stoppingType);
  bool parseBinaryOperator(const EditionReference& leftHandSide,
                           EditionReference& rightHandSide,
                           Token::Type stoppingType);
  EditionReference parseVector();
  EditionReference parseFunctionParameters();
  EditionReference parseCommaSeparatedList();
  void privateParseTimes(EditionReference& leftHandSide,
                         Token::Type stoppingType);
  void privateParseReservedFunction(EditionReference& leftHandSide,
                                    const Builtin* builtin);
  void privateParseCustomIdentifier(EditionReference& leftHandSide,
                                    const char* name, size_t length,
                                    Token::Type stoppingType);
  void parseSequence(EditionReference& leftHandSide, const char* name,
                     Token::Type rightDelimiter);
  bool generateMixedFractionIfNeeded(EditionReference& leftHandSide);
  // Allows you to rewind to previous position
  void rememberCurrentParsingPosition(size_t* tokenizerPosition,
                                      Token* storedCurrentToken = nullptr,
                                      Token* storedNextToken = nullptr);
  void restorePreviousParsingPosition(
      size_t tokenizerPosition,
      Token storedCurrentToken = Token(Token::Type::Undefined),
      Token storedNextToken = Token(Token::Type::Undefined));
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

}  // namespace PoincareJ

#endif
