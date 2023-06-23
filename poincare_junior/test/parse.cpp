#include <poincare_junior/include/expression.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/layout/k_creator.h>
#include <poincare_junior/src/layout/parsing/rack_parser.h>
#include <poincare_junior/src/layout/parsing/tokenizer.h>
#include <quiz.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_layout_tokenize) {
  ParsingContext context(ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer("ab*123.45"_l, &context);
  Token token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier &&
              token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier &&
              token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Times && token.length() == 1);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 6);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::EndOfStream);

  token = Tokenizer("log2"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 3);

  token = Tokenizer("tantan"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 3);

  token = Tokenizer("atan"_l, &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 4);
}

bool is_parsable(Node layout) {
  EditionReference expression = RackParser(layout).parse();
  return !expression.isUninitialized();
}

// TODO import all the parsing tests from poincare

QUIZ_CASE(pcj_layout_parse) {
  assert_trees_are_equal(RackParser("2^(3+1)^4"_l).parse(),
                         KPow(2_e, KPow(KAdd(3_e, 1_e), 4_e)));
  quiz_assert(is_parsable("12(123.4567E2 +  0x2a+2*0b0101)"_l));
  quiz_assert(is_parsable("-1"_l));
  quiz_assert(is_parsable("1+2+3+4+5+6"_l));
  quiz_assert(is_parsable("(1+(2+(3+4)))"_l));
  quiz_assert(is_parsable(KRackL(KFracL("2"_l, "3"_l), KParenthesisL("4"_l))));
  quiz_assert(is_parsable(
      KRackL(KFracL("2"_l, "3"_l), KVertOffL(KFracL("4"_l, "5"_l)))));

  quiz_assert(!is_parsable("ln(ln(2"_l));
  quiz_assert(is_parsable("log(2)"_l));
  quiz_assert(is_parsable("log(2,3)"_l));
}
