#include <poincare/src/expression/k_tree.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/parsing/tokenizer.h>
#include <quiz.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_layout_tokenize) {
  ParsingContext context(nullptr, ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer(Rack::From("ab*123.45"_l), &context);
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

  token = Tokenizer(Rack::From("log2"_l), &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 3);

  token = Tokenizer(Rack::From("tantan"_l), &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 3);

  token = Tokenizer(Rack::From("atan"_l), &context).popToken();
  quiz_assert(token.type() == Token::Type::ReservedFunction &&
              token.length() == 4);

  token = Tokenizer(Rack::From("\"apple\""_l), &context).popToken();
  quiz_assert(token.type() == Token::Type::CustomIdentifier &&
              token.length() == 7);

  token =
      Tokenizer(Rack::From("12"_l ^ KCodePointL<u'ᴇ'>() ^ "-34"_l), &context)
          .popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 6);
}

bool is_parsable(const Tree* layout) {
  TreeRef expression = RackParser(layout, nullptr).parse();
  return !expression.isUninitialized();
}

// TODO import all the parsing tests from poincare

QUIZ_CASE(pcj_layout_parse) {
  assert_trees_are_equal(RackParser("2^(3+1)^4"_l, nullptr).parse(),
                         KPow(2_e, KPow(KParenthesis(KAdd(3_e, 1_e)), 4_e)));
  quiz_assert(is_parsable("12(123 +  0x2a+2*0b0101)"_l));
  // TODO _l with non-ascii codepoints
  quiz_assert(is_parsable(
      KRackL(KCodePointL<'1'>(), KCodePointL<u'ᴇ'>(), KCodePointL<'2'>())));
  quiz_assert(is_parsable("-1"_l));
  quiz_assert(is_parsable(".1"_l));
  quiz_assert(is_parsable("1+2+3+4+5+6"_l));
  quiz_assert(is_parsable("(1+(2+(3+4)))"_l));
  quiz_assert(is_parsable(KRackL(KFracL("2"_l, "3"_l), KParenthesisL("4"_l))));
  quiz_assert(is_parsable(
      KRackL(KFracL("2"_l, "3"_l), KSuperscriptL(KFracL("4"_l, "5"_l)))));

  quiz_assert(!is_parsable("ln(ln(2"_l));
  quiz_assert(is_parsable("log(2)"_l));
  quiz_assert(is_parsable("log(2,3)"_l));
  quiz_assert(is_parsable("[[1,2][3,4]]"_l));
  quiz_assert(!is_parsable("[[1,2][3]]"_l));
  quiz_assert(!is_parsable("[]"_l));
  quiz_assert(is_parsable("True xor not False"_l));
  quiz_assert(is_parsable("f(x)"_l));
  quiz_assert(!is_parsable("f(f)"_l));
}
