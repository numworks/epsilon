#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

#include "tokenizer.h"

namespace Poincare {

class Parser {
public:
  enum class Status {
    Success,
    Progress,
    Error
  };

  Parser(const char * text) :
    m_status(Status::Progress),
    m_tokenizer(text),
    m_currentToken(Token(Token::Undefined)),
    m_nextToken(m_tokenizer.popToken()),
    m_pendingImplicitMultiplication(false) {}

  Expression parse();
  Status getStatus() const { return m_status; }

private:
  Expression parseUntil(Token::Type stoppingType);

  // Methods on Tokens
  void popToken();
  bool canPopToken(Token::Type type);
  bool popTokenUntil(Token::Type stoppingType);
  void isThereImplicitMultiplication();

  // Specific Token parsers
  void raiseError(Expression & leftHandSide);
  void parseNumber(Expression & leftHandSide);
  void parseSymbol(Expression & leftHandSide);
  void parseMatrix(Expression & leftHandSide);
  void parseLeftParenthesis(Expression & leftHandSide);
  void parseBang(Expression & leftHandSide);
  void parsePlus(Expression & leftHandSide);
  void parseMinus(Expression & leftHandSide);
  void parseTimes(Expression & leftHandSide);
  void parseSlash(Expression & leftHandSide);
  void parseImplicitTimes(Expression & leftHandSide);
  void parseCaret(Expression & leftHandSide);
  void parseEqual(Expression & leftHandSide);
  void parseStore(Expression & leftHandSide);

  // Parsing helpers
  bool parseBinaryOperator(const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType);
  Expression parseVector();
  template <class T> Expression parseReservedFunction();
  Expression parseFunctionParameters();
  Expression parseCommaSeparatedList();

  // Data members
  Status m_status;
  Tokenizer m_tokenizer;
  Token m_currentToken;
  Token m_nextToken;
  bool m_pendingImplicitMultiplication;
};

}

#endif
