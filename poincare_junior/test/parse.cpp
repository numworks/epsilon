#include "helper.h"
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

void assert_is_parsable(Node layout) {
  EditionReference expression = Parser(layout).parse();
  quiz_assert(!expression.isUninitialized());
  // expression.log();
}

QUIZ_CASE(pcj_layout_parse) {
  assert_trees_are_equal(Parser("2^(3+1)^4"_l).parse(), Pow(2_e, Pow(Add(3_e, 1_e), 4_e)));
  assert_is_parsable("12(123.4567E2 +  0x2a+2*0b0101)"_l);
  assert_is_parsable("-1"_l);
  assert_is_parsable("1+2+3+4+5+6"_l);
  assert_is_parsable("(1+(2+(3+4)))"_l);
  assert_is_parsable(RackL(FracL("2"_l, "3"_l), ParenthesisL("4"_l)));
  assert_is_parsable(RackL(FracL("2"_l, "3"_l), VertOffL(FracL("4"_l, "5"_l))));
}
