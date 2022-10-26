#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

#include "helper.h"
#include "tokenizer.h"
#include "parsing_context.h"


namespace Poincare {

class Parser {
public:
  enum class Status {
    Success,
    Progress,
    Error
  };

  /* Set the context to nullptr if the expression has already been parsed
   * Setting the context to nullptr removes some ambiguous cases like:
   * - f(x) will always be parsed as f(x) and not f*(x)
   * - u{n} will always be parsed as u_n and not u*{n}
   * - abc will always be parsed as abc and not a*b*c
   * The same is true if you set parseForAssignment = true
   * but the parser will set parseForAssignment = false when it encounters a "=".
   * (so that f(x)=xy is parsed as f(x)=x*y, and not as f*(x)=x*y or as f(x)=xy) */
  Parser(const char * text, Context * context, const char * textEnd = nullptr, ParsingContext::ParsingMethod parsingMethod = ParsingContext::ParsingMethod::Classic) :
    m_parsingContext(context, parsingMethod),
    m_status(Status::Progress),
    m_tokenizer(text, &m_parsingContext, textEnd),
    m_currentToken(Token(Token::Undefined)),
    m_nextToken(Token(Token::Undefined)),
    m_pendingImplicitOperator(false),
    m_waitingSlashForMixedFraction(false)
  {}

  Expression parse();
  Status getStatus() const { return m_status; }

private:
  Expression parseUntil(Token::Type stoppingType, Expression leftHandSide = Expression());
  Expression parseExpressionWithRightwardsArrow(const char * rightwardsArrowPosition);
  Expression initializeFirstTokenAndParseUntilEnd();

  // Methods on Tokens
  void popToken();
  bool popTokenIfType(Token::Type type);
  bool nextTokenHasPrecedenceOver(Token::Type stoppingType);

  void isThereImplicitOperator();
  Token::Type implicitOperatorType();

  // Specific Token parsers
  void parseUnexpected(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseNumber(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseConstant(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseUnit(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseReservedFunction(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseSpecialIdentifier(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCustomIdentifier(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseEmpty(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseMatrix(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftParenthesis(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftSystemParenthesis(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftSystemBrace(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseBang(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parsePercent(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parsePlus(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseMinus(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseTimes(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseSlash(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseImplicitTimes(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseImplicitAdditionBetweenUnits(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCaret(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCaretWithParenthesis(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseComparisonOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseAssigmentEqual(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLogicalOperatorNot(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseAndOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0) { parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::And, leftHandSide, stoppingType); }
  void parseNandOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0) { parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nand, leftHandSide, stoppingType); }
  void parseOrOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0) { parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Or, leftHandSide, stoppingType); }
  void parseXorOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0) { parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Xor, leftHandSide, stoppingType); }
  void parseNorOperator(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0) { parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType::Nor, leftHandSide, stoppingType); }

  void parseRightwardsArrow(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftSuperscript(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseList(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseNorthEastArrow(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseSouthEastArrow(Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);

  // Parsing helpers
  void privateParsePlusAndMinus(Expression & leftHandSide, bool plus, Token::Type stoppingType = (Token::Type)0);
  void privateParseEastArrow(Expression & leftHandSide, bool north, Token::Type stoppingType = (Token::Type)0);
  void parseBinaryLogicalOperator(BinaryLogicalOperatorNode::OperatorType operatorType, Expression & leftHandSide, Token::Type stoppingType);
  bool parseBinaryOperator(const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType);
  Expression parseVector();
  Expression parseFunctionParameters();
  Expression parseCommaSeparatedList();
  void privateParseTimes(Expression & leftHandSide, Token::Type stoppingType);
  void privateParseReservedFunction(Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper);
  void privateParseCustomIdentifier(Expression & leftHandSide, const char * name, size_t length, Token::Type stoppingType);
  void parseSequence(Expression & leftHandSide, const char * name, Token::Type rightDelimiter);
  void defaultParseLeftParenthesis(bool isSystemParenthesis, Expression & leftHandSide, Token::Type stoppingType);
  bool generateMixedFractionIfNeeded(Expression & leftHandSide);
  // Allows you to rewind to previous position
  void rememberCurrentParsingPosition(const char ** tokenizerPosition, Token * storedCurrentToken = nullptr, Token * storedNextToken = nullptr);
  void restorePreviousParsingPosition(const char * tokenizerPosition, Token storedCurrentToken = Token(Token::Undefined), Token storedNextToken = Token(Token::Undefined));
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

}

#endif
