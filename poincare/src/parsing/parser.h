#ifndef POINCARE_PARSING_PARSER_H
#define POINCARE_PARSING_PARSER_H

/* A precedence-climbing parser is implemented hereafter.
 * It is a trade-off between
 *   a readable but less efficient recursive-descent parser
 * and
 *   an efficient but less readable shunting-yard parser. */

#include <poincare_nodes.h>
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

  Expression parse(Context * context);
  Status getStatus() const { return m_status; }

  static bool IsReservedName(const char * name, size_t nameLength);

private:
  static const Expression::FunctionHelper * const * GetReservedFunction(const char * name, size_t nameLength);
  static bool IsSpecialIdentifierName(const char * name, size_t nameLength);

  Expression parseUntil(Context * context, Token::Type stoppingType);

  // Methods on Tokens
  void popToken();
  bool popTokenIfType(Token::Type type);
  bool nextTokenHasPrecedenceOver(Token::Type stoppingType);
  void isThereImplicitMultiplication();

  // Specific Token parsers
  void parseUnexpected(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseNumber(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseConstant(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseUnit(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseIdentifier(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseEmpty(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseMatrix(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftSystemParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseBang(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parsePlus(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseMinus(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseTimes(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseSlash(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseImplicitTimes(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCaret(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseCaretWithParenthesis(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseEqual(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseStore(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);
  void parseLeftSuperscript(Context * context, Expression & leftHandSide, Token::Type stoppingType = (Token::Type)0);

  // Parsing helpers
  bool parseBinaryOperator(Context * context, const Expression & leftHandSide, Expression & rightHandSide, Token::Type stoppingType);
  Expression parseVector(Context * context);
  Expression parseFunctionParameters(Context * context);
  Expression parseCommaSeparatedList(Context * context);
  void parseReservedFunction(Context * context, Expression & leftHandSide, const Expression::FunctionHelper * const * functionHelper);
  void parseSpecialIdentifier(Context * context, Expression & leftHandSide);
  void parseSequence(Context * context, Expression & leftHandSide, const char name, Token::Type leftDelimiter, Token::Type rightDelimiter);
  void parseCustomIdentifier(Context * context, Expression & leftHandSide, const char * name, size_t length, bool symbolPlusParenthesesAreFunctions);
  void defaultParseLeftParenthesis(Context * context, bool isSystemParenthesis, Expression & leftHandSide, Token::Type stoppingType);

  // Data members
  Status m_status;
    /* m_status is initialized to Status::Progress,
     * is changed to Status::Error if the Parser encounters an error,
     * and is otherwise changed Status::Success. */
  Tokenizer m_tokenizer;
  Token m_currentToken;
  Token m_nextToken;
  bool m_pendingImplicitMultiplication;

  // The array of reserved functions' helpers
  static constexpr const Expression::FunctionHelper * s_reservedFunctions[] = {
    // Ordered according to name and numberOfChildren
    &AbsoluteValue::s_functionHelper,
    &ArcCosine::s_functionHelper,
    &HyperbolicArcCosine::s_functionHelper,
    &ComplexArgument::s_functionHelper,
    &ArcSine::s_functionHelper,
    &HyperbolicArcSine::s_functionHelper,
    &ArcTangent::s_functionHelper,
    &HyperbolicArcTangent::s_functionHelper,
    &BinomCDF::s_functionHelper,
    &BinomialCoefficient::s_functionHelper,
    &BinomPDF::s_functionHelper,
    &Ceiling::s_functionHelper,
    &ConfidenceInterval::s_functionHelper,
    &Conjugate::s_functionHelper,
    &Cosine::s_functionHelper,
    &HyperbolicCosine::s_functionHelper,
    &Determinant::s_functionHelper,
    &Derivative::s_functionHelper,
    &MatrixDimension::s_functionHelper,
    &Factor::s_functionHelper,
    &Floor::s_functionHelper,
    &FracPart::s_functionHelper,
    &GreatCommonDivisor::s_functionHelper,
    &MatrixIdentity::s_functionHelper,
    &ImaginaryPart::s_functionHelper,
    &Integral::s_functionHelper,
    &InvBinom::s_functionHelper,
    &MatrixInverse::s_functionHelper,
    &InvNorm::s_functionHelper,
    &LeastCommonMultiple::s_functionHelper,
    &NaperianLogarithm::s_functionHelper,
    &CommonLogarithm::s_functionHelper,
    &Logarithm::s_functionHelper,
    &NormCDF::s_functionHelper,
    &NormCDF2::s_functionHelper,
    &NormPDF::s_functionHelper,
    &PermuteCoefficient::s_functionHelper,
    &SimplePredictionInterval::s_functionHelper,
    &PredictionInterval::s_functionHelper,
    &Product::s_functionHelper,
    &DivisionQuotient::s_functionHelper,
    &Randint::s_functionHelper,
    &Random::s_functionHelper,
    &RealPart::s_functionHelper,
    &DivisionRemainder::s_functionHelper,
    &NthRoot::s_functionHelper,
    &Round::s_functionHelper,
    &SignFunction::s_functionHelper,
    &Sine::s_functionHelper,
    &HyperbolicSine::s_functionHelper,
    &Sum::s_functionHelper,
    &Tangent::s_functionHelper,
    &HyperbolicTangent::s_functionHelper,
    &MatrixTrace::s_functionHelper,
    &MatrixTranspose::s_functionHelper,
    &SquareRoot::s_functionHelper
  };
  static constexpr const Expression::FunctionHelper * const * s_reservedFunctionsUpperBound = s_reservedFunctions + (sizeof(s_reservedFunctions)/sizeof(Expression::FunctionHelper *));
  /* The method GetReservedFunction passes through the successive
   * entries of the above array in order to determine whether m_currentToken
   * corresponds to an entry. As a helper, the static constexpr
   * s_reservedFunctionsUpperBound marks the end of the array. */
};

}

#endif
