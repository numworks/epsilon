#include <quiz.h>
#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/constructor.h>
#include <poincare_junior/src/layout/constructor.h>
#include <poincare_junior/src/layout/parsing/tokenizer.h>
#include <poincare_junior/src/layout/parsing/parser.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_tokenize) {
  ParsingContext context(ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer("ab*123.45"_l, &context);
  Token token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Times && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 6);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::EndOfStream);

  token = Tokenizer("log2"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction && token.length() == 3);

  token = Tokenizer("tantan"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction && token.length() == 3);

  token = Tokenizer("atan"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction && token.length() == 4);
}

QUIZ_CASE(pcj_layout_parse) {
  Parser("12(123.4567E2 +  0x2a+2*0b0101)"_l).parse().log();
}
