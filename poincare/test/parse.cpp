#include <apps/shared/global_context.h>
#include <omg/code_point.h>
#include <poincare/old/empty_context.h>
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

void assertLayoutParsesTo(const Tree* layout, const Tree* expected,
                          Poincare::Context* context = nullptr,
                          ParsingContext::ParsingMethod parsingMethod =
                              ParsingContext::ParsingMethod::Classic) {
  Tree* expression = RackParser(layout, context, parsingMethod).parse();
  assert_trees_are_equal(expression, expected);
}

QUIZ_CASE(pcj_parse_layout_tokenize) {
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

bool is_parsable(const Tree* layout, Poincare::Context* context = nullptr) {
  TreeRef expression = RackParser(layout, context).parse();
  return !expression.isUninitialized();
}

// TODO import all the parsing tests from poincare

QUIZ_CASE(pcj_parse_layout) {
  assertLayoutParsesTo("2^(3+1)^4"_l,
                       KPow(2_e, KPow(KParentheses(KAdd(3_e, 1_e)), 4_e)));
  quiz_assert(is_parsable("12(123 +  0x2a+2*0b0101)"_l));
  // TODO _l with non-ascii codepoints
  quiz_assert(is_parsable("1ᴇ2"_l));
  assertLayoutParsesTo("12.34ᴇ999"_l, KDecimal(1234_e, -997_e));
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
  assertLayoutParsesTo("sum"_l ^ KParenthesesL(KRackL(KCurlyBracesL("2"_l))),
                       KListSum(KList(2_e)));
}

QUIZ_CASE(pcj_parse_unit) {
  for (const Tree* t : (const Tree*[]){"m"_l, "min"_l, "°"_l, "'"_l, "\""_l}) {
    LayoutSpanDecoder decoder(Rack::From(t));
    quiz_assert(Units::Unit::CanParse(&decoder, nullptr, nullptr));
  }
}

QUIZ_CASE(pcj_parse_assignment) {
  Shared::GlobalContext context;

  assertLayoutParsesTo("y=zz"_l, KEqual("y"_e, KMult("z"_e, "z"_e)), &context);

  assertLayoutParsesTo("y=xxln(x)"_l,
                       KEqual("y"_e, KMult("x"_e, "x"_e, KLnUser("x"_e))),
                       &context);

  /* Expected if the "Classic" (default) parsing method is selected on an
   * assignment expression: the left-hand side is parsed as "f*(x)". */
  assertLayoutParsesTo("f(x)=xxln(x)"_l,
                       KEqual(KMult("f"_e, KParentheses("x"_e)),
                              KMult("x"_e, "x"_e, KLnUser("x"_e))),
                       &context);

  assertLayoutParsesTo(
      "f(x)=xxln(x)"_l,
      KEqual(KFun<"f">("x"_e), KMult("x"_e, "x"_e, KLnUser("x"_e))), &context,
      ParsingContext::ParsingMethod::Assignment);
}

QUIZ_CASE(pcj_parse_mixed_fraction) {
  assertLayoutParsesTo("1 2/3"_l, KMixedFraction(1_e, KDiv(2_e, 3_e)));
  assertLayoutParsesTo("1"_l ^ KFracL("2"_l, "3"_l),
                       KMixedFraction(1_e, KDiv(2_e, 3_e)));
}

QUIZ_CASE(pcj_parse_parametric) {
  /* Provide a context to parse parametric, or the parsing will always succeed,
   * even with an incorrect symbol. */
  Poincare::EmptyContext context;

  // Must have a symbol as the first argument
  quiz_assert(
      !is_parsable(KRackL(KIntegralL("4"_l, "0"_l, "1"_l, "4"_l)), &context));
  quiz_assert(
      !is_parsable(KRackL(KDiffL("e"_l, "0"_l, "1"_l, "e"_l)), &context));
  quiz_assert(
      !is_parsable(KRackL(KProductL("_s"_l, "0"_l, "1"_l, "_s"_l)), &context));

  // Works with symbol = "x"
  assertLayoutParsesTo(KRackL(KIntegralL("x"_l, "0"_l, "1"_l, "x"_l)),
                       KIntegral("x"_e, 0_e, 1_e, "x"_e), &context);
  // Works with symbol = "t" (even if "t" is also "ton")
  assertLayoutParsesTo(KRackL(KListSequenceL("t"_l, "10"_l, "t"_l)),
                       KListSequence("t"_e, 10_e, "t"_e), &context);
  // Works with symbol = "string"
  assertLayoutParsesTo(KRackL(KSumL("string"_l, "0"_l, "10"_l, "string"_l)),
                       KSum("string"_e, 0_e, 10_e, "string"_e), &context);
}
