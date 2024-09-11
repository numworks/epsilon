#include <apps/shared/global_context.h>
#include <omg/code_point.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parsing/parsing_context.h>
#include <poincare/src/layout/parsing/rack_parser.h>
#include <poincare/src/layout/parsing/tokenizer.h>
#include <quiz.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_layout_tokenize) {
  Shared::GlobalContext ctx;
  ParsingContext context(&ctx, ParsingContext::ParsingMethod::Classic);
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

  token = Tokenizer(Rack::From("12ᴇ-34"_l), &context).popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 6);
}

bool is_parsable(const Tree* layout) {
  TreeRef expression = RackParser(layout, nullptr).parse();
  return !expression.isUninitialized();
}

// TODO import all the parsing tests from poincare

QUIZ_CASE(pcj_layout_parse) {
  assert_trees_are_equal(RackParser("2^(3+1)^4"_l, nullptr).parse(),
                         KPow(2_e, KPow(KParentheses(KAdd(3_e, 1_e)), 4_e)));
  quiz_assert(is_parsable("12(123 +  0x2a+2*0b0101)"_l));
  // TODO _l with non-ascii codepoints
  quiz_assert(is_parsable("1ᴇ2"_l));
  assert_trees_are_equal(RackParser("12.34ᴇ999"_l, nullptr).parse(),
                         KDecimal(1234_e, -997_e));
  quiz_assert(is_parsable("-1"_l));
  quiz_assert(is_parsable(".1"_l));
  quiz_assert(is_parsable("1+2+3+4+5+6"_l));
  quiz_assert(is_parsable("(1+(2+(3+4)))"_l));
  quiz_assert(is_parsable(KRackL(KFracL("2"_l, "3"_l), KParenthesesL("4"_l))));
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

QUIZ_CASE(pcj_parse_unit) {
  for (const Tree* t : (const Tree*[]){"m"_l, "min"_l, "°"_l, "'"_l, "\""_l}) {
    LayoutSpanDecoder decoder(Rack::From(t));
    quiz_assert(Units::Unit::CanParse(&decoder, nullptr, nullptr));
  }
}

QUIZ_CASE(pcj_assignment_parse) {
  Shared::GlobalContext context;

  assert_trees_are_equal(RackParser("y=zz"_l, &context).parse(),
                         KEqual("y"_e, KMult("z"_e, "z"_e)));

  assert_trees_are_equal(RackParser("y=xxln(x)"_l, &context).parse(),
                         KEqual("y"_e, KMult("x"_e, "x"_e, KLn("x"_e))));

  assert_trees_are_equal(RackParser("f(x)=xxln(x)"_l, &context).parse(),
                         KEqual(KMult("f"_e, KParentheses("x"_e)),
                                KMult("x"_e, "x"_e, KLn("x"_e))));
  // Expected if the "Classic" (default) parsing method is selected on an
  // assignment expression: the left-hand side is parsed as "f*(x)"

  assert_trees_are_equal(
      RackParser("f(x)=xxln(x)"_l, &context,
                 ParsingContext::ParsingMethod::Assignment)
          .parse(),
      KEqual(KFun<"f">("x"_e), KMult("x"_e, "x"_e, KLn("x"_e))));
}
