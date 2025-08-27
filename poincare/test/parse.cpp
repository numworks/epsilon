#include <apps/global_preferences.h>
#include <omg/code_point.h>
#include <poincare/context.h>
#include <poincare/user_expression.h>
#include <poincare/src/expression/derivation.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/units/k_units.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/k_tree.h>
#include <poincare/src/layout/parser.h>
#include <poincare/src/layout/parsing/parsing_context.h>
#include <poincare/src/layout/parsing/tokenizer.h>
#include <poincare/src/layout/rack_from_text.h>
#include <poincare/src/memory/n_ary.h>
#include <quiz.h>

#include "helper.h"
#include "helpers/symbol_store.h"

using namespace Poincare::Internal;

void assert_tokenizes_as(Token::Type expectedToken, const char* string) {
  ParsingContext parsingContext{.params = {.preserveInput = true}};
  Rack* inputLayout = RackFromText(string);
  Tokenizer tokenizer(inputLayout, &parsingContext);
  quiz_assert_print_if_failure(tokenizer.popToken().type() == expectedToken,
                               string, "expected token", "unexpected token");
  quiz_assert(tokenizer.popToken().type() == Token::Type::EndOfStream);
  inputLayout->removeTree();
}

void assert_tokenizes_as_number(const char* string) {
  assert_tokenizes_as(Token::Type::Number, string);
}

void assert_tokenizes_as_unit(const char* string) {
  assert_tokenizes_as(Token::Type::Unit, string);
}

void assert_tokenizes_as_constant(const char* string) {
  assert_tokenizes_as(Token::Type::Constant, string);
}

void assert_tokenizes_as_undefined_token(const char* string) {
  Rack* inputLayout = RackFromText(string);
  ParsingContext parsingContext;
  Tokenizer tokenizer(inputLayout, &parsingContext);
  while (true) {
    Token token = tokenizer.popToken();
    if (token.type() == Token::Type::Undefined) {
      return;
    }
    quiz_assert_print_if_failure(token.type() != Token::Type::EndOfStream,
                                 string);
  }
}

void assertLayoutParsesTo(
    const Tree* layout, const Tree* expected,
    Poincare::ParserHelper::ParsingParameters params = {}) {
  Tree* expression = Parser::Parse(layout, Poincare::EmptyContext{}, params);
  assert_trees_are_equal(expression, expected);
}

bool is_parsable(const Tree* layout, bool preserveInput = true) {
  TreeRef expression = Parser::Parse(layout, Poincare::EmptyContext{},
                                     {.preserveInput = preserveInput});
  return !expression.isUninitialized();
}

QUIZ_CASE(pcj_parse_layout_tokenize) {
  ParsingContext context;
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

  // Parse as "Ans*5" and not "A*n*s5"
  tokenizer = Tokenizer(Rack::From("Ans5"_l), &context);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::SpecialIdentifier &&
              token.length() == 3);
  token = tokenizer.popToken();
  quiz_assert(token.type() == Token::Type::Number && token.length() == 1);
}

QUIZ_CASE(pcj_parse_tokenize_numbers) {
  assert_tokenizes_as_number("1");
  assert_tokenizes_as_number("12");
  assert_tokenizes_as_number("123");
  assert_tokenizes_as_number("1.3");
  assert_tokenizes_as_number(".3");
  assert_tokenizes_as_number("1.3ᴇ3");
  assert_tokenizes_as_number("12.34ᴇ56");
  assert_tokenizes_as_number(".3ᴇ-32");
  assert_tokenizes_as_number("12.");
  assert_tokenizes_as_number(
      ".99999999999999999999999999999999999999999999999999999999999999999999999"
      "9999999999999999999999999");
  assert_tokenizes_as_number("0.");
  assert_tokenizes_as_number("1.ᴇ-4");
  assert_tokenizes_as_number("1.ᴇ9999");

  assert_tokenizes_as(Token::Type::BinaryNumber, "0b0011010101");
  assert_tokenizes_as(Token::Type::HexadecimalNumber, "0x1234567890ABCDEF");

  assert_tokenizes_as_undefined_token("1ᴇ");
  assert_tokenizes_as_undefined_token("1..");
  assert_tokenizes_as_undefined_token("..");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ");
  assert_tokenizes_as_undefined_token("1.ᴇ");
  assert_tokenizes_as_undefined_token("1ᴇ--4");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ4");
  assert_tokenizes_as_undefined_token("1ᴇ2ᴇ4");
}

QUIZ_CASE(pcj_parse_tokenize_constants) {
  assert_tokenizes_as_constant("_c");
  assert_tokenizes_as_constant("_G");
  assert_tokenizes_as_constant("_e");
  assert_tokenizes_as_constant("_g0");
  assert_tokenizes_as_constant("_k");
  assert_tokenizes_as_constant("_ke");
  assert_tokenizes_as_constant("_me");
  assert_tokenizes_as_constant("_mn");
  assert_tokenizes_as_constant("_mp");
  assert_tokenizes_as_constant("_Na");
  assert_tokenizes_as_constant("_R");
  assert_tokenizes_as_constant("_ε0");
  assert_tokenizes_as_constant("_μ0");
  assert_tokenizes_as_constant("_hplanck");
}

QUIZ_CASE(pcj_parse_layout) {
  assertLayoutParsesTo("2^(3+1)^4"_l,
                       KPow(2_e, KPow(KParentheses(KAdd(3_e, 1_e)), 4_e)));
  quiz_assert(is_parsable("12(123 +  0x2a+2*0b0101)"_l));
  // TODO _l with non-ascii codepoints
  quiz_assert(is_parsable("1ᴇ2"_l));
  assertLayoutParsesTo("12.34ᴇ999"_l, KDecimal(1234_e, -997_e));
  assertLayoutParsesTo("12000.30004"_l, KDecimal(1200030004_e, 5_e));
  assertLayoutParsesTo("12000.3000400000000"_l, KDecimal(1200030004_e, 5_e));
  quiz_assert(is_parsable("-1"_l));
  quiz_assert(is_parsable(".1"_l));
  quiz_assert(is_parsable("1+2+3+4+5+6"_l));
  quiz_assert(is_parsable("(1+(2+(3+4)))"_l));
  quiz_assert(is_parsable(KRackL(KFracL("2"_l, "3"_l), KParenthesesL("4"_l))));
  quiz_assert(is_parsable(
      KRackL(KFracL("2"_l, "3"_l), KSuperscriptL(KFracL("4"_l, "5"_l)))));
  assertLayoutParsesTo(KRackL(KPoint2DL("2"_l, "3"_l)), KPoint(2_e, 3_e));

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
  quiz_assert(!is_parsable("a0123456789012345678901234567890123456789"_l));
}

// TODO: add test for pool memory exhaustion
QUIZ_CASE(pcj_parse_memory_exhaustion) {
  assert_parsed_expression_is("2+3", KAdd(2_e, 3_e));

  int memoryFailureHasBeenHandled = false;
  {
    ExceptionTry {
      Tree* add = SharedTreeStack->pushAdd(0);
      while (true) {
        Tree* child = parse("1+2+3+4+5+6+7+8+9+10");
        NAry::AddChild(add, child);
      }
    }
    ExceptionCatch(type) {
      quiz_assert(type == ExceptionType::TreeStackOverflow);
      memoryFailureHasBeenHandled = true;
    }
  }

  quiz_assert(memoryFailureHasBeenHandled);
  Poincare::Expression e =
      Poincare::UserExpression::Parse("1+1", Poincare::EmptyContext{});
  /* Stupid check to make sure the global variable generated by Bison is not
   * ruining everything */
}

QUIZ_CASE(pcj_parse_numbers) {
  // Parse decimal
  assert_parsed_expression_is("0", 0_e);
  assert_parsed_expression_is("0.1", 0.1_e);
  assert_parsed_expression_is("1.", 1_e);
  assert_parsed_expression_is(".1", 0.1_e);
  assert_parsed_expression_is("0ᴇ2", KDecimal(0_e, -2_e));
  assert_parsed_expression_is("0.1ᴇ2", KDecimal(1_e, -1_e));
  assert_parsed_expression_is("1.ᴇ2", KDecimal(1_e, -2_e));
  assert_parsed_expression_is(".1ᴇ2", KDecimal(1_e, -1_e));
  assert_parsed_expression_is("0ᴇ-2", 0.00_e);
  assert_parsed_expression_is("0.1ᴇ-2", 0.001_e);
  assert_parsed_expression_is("1.ᴇ-2", 0.01_e);
  assert_parsed_expression_is(".1ᴇ-2", 0.001_e);
  assert_parsed_expression_is("0.0000012345678901234",
                              KDecimal(12345678901234_e, 19_e));
  assert_parsed_expression_is("0.00000123456789012345",
                              KDecimal(123456789012345_e, 20_e));
  assert_parsed_expression_is("1234567890123.4",
                              KDecimal(12345678901234_e, 1_e));
  assert_parsed_expression_is("123456789012345.2",
                              KDecimal(1234567890123452_e, 1_e));
  assert_parsed_expression_is("12.34567", KDecimal(1234567_e, 5_e));
  assert_parsed_expression_is(".999999999999990",
                              KDecimal(99999999999999_e, 14_e));
  assert_parsed_expression_is("999.999999999999",
                              KDecimal(999999999999999_e, 12_e));
  assert_parsed_expression_is("2.3ᴇ1000", KDecimal(23_e, -999_e));
  assert_parsed_expression_is("0.23ᴇ-999", KDecimal(23_e, 1001_e));

  // Parse integer
  const char* input = "123456789012345678765434567";
  Tree* a = Integer::Push(input, strlen(input));
  assert_parsed_expression_is(input, a);
  const char* input2 = MaxIntegerString();
  a->removeTree();
  Tree* b = Integer::Push(input2, strlen(input2));
  assert_parsed_expression_is(input2, b);
  b->removeTree();

  // Parsed Based integer
  assert_parsed_expression_is("0b1011", 11_e);
  assert_parsed_expression_is("0x12AC", 4780_e);
}

QUIZ_CASE(pcj_parse_expressions) {
  assert_parsed_expression_is("1", 1_e);
  assert_parsed_expression_is("(1)", KParentheses(1_e));
  assert_parsed_expression_is("((1))", KParentheses(KParentheses(1_e)));
  assert_parsed_expression_is("1+2", KAdd(1_e, 2_e));
  assert_parsed_expression_is("(1)+2", KAdd(KParentheses(1_e), 2_e));
  assert_parsed_expression_is("(1+2)", KParentheses(KAdd(1_e, 2_e)));
  assert_parsed_expression_is("1+2+3", KAdd(1_e, 2_e, 3_e));
  assert_parsed_expression_is("1+2+(3+4)",
                              KAdd(1_e, 2_e, KParentheses(KAdd(3_e, 4_e))));
  assert_parsed_expression_is("+2", 2_e);
  assert_parsed_expression_is("1×2", KMult(1_e, 2_e));
  assert_parsed_expression_is("1×2×3", KMult(1_e, 2_e, 3_e));
  assert_parsed_expression_is("1+2×3", KAdd(1_e, KMult(2_e, 3_e)));
  assert_parsed_expression_is("1/2", KDiv(1_e, 2_e));
  assert_parsed_expression_is("(1/2)", KParentheses(KDiv(1_e, 2_e)));
  assert_parsed_expression_is("1/2/3", KDiv(KDiv(1_e, 2_e), 3_e));
  assert_parsed_expression_is("1/2×3", KMult(KDiv(1_e, 2_e), 3_e));
  assert_parsed_expression_is("(1/2×3)",
                              KParentheses(KMult(KDiv(1_e, 2_e), 3_e)));
  assert_parsed_expression_is("1×2/3", KMult(1_e, KDiv(2_e, 3_e)));
  assert_parsed_expression_is("(1×2/3)",
                              KParentheses(KMult(1_e, KDiv(2_e, 3_e))));
  assert_parsed_expression_is("(1/2/3)",
                              KParentheses(KDiv(KDiv(1_e, 2_e), 3_e)));
  assert_parsed_expression_is("1^2", KPow(1_e, 2_e));
  assert_parsed_expression_is("1^2^3", KPow(1_e, KPow(2_e, 3_e)));
  assert_parsed_expression_is("1=2", KEqual(1_e, 2_e));
  assert_text_not_parsable("=5");
  assert_parsed_expression_is("-1", KOpposite(1_e));
  assert_parsed_expression_is("(-1)", KParentheses(KOpposite(1_e)));
  assert_parsed_expression_is("1-2", KSub(1_e, 2_e));
  assert_parsed_expression_is("-1-2", KSub(KOpposite(1_e), 2_e));
  assert_parsed_expression_is("1-2-3", KSub(KSub(1_e, 2_e), 3_e));
  assert_parsed_expression_is("(1-2)", KParentheses(KSub(1_e, 2_e)));
  assert_parsed_expression_is("1+-2", KAdd(1_e, KOpposite(2_e)));
  assert_parsed_expression_is("--1", KOpposite(KOpposite(1_e)));
  assert_parsed_expression_is("(1+2)-3",
                              KSub(KParentheses(KAdd(1_e, 2_e)), 3_e));
  assert_parsed_expression_is("(2×-3)",
                              KParentheses(KMult(2_e, KOpposite(3_e))));
  assert_parsed_expression_is("1^(2)-3",
                              KSub(KPow(1_e, KParentheses(2_e)), 3_e));
  assert_parsed_expression_is("1^2-3", KSub(KPow(1_e, 2_e), 3_e));
  assert_parsed_expression_is("2^-3", KPow(2_e, KOpposite(3_e)));
  assert_parsed_expression_is("2--2+-1",
                              KAdd(KSub(2_e, KOpposite(2_e)), KOpposite(1_e)));
  assert_parsed_expression_is("2--2×-1",
                              KSub(2_e, KOpposite(KMult(2_e, KOpposite(1_e)))));
  assert_parsed_expression_is("-1^2", KOpposite(KPow(1_e, 2_e)));
  assert_parsed_expression_is("2e^(3)",
                              KMult(2_e, KPow(e_e, KParentheses(3_e))));
  assert_parsed_expression_is("2/-3/-4",
                              KDiv(KDiv(2_e, KOpposite(3_e)), KOpposite(4_e)));
  assert_parsed_expression_is("1×2-3×4",
                              KSub(KMult(1_e, 2_e), KMult(3_e, 4_e)));
  assert_parsed_expression_is("-1×2", KOpposite(KMult(1_e, 2_e)));
  assert_parsed_expression_is("1!", KFact(1_e));
  assert_parsed_expression_is("1+2!", KAdd(1_e, KFact(2_e)));
  assert_parsed_expression_is("1!+2", KAdd(KFact(1_e), 2_e));
  assert_parsed_expression_is("1!+2!", KAdd(KFact(1_e), KFact(2_e)));
  assert_parsed_expression_is("1×2!", KMult(1_e, KFact(2_e)));
  assert_parsed_expression_is("1!×2", KMult(KFact(1_e), 2_e));
  assert_parsed_expression_is("1!×2!", KMult(KFact(1_e), KFact(2_e)));
  assert_parsed_expression_is("1-2!", KSub(1_e, KFact(2_e)));
  assert_parsed_expression_is("1!-2", KSub(KFact(1_e), 2_e));
  assert_parsed_expression_is("1!-2!", KSub(KFact(1_e), KFact(2_e)));
  assert_parsed_expression_is("1/2!", KDiv(1_e, KFact(2_e)));
  assert_parsed_expression_is("1!/2", KDiv(KFact(1_e), 2_e));
  assert_parsed_expression_is("1!/2!", KDiv(KFact(1_e), KFact(2_e)));
  assert_parsed_expression_is("1^2!", KPow(1_e, KFact(2_e)));
  assert_parsed_expression_is("1!^2", KPow(KFact(1_e), 2_e));
  assert_parsed_expression_is("1!^2!", KPow(KFact(1_e), KFact(2_e)));
  assert_parsed_expression_is("(1)!", KFact(KParentheses(1_e)));
  assert_text_not_parsable("1+");
  assert_text_not_parsable(")");
  assert_text_not_parsable(")(");
  assert_text_not_parsable("()");
  assert_text_not_parsable("(1");
  assert_text_not_parsable("1)");
  assert_text_not_parsable("1//2");
  assert_text_not_parsable("×1");
  assert_text_not_parsable("1^^2");
  assert_text_not_parsable("^1");
  assert_text_not_parsable("t0000000");
  assert_text_not_parsable("[[t0000000[");
  assert_text_not_parsable("0→x=0");
  assert_text_not_parsable("0→3=0");
  assert_text_not_parsable("1ᴇ2ᴇ3");
  assert_text_not_parsable("0b001112");
  assert_text_not_parsable("0x123G");
  assert_text_not_parsable("re^(im(,0))");
}

QUIZ_CASE(pcj_parse_matrices) {
  SimpleKTrees::KTree m1 = KMatrix<1, 1>(1_e);
  assert_parsed_expression_is("[[1]]", m1);
  assert_parsed_expression_is("[[1,2,3]]", KMatrix<1, 3>(1_e, 2_e, 3_e));
  assert_parsed_expression_is("[[1,2,3][4,5,6]]",
                              KMatrix<2, 3>(1_e, 2_e, 3_e, 4_e, 5_e, 6_e));
  assert_parsed_expression_is("[[1,[[1]]]]", KMatrix<1, 2>(1_e, m1));
  assert_text_not_parsable("[");
  assert_text_not_parsable("]");
  assert_text_not_parsable("[[");
  assert_text_not_parsable("][");
  assert_text_not_parsable("[]");
  assert_text_not_parsable("[1]");
  assert_text_not_parsable("[[1,2],[3]]");
  assert_text_not_parsable("[[]");
  assert_text_not_parsable("[[1]");
  assert_text_not_parsable("[1]]");
  assert_text_not_parsable("[[,]]");
  assert_text_not_parsable("[[1,]]");
  assert_text_not_parsable(",");
  assert_text_not_parsable("[,]");
}

QUIZ_CASE(pcj_parse_lists) {
  assert_parsed_expression_is("{}", KList());
  assert_parsed_expression_is("{1}", KList(1_e));
  assert_parsed_expression_is("{1,2,3}", KList(1_e, 2_e, 3_e));
  assert_parsed_expression_is("{1,{2,3},4}", KList(1_e, KList(2_e, 3_e), 4_e));
  assert_text_not_parsable("{");
  assert_text_not_parsable("{{");
  assert_text_not_parsable("}");
  assert_text_not_parsable("}}");
  assert_text_not_parsable("}{");
  assert_text_not_parsable("{,}");
  assert_text_not_parsable("{1,2,}");
  assert_text_not_parsable("{1,,3}");
  assert_text_not_parsable("{,2,3}");

  // List access
  assert_parsed_expression_is("{2}(1)", KListElement(KList(2_e), 1_e));
  assert_parsed_expression_is("{3,4,5}(1,2)",
                              KListSlice(KList(3_e, 4_e, 5_e), 1_e, 2_e));
}

QUIZ_CASE(pcj_parse_store) {
  assert_parsed_expression_is("1→a", KStore(1_e, "a"_e));
  assert_parsed_expression_is("t→a", KStore(KUnits::ton, "a"_e));
  assert_parsed_expression_is("1→g", KStore(1_e, "g"_e));
  assert_parsed_expression_is("1→f(x)", KStore(1_e, KFun<"f">("x"_e)));
  assert_parsed_expression_is("x→f(x)", KStore("x"_e, KFun<"f">("x"_e)));
  assert_parsed_expression_is("n→f(x)", KStore("n"_e, KFun<"f">("x"_e)));
  assert_parsed_expression_is("ab→f(ab)", KStore("ab"_e, KFun<"f">("ab"_e)));
  assert_parsed_expression_is("ab→f(x)",
                              KStore(KMult("a"_e, "b"_e), KFun<"f">("x"_e)));
  assert_parsed_expression_is("t→f(t)", KStore("t"_e, KFun<"f">("t"_e)));
  assert_parsed_expression_is("t→f(x)", KStore(KUnits::ton, KFun<"f">("x"_e)));
  assert_parsed_expression_is("[[x]]→f(x)",
                              KStore(KMatrix<1, 1>("x"_e), KFun<"f">("x"_e)));
  assert_text_not_parsable("a→b→c");
  assert_text_not_parsable("0→0→c");
  assert_text_not_parsable("1→");
  assert_text_not_parsable("→2");
  assert_text_not_parsable("(1→a)");
  assert_text_not_parsable("1→\1");  // UnknownX
  assert_text_not_parsable("1→\2");  // UnknownN
  assert_text_not_parsable("1→acos");
  assert_text_not_parsable("x→tan(x)");
  assert_text_not_parsable("3→min");
  assert_text_not_parsable("1→f(f)");

  quiz_assert(is_parsable("0→a"_l));
  quiz_assert(is_parsable("2+3→b"_l));
  quiz_assert(!is_parsable("2+(3→b)"_l));
  quiz_assert(!is_parsable("2→b+3"_l));
  quiz_assert(!is_parsable("a→b-c"_l));
  quiz_assert(is_parsable("a-b→c"_l));
  quiz_assert(!is_parsable("2^(0→a)"_l));
  quiz_assert(!is_parsable("log(1→c)"_l));
}

// TODO: unify style layout or string parsing
QUIZ_CASE(pcj_parse_unit) {
  for (const Tree* t : (const Tree*[]){"m"_l, "min"_l, "°"_l, "'"_l, "\""_l}) {
    LayoutSpanDecoder decoder(Rack::From(t));
    quiz_assert(Units::Unit::CanParse(&decoder, nullptr, nullptr));
  }

  // Non-existing units are not parsable
  assert_text_not_parsable("_n");
  assert_text_not_parsable("_a");

  // Any identifier starting with '_' is tokenized as a unit
  assert_tokenizes_as_unit("_m");
  assert_tokenizes_as_unit("_km");
  assert_tokenizes_as_unit("_pm");
  assert_tokenizes_as_unit("_A");

  // Can parse implicit multiplication with units
  assert_parsed_expression_is("_kmπ", KMult(KUnits::kilometer, π_e));
  assert_parsed_expression_is("π_km", KMult(π_e, KUnits::kilometer));
  assert_parsed_expression_is("_s_km",
                              KMult(KUnits::second, KUnits::kilometer));
  assert_parsed_expression_is("3_s", KMult(3_e, KUnits::second));

  // "l" is alias for "L"
  assert_parsed_expression_is("l", KUnits::liter);
  assert_parsed_expression_is("_l", KUnits::liter);

  assert_parsed_expression_is("kmπ", KMult(KUnits::kilometer, π_e));
  assert_parsed_expression_is("πkm", KMult(π_e, KUnits::kilometer));
  assert_parsed_expression_is("skm", KMult(KUnits::second, KUnits::kilometer));
  assert_parsed_expression_is("3s", KMult(3_e, KUnits::second));

  // Angle units
  assert_parsed_expression_is("3°", KMult(3_e, KUnits::degree));
  assert_parsed_expression_is("3\"", KMult(3_e, KUnits::arcSecond));
  assert_parsed_expression_is("3\"+a\"", KAdd(KMult(3_e, KUnits::arcSecond),
                                              KMult("a"_e, KUnits::arcSecond)));
  assert_parsed_expression_is("3\"abc\"", KMult(3_e, "\"abc\""_e));
  assert_parsed_expression_is("x\"abc\"", KMult("x"_e, "\"abc\""_e));
  assert_parsed_expression_is("\"0", KMult(KUnits::arcSecond, 0_e));

  // Implicit addition
  assert_parsed_expression_is(
      "3h40min5s", KAdd(KMult(3_e, KUnits::hour), KMult(40_e, KUnits::minute),
                        KMult(5_e, KUnits::second)));
  assert_parsed_expression_is(
      "5mi4yd2ft3in", KAdd(KMult(5_e, KUnits::mile), KMult(4_e, KUnits::yard),
                           KMult(2_e, KUnits::foot), KMult(3_e, KUnits::inch)));
  assert_parsed_expression_is(
      "5lb4oz", KAdd(KMult(5_e, KUnits::pound), KMult(4_e, KUnits::ounce)));
  assert_parsed_expression_is(
      "5°4'3\"", KAdd(KMult(5_e, KUnits::degree), KMult(4_e, KUnits::arcMinute),
                      KMult(3_e, KUnits::arcSecond)));
  // Works with decimal numbers
  assert_parsed_expression_is(
      "3.5h40.3min5.1s",
      KAdd(KMult(3.5_e, KUnits::hour), KMult(40.3_e, KUnits::minute),
           KMult(5.1_e, KUnits::second)));
  // Has priority over other operation
  assert_parsed_expression_is(
      "2×3h5s",
      KMult(2_e, KAdd(KMult(3_e, KUnits::hour), KMult(5_e, KUnits::second))));
  assert_parsed_expression_is(
      "4h-3h5s",
      KSub(KMult(4_e, KUnits::hour),
           KAdd(KMult(3_e, KUnits::hour), KMult(5_e, KUnits::second))));
  // Does not work with fractions or other numbers
  assert_parsed_expression_is(
      "(3/5)hπs",
      KMult(KParentheses(KDiv(3_e, 5_e)), KUnits::hour, π_e, KUnits::second));
  // Does not work with any unit
  assert_parsed_expression_is("3km4m", KMult(3_e, "k"_e, "m4"_e,
                                             KUnits::meter));  // 3*k*m4*m
  // Does not work in any order
  assert_parsed_expression_is("3s5h",
                              KMult(3_e, "s5"_e, KUnits::hour));  // 3*s5*h
  // Does not allow repetition
  assert_parsed_expression_is("3s5s",
                              KMult(3_e, "s5"_e, KUnits::second));  // 3*s5*s
}

QUIZ_CASE(pcj_parse_unit_convert) {
  assert_parsed_expression_is(
      "3t→kg", KUnitConversion(KMult(3_e, KUnits::ton), KUnits::kilogram));
  assert_parsed_expression_is(
      "3t→kg^2",
      KUnitConversion(KMult(3_e, KUnits::ton), KPow(KUnits::kilogram, 2_e)));
  assert_parsed_expression_is(
      "3°4'→rad", KUnitConversion(KAdd(KMult(3_e, KUnits::degree),
                                       KMult(4_e, KUnits::arcMinute)),
                                  KUnits::radian));
  assert_text_not_parsable("1→_m");
  assert_text_not_parsable("2t→3kg");
  assert_text_not_parsable("1→2");
  assert_text_not_parsable("1→a+a");
  assert_text_not_parsable("1→inf");
  assert_text_not_parsable("1→∞");
  assert_text_not_parsable("1→undef");
  assert_text_not_parsable("1→π");
  assert_text_not_parsable("1→i");
  assert_text_not_parsable("1→e");
  assert_text_not_parsable("1→3_m");
  assert_text_not_parsable("4→_km/_m");
  assert_text_not_parsable("3×_min→_s+1-1");
  assert_text_not_parsable("0→_K");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("_m→a", symbolStore);
  PoincareTest::store("_m→b", symbolStore);
  assert_text_not_parsable("1_km→a×b", symbolStore);
  PoincareTest::store("2→a", symbolStore);
  assert_text_not_parsable("3_m→a×_km", symbolStore);
  PoincareTest::store("2→f(x)", symbolStore);
  assert_text_not_parsable("3_m→f(2)×_km");
}

// TODO: unify style layout or string parsing
QUIZ_CASE(pcj_parse_assignment) {
  assertLayoutParsesTo("y=zz"_l, KEqual("y"_e, KMult("z"_e, "z"_e)));

  assertLayoutParsesTo("y=xxln(x)"_l,
                       KEqual("y"_e, KMult("x"_e, "x"_e, KLnUser("x"_e))));

  /* Expected if the "Classic" (default) parsing method is selected on an
   * assignment expression: the left-hand side is parsed as "f*(x)". */
  assertLayoutParsesTo("f(x)=xxln(x)"_l,
                       KEqual(KMult("f"_e, KParentheses("x"_e)),
                              KMult("x"_e, "x"_e, KLnUser("x"_e))));

  assertLayoutParsesTo(
      "f(x)=xxln(x)"_l,
      KEqual(KFun<"f">("x"_e), KMult("x"_e, "x"_e, KLnUser("x"_e))),
      {.isAssignment = true});

  assert_parsed_expression_is(
      "f(x)=xy",
      KEqual(KMult("f"_e, KParentheses("x"_e)), KMult("x"_e, "y"_e)));
  assert_parsed_expression_is("f(x)=xy",
                              KEqual(KFun<"f">("x"_e), KMult("x"_e, "y"_e)),
                              {.isAssignment = true});

  /* Ensure y=ax is not understood as "y"="ax" but "y"="a"*"x" when parsing for
   * assignment. (The "parsing for assignment" should apply only to left
   * handside of the comparison). */
  assert_parsed_expression_is("y=ax", KEqual("y"_e, KMult("a"_e, "x"_e)),
                              {.isAssignment = true});

  assert_parsed_expression_is(
      "f(x)=4=3", KLogicalAnd(KEqual(KMult("f"_e, KParentheses("x"_e)), 4_e),
                              KEqual(4_e, 3_e)));
  assert_parsed_expression_is("f(x)=4=3",
                              KEqual(KFun<"f">("x"_e), KEqual(4_e, 3_e)),
                              {.isAssignment = true});
}

QUIZ_CASE(pcj_parse_mixed_fraction) {
  assertLayoutParsesTo("1"_l ^ KFracL("2"_l, "3"_l),
                       KMixedFraction(1_e, KDiv(2_e, 3_e)),
                       {.preserveInput = true});

  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::US);
  assert(Poincare::SharedPreferences->mixedFractionsAreEnabled());
  assert_parsed_expression_is("1 2/3", KMixedFraction(1_e, KDiv(2_e, 3_e)));
  assert_parsed_expression_is(
      "1((2)/(3))",
      KMult(1_e, KParentheses(KDiv(KParentheses(2_e), KParentheses(3_e)))));
  assert_parsed_expression_is("1(2/3)2",
                              KMult(1_e, KParentheses(KDiv(2_e, 3_e)), 2_e));
  assert_parsed_expression_is(
      "1(2/3)(2/3)",
      KMult(1_e, KParentheses(KDiv(2_e, 3_e)), KParentheses(KDiv(2_e, 3_e))));
  assert_parsed_expression_is("1(e/3)",
                              KMult(1_e, KParentheses(KDiv(e_e, 3_e))));
  assert_parsed_expression_is("1(2.5/3)",
                              KMult(1_e, KParentheses(KDiv(2.5_e, 3_e))));

  GlobalPreferences::SharedGlobalPreferences()->setCountry(I18n::Country::WW);

  assert_text_not_parsable("1 1/2");
}

QUIZ_CASE(pcj_parse_derivative_apostrophe) {
  constexpr SimpleKTrees::KTree apostropheUnit = KUnits::arcMinute;
  constexpr SimpleKTrees::KTree quoteUnit = KUnits::arcSecond;

  // Reserved function
  assert_text_not_parsable("cos'(x)");
  assert_text_not_parsable("cos\"(x)");

  // No symbols defined
  assert_parsed_expression_is(
      "f'(x)", KMult("f"_e, apostropheUnit, KParentheses("x"_e)));
  assert_parsed_expression_is("f\"(x)",
                              KMult("f"_e, quoteUnit, KParentheses("x"_e)));
  assert_parsed_expression_is(
      "f''(x)",
      KMult("f"_e, apostropheUnit, apostropheUnit, KParentheses("x"_e)));
  assert_parsed_expression_is(
      "f^(1)(x)", KMult(KPow("f"_e, KParentheses(1_e)), KParentheses("x"_e)));
  assert_parsed_expression_is(
      "f^(2)(x)", KMult(KPow("f"_e, KParentheses(2_e)), KParentheses("x"_e)));
  assert_parsed_expression_is(
      "f^(3)(x)", KMult(KPow("f"_e, KParentheses(3_e)), KParentheses("x"_e)));
  assert_parsed_expression_is("f^1(x)",
                              KMult(KPow("f"_e, 1_e), KParentheses("x"_e)));
  assert_parsed_expression_is("f^2(x)",
                              KMult(KPow("f"_e, 2_e), KParentheses("x"_e)));
  assert_parsed_expression_is("f^3(x)",
                              KMult(KPow("f"_e, 3_e), KParentheses("x"_e)));
  assert_parsed_expression_is("f'", KMult("f"_e, apostropheUnit));
  assert_parsed_expression_is("f\"", KMult("f"_e, quoteUnit));
  assert_parsed_expression_is("f''",
                              KMult("f"_e, apostropheUnit, apostropheUnit));
  assert_parsed_expression_is("f^(2)", KPow("f"_e, KParentheses(2_e)));

  PoincareTest::SymbolStore symbolStore;
  // Function defined
  PoincareTest::store("undef→f(x)", symbolStore);
  assert_parsed_expression_is(
      "f'(x)",
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 1_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      symbolStore);
  assert_parsed_expression_is(
      "f\"(x)",
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 2_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      symbolStore);
  assert_parse_to_same_expression("f'(x)", "f^(1)(x)", symbolStore);
  assert_parse_to_same_expression("f\"(x)", "f^(2)(x)", symbolStore);
  assert_parse_to_same_expression("f''(x)", "f^(2)(x)", symbolStore);
  assert_parse_to_same_expression("f'''(x)", "f^(3)(x)", symbolStore);
  assert_parse_to_same_expression("f\"\"(x)", "f^(4)(x)", symbolStore);
  assert_parse_to_same_expression("f'\"'(x)", "f^(4)(x)", symbolStore);
  assert_parsed_expression_is(
      "f^(3)(x)",
      KDiff(Derivation::k_functionDerivativeVariable, "x"_e, 3_e,
            KFun<"f">(Derivation::k_functionDerivativeVariable)),
      symbolStore);
  assert_parsed_expression_is(
      "f^(3/2)(x)",
      KMult(KPow("f"_e, KParentheses(KDiv(3_e, 2_e))), KParentheses("x"_e)),
      symbolStore);
  assert_parsed_expression_is("f'", KMult("f"_e, apostropheUnit), symbolStore);
  assert_parsed_expression_is("f\"", KMult("f"_e, quoteUnit), symbolStore);
  assert_parsed_expression_is(
      "f''", KMult("f"_e, apostropheUnit, apostropheUnit), symbolStore);
  assert_parsed_expression_is("f^(2)", KPow("f"_e, KParentheses(2_e)),
                              symbolStore);
  symbolStore.reset();
  // Expression defined
  PoincareTest::store("undef→f", symbolStore);
  assert_parsed_expression_is("f'", KMult("f"_e, apostropheUnit), symbolStore);
  assert_parsed_expression_is("f\"", KMult("f"_e, quoteUnit), symbolStore);
  assert_parsed_expression_is(
      "f''", KMult("f"_e, apostropheUnit, apostropheUnit), symbolStore);
  assert_parsed_expression_is("f^(1)", KPow("f"_e, KParentheses(1_e)),
                              symbolStore);
  assert_parsed_expression_is("f^(2)", KPow("f"_e, KParentheses(2_e)),
                              symbolStore);
  assert_parsed_expression_is("f^(3)", KPow("f"_e, KParentheses(3_e)),
                              symbolStore);
  symbolStore.reset();
  assert_text_not_parsable(
      "→M^\U00000012√\U00000012^\U000000122\U00000013\U00000013\U00000013",
      symbolStore);
}

QUIZ_CASE(pcj_parse_parametric) {
  // Must have a symbol as the first argument
  quiz_assert(!is_parsable(KRackL(KIntegralL("4"_l, "0"_l, "1"_l, "4"_l))));
  quiz_assert(!is_parsable(KRackL(KDiffL("e"_l, "0"_l, "1"_l, "e"_l))));
  quiz_assert(!is_parsable(KRackL(KProductL("_s"_l, "0"_l, "1"_l, "_s"_l))));

  // Works with symbol = "x"
  assertLayoutParsesTo(KRackL(KIntegralL("x"_l, "0"_l, "1"_l, "x"_l)),
                       KIntegral("x"_e, 0_e, 1_e, "x"_e));
  // Works with symbol = "t" (even if "t" is also "ton")
  assertLayoutParsesTo(KRackL(KListSequenceL("t"_l, "10"_l, "t"_l)),
                       KListSequence("t"_e, 10_e, "t"_e));
  // Works with symbol = "string"
  assertLayoutParsesTo(KRackL(KSumL("string"_l, "0"_l, "10"_l, "string"_l)),
                       KSum("string"_e, 0_e, 10_e, "string"_e));

  quiz_assert(is_parsable("sum(k,k,0,1)"_l));
  quiz_assert(!is_parsable("sum(000000),"_l));
  quiz_assert(!is_parsable("sum"_l));
  quiz_assert(!is_parsable("sum×0"_l));
  quiz_assert(!is_parsable("sum("_l));
  quiz_assert(!is_parsable("sum()"_l));
  quiz_assert(!is_parsable("sum(k,)"_l));
  quiz_assert(!is_parsable("sum(k,k"_l));
  quiz_assert(!is_parsable("sum(k,k,"_l));
  quiz_assert(!is_parsable("sum(k,k,0"_l));
  quiz_assert(!is_parsable("sum(k,k,0,"_l));
  quiz_assert(!is_parsable("sum(k,k,0,1"_l));
  quiz_assert(is_parsable("sum"_l ^ KParenthesesL("k,k,0,1"_l)));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("000000"_l) ^ ","_l));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL(KRackL())));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("k,"_l)));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("k,k"_l)));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("k,k,"_l)));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("k,k,0"_l)));
  quiz_assert(!is_parsable("sum"_l ^ KParenthesesL("k,k,0,"_l)));
}

QUIZ_CASE(pcj_parse_sequences) {
#if POINCARE_SEQUENCE
  Tree* expected = SharedTreeStack->pushUserSequence("u");
  (4_e)->cloneTree();
  assertLayoutParsesTo("u(4)"_l, expected, {.preserveInput = true});
  expected->removeTree();
  quiz_assert(is_parsable("f'(4)"_l));
  quiz_assert(!is_parsable("u'(4)"_l));
#endif
}

QUIZ_CASE(pcj_parse_identifiers) {
  // Custom variables without storage
  assert_parsed_expression_is("a", "a"_e);
  assert_parsed_expression_is("x", "x"_e);
  assert_parsed_expression_is("\"toot\"", "\"toot\""_e);
  assert_parsed_expression_is("\"tot12\"", "\"tot12\""_e);
  assert_parsed_expression_is("f(f)", KMult("f"_e, KParentheses("f"_e)));
  assert_parsed_expression_is("f((1))",
                              KMult("f"_e, KParentheses(KParentheses(1_e))));
  assert_text_not_parsable("_a");
  // No symbol of length > 6
  assert_text_not_parsable("abcdefgh", Poincare::EmptyContext{},
                           {.preserveInput = true});
  assert_text_not_parsable("f(1,2,3)");

  // User-defined functions
  assert_parsed_expression_is("1→f(x)", KStore(1_e, KFun<"f">("x"_e)));
  assert_parsed_expression_is("1→ab12AB_(x)",
                              KStore(1_e, KFun<"ab12AB_">("x"_e)));

  // Reserved symbols
  assert_parsed_expression_is("Ans", "Ans"_e);
  assert_parsed_expression_is("ans", "Ans"_e);
  assert_parsed_expression_is("i", i_e);
  assert_parsed_expression_is("π", π_e);
  assert_parsed_expression_is("pi", π_e);
  assert_parsed_expression_is("e", e_e);
  assert_parsed_expression_is("∞", KInf);
  assert_parsed_expression_is("+∞", KInf);
  assert_parsed_expression_is("inf", KInf);
  assert_parsed_expression_is("+inf", KInf);
  assert_parsed_expression_is("undef", KUndef);

  // Reserved functions
  assert_parsed_expression_is("arccos(1)", KACos(1_e));
  assert_parsed_expression_is("acos(1)", KACos(1_e));
  assert_parsed_expression_is("cos^(-1)(1)", KACos(1_e));
  assert_parsed_expression_is("arcosh(1)", KArCosH(1_e));
  assert_parsed_expression_is("arccot(1)", KACot(1_e));
  assert_parsed_expression_is("arccsc(1)", KACsc(1_e));
  assert_parsed_expression_is("abs(1)", KAbs(1_e));
  assert_parsed_expression_is("arg(1)", KArg(1_e));
  assert_parsed_expression_is("arcsec(1)", KASec(1_e));
  assert_parsed_expression_is("asin(1)", KASin(1_e));
  assert_parsed_expression_is("arcsin(1)", KASin(1_e));
  assert_parsed_expression_is("sin^(-1)(1)", KASin(1_e));
  assert_parsed_expression_is("arsinh(1)", KArSinH(1_e));
  assert_parsed_expression_is("arctan(1)", KATan(1_e));
  assert_parsed_expression_is("atan(1)", KATan(1_e));
  assert_parsed_expression_is("tan^(-1)(1)", KATan(1_e));
  assert_parsed_expression_is("artanh(1)", KArTanH(1_e));
  assert_parsed_expression_is("binomial(2,1)", KBinomial(2_e, 1_e));
  assert_parsed_expression_is("ceil(1)", KCeil(1_e));
  assert_parsed_expression_is("cross(1,1)", KCross(1_e, 1_e));
  assert_text_not_parsable("diff(1,2,3)");
  assert_text_not_parsable("diff(0,_s,0)");
  assert_parsed_expression_is("diff(1,x,3)", KDiff("x"_e, 3_e, 1_e, 1_e));
  assert_parsed_expression_is("diff(xa,xa,3)", KDiff("xa"_e, 3_e, 1_e, "xa"_e));
  assert_parsed_expression_is(
      "diff(xaxaxa,xaxax,1)",
      KDiff("xaxax"_e, 1_e, 1_e, KMult("xaxax"_e, "a"_e)));
  assert_parsed_expression_is(
      "diff(diff(yb,yb,xa),xa,3)",
      KDiff("xa"_e, 3_e, 1_e, KDiff("yb"_e, "xa"_e, 1_e, "yb"_e)));
  assert_parsed_expression_is(
      "diff([[2,t]],t,4)", KDiff("t"_e, 4_e, 1_e, KMatrix<1, 2>(2_e, "t"_e)));
  assert_parsed_expression_is("dim(1)", KDim(1_e));
  assert_parsed_expression_is("conj(1)", KConj(1_e));
  assert_parsed_expression_is("cot(1)", KCot(1_e));
  assert_parsed_expression_is("det(1)", KDet(1_e));
  assert_parsed_expression_is("dot(1,1)", KDot(1_e, 1_e));
  assert_parsed_expression_is("cos(1)", KCos(1_e));
  assert_parsed_expression_is("cosh(1)", KCosH(1_e));
  assert_parsed_expression_is("csc(1)", KCsc(1_e));
  assert_parsed_expression_is("factor(1)", KFactor(1_e));
  assert_parsed_expression_is("floor(1)", KFloor(1_e));
  assert_parsed_expression_is("frac(1)", KFrac(1_e));
  assert_parsed_expression_is("gcd(1,2,3)", KGCD(1_e, 2_e, 3_e));
  assert_text_not_parsable("gcd(1)");
  assert_text_not_parsable("gcd()");
  assert_parsed_expression_is("im(1)", KIm(1_e));
  assert_parsed_expression_is("int(1,x,2,3)", KIntegral("x"_e, 2_e, 3_e, 1_e));
  assert_text_not_parsable("int(1,2,3,4)");
  assert_text_not_parsable("int(1,_s,3,4)");
  assert_parsed_expression_is("inverse(1)", KInverse(1_e));
  assert_parsed_expression_is("lcm(1,2,3)", KLCM(1_e, 2_e, 3_e));
  assert_text_not_parsable("lcm(1)");
  assert_text_not_parsable("lcm()");
  assert_parsed_expression_is("ln(1)", KLnUser(1_e));
  assert_parsed_expression_is("log(1)", KLog(1_e));
  assert_parsed_expression_is("log(1,2)", KLogBase(1_e, 2_e));
  assert_parsed_expression_is("norm(1)", KNorm(1_e));
  assert_parsed_expression_is("permute(2,1)", KPermute(2_e, 1_e));
  assert_parsed_expression_is("product(1,n,2,3)",
                              KProduct("n"_e, 2_e, 3_e, 1_e));
  assert_text_not_parsable("product(1,2,3,4)");
  assert_text_not_parsable("product(1,_s,3,4)");
  assert_parsed_expression_is("quo(1,2)", KQuo(1_e, 2_e));
  assert_parsed_expression_is("random()", KRandom);
  assert_parsed_expression_is("randint(1,2)", KRandInt(1_e, 2_e));
  assert_parsed_expression_is("randint(2)", KRandInt(1_e, 2_e));
  assert_parsed_expression_is("randintnorep(1,10,3)",
                              KRandIntNoRep(1_e, 10_e, 3_e));
  assert_parsed_expression_is("re(1)", KRe(1_e));
  assert_parsed_expression_is("ref(1)", KRef(1_e));
  assert_parsed_expression_is("rem(1,2)", KRem(1_e, 2_e));
  assert_parsed_expression_is("root(1,2)", KRoot(1_e, 2_e));
  assert_parsed_expression_is("round(1,2)", KRound(1_e, 2_e));
  assert_parsed_expression_is("rref(1)", KRref(1_e));
  assert_parsed_expression_is("sec(1)", KSec(1_e));
  assert_parsed_expression_is("sin(1)", KSin(1_e));
  assert_parsed_expression_is("sign(1)", KSignUser(1_e));
  assert_parsed_expression_is("sinh(1)", KSinH(1_e));
  assert_parsed_expression_is("sum(1,n,2,3)", KSum("n"_e, 2_e, 3_e, 1_e));
  assert_text_not_parsable("sum(1,2,3,4)");
  assert_text_not_parsable("sum(1,_s,3,4)");
  assert_parsed_expression_is("tan(1)", KTan(1_e));
  assert_parsed_expression_is("tanh(1)", KTanH(1_e));
  assert_parsed_expression_is("trace(1)", KTrace(1_e));
  assert_parsed_expression_is("transpose(1)", KTranspose(1_e));
  assert_parsed_expression_is("√(1)", KSqrt(1_e));
  assert_text_not_parsable("cos(1,2)");
  assert_text_not_parsable("quo()");
  assert_text_not_parsable("log(1,2,3)");
  assert_text_not_parsable("sinh^(-1)(2)");

  // Powered reserved functions (integer powers other than -1)
  assert_parsed_expression_is("cos^(0)(1)", KPow(KCos(1_e), 0_e));
  assert_parsed_expression_is("sin^(1)(1)", KPow(KSin(1_e), 1_e));
  assert_parsed_expression_is("tan^(2)(1)", KPow(KTan(1_e), 2_e));
  assert_parsed_expression_is("arccos^(3)(1)", KPow(KACos(1_e), 3_e));
  assert_parsed_expression_is("arcsin^(4)(1)", KPow(KASin(1_e), 4_e));
  assert_parsed_expression_is("arctan^(5)(1)", KPow(KATan(1_e), 5_e));
  assert_parsed_expression_is("cot^(6)(1)", KPow(KCot(1_e), 6_e));
  assert_parsed_expression_is("sec^(7)(1)", KPow(KSec(1_e), 7_e));
  assert_parsed_expression_is("csc^(8)(1)", KPow(KCsc(1_e), 8_e));
  assert_parsed_expression_is("arccot^(9)(1)", KPow(KACot(1_e), 9_e));
  assert_parsed_expression_is("arcsec^(10)(1)", KPow(KASec(1_e), 10_e));
  assert_parsed_expression_is("arccsc^(11)(1)", KPow(KACsc(1_e), 11_e));
  assert_parsed_expression_is("cosh^(-2)(1)", KPow(KCosH(1_e), -2_e));
  assert_parsed_expression_is("sinh^(-3)(1)", KPow(KSinH(1_e), -3_e));
  assert_parsed_expression_is("tanh^(-4)(1)", KPow(KTanH(1_e), -4_e));
  assert_parsed_expression_is("arsinh^(-5)(1)", KPow(KArSinH(1_e), -5_e));
  assert_parsed_expression_is("arcosh^(-6)(1)", KPow(KArCosH(1_e), -6_e));
  assert_parsed_expression_is("artanh^(-7)(1)", KPow(KArTanH(1_e), -7_e));

  assert_text_not_parsable("ln^(2)(2)");

  PoincareTest::SymbolStore symbolStore;
  PoincareTest::store("0→ab", symbolStore);
  PoincareTest::store("x→bacos(x)", symbolStore);
  PoincareTest::store("0→azfoo", symbolStore);
  PoincareTest::store("x→foobar(x)", symbolStore);
  PoincareTest::store("0→a3b", symbolStore);
  assert_parsed_expression_is("xyz", KMult("x"_e, "y"_e, "z"_e), symbolStore);
  assert_parsed_expression_is("xy123z", KMult("x"_e, "y123"_e, "z"_e),
                              symbolStore);
  assert_parsed_expression_is("3→xyz", KStore(3_e, "xyz"_e), symbolStore);
  assert_parsed_expression_is("ab", "ab"_e, symbolStore);
  assert_parsed_expression_is("ab3", KMult("a"_e, "b3"_e), symbolStore);
  assert_parsed_expression_is("a3b", "a3b"_e, symbolStore);
  assert_parsed_expression_is("aacos(x)", KMult("a"_e, KACos("x"_e)),
                              symbolStore);
  assert_parsed_expression_is("bacos(x)", KFun<"bacos">("x"_e), symbolStore);
  assert_parsed_expression_is(
      "azfoobar(x)", KMult("a"_e, "z"_e, KFun<"foobar">("x"_e)), symbolStore);
}

QUIZ_CASE(pcj_parse_implicit_multiplication) {
  assert_text_not_parsable(".1.2");
  assert_text_not_parsable("1 2");
  assert_parsed_expression_is("1x", KMult(1_e, "x"_e));
  assert_parsed_expression_is("1Ans", KMult(1_e, "Ans"_e));
  // Special identifiers can be implicitly multiplied from the left
  assert_parsed_expression_is("Ans5", KMult("Ans"_e, 5_e));
  // Fallback from binary number
  assert_parsed_expression_is("0b2", KMult(0_e, "b2"_e));
  assert_parsed_expression_is("0xG", KMult(0_e, "x"_e, "G"_e));
  assert_parsed_expression_is("1x+2", KAdd(KMult(1_e, "x"_e), 2_e));
  assert_parsed_expression_is("1π", KMult(1_e, π_e));
  assert_parsed_expression_is("1x-2", KSub(KMult(1_e, "x"_e), 2_e));
  assert_parsed_expression_is("-1x", KOpposite(KMult(1_e, "x"_e)));
  assert_parsed_expression_is("2×1x", KMult(2_e, 1_e, "x"_e));
  assert_parsed_expression_is("2^1x", KMult(KPow(2_e, 1_e), "x"_e));
  assert_parsed_expression_is("1x^2", KMult(1_e, KPow("x"_e, 2_e)));
  assert_parsed_expression_is("2/1x", KDiv(2_e, KMult(1_e, "x"_e)));
  assert_parsed_expression_is("1x/2", KDiv(KMult(1_e, "x"_e), 2_e));
  assert_parsed_expression_is("(1)2", KMult(KParentheses(1_e), 2_e));
  assert_parsed_expression_is("1(2)", KMult(1_e, KParentheses(2_e)));
  assert_parsed_expression_is("sin(1)2", KMult(KSin(1_e), 2_e));
  assert_parsed_expression_is("1cos(2)", KMult(1_e, KCos(2_e)));
  assert_parsed_expression_is("1!2", KMult(KFact(1_e), 2_e));
  assert_parsed_expression_is("2e^(3)",
                              KMult(2_e, KPow(e_e, KParentheses(3_e))));
  assert_parsed_expression_is("(2^3)3",
                              KMult(KParentheses(KPow(2_e, 3_e)), 3_e));
  assert_parsed_expression_is("[[1]][[2]]",
                              KMult(KMatrix<1, 1>(1_e), KMatrix<1, 1>(2_e)));
  assert_parsed_expression_is("2{1,2}", KMult(2_e, KList(1_e, 2_e)));
  assert_parsed_expression_is("{1,2}2", KMult(KList(1_e, 2_e), 2_e));
}

QUIZ_CASE(pcj_parse_with_missing_parentheses) {
  assert_parsed_expression_is("1+-2", KAdd(1_e, KOpposite(2_e)));
  assert_parsed_expression_is("1--2", KSub(1_e, KOpposite(2_e)));
  assert_parsed_expression_is("1+conj(-2)", KAdd(1_e, KConj(KOpposite(2_e))));
  assert_parsed_expression_is("1-conj(-2)", KSub(1_e, KConj(KOpposite(2_e))));
  assert_parsed_expression_is("3conj(1+i)", KMult(3_e, KConj(KAdd(1_e, i_e))));
  assert_parsed_expression_is("2×-3", KMult(2_e, KOpposite(3_e)));
  assert_parsed_expression_is("--2", KOpposite(KOpposite(2_e)));
  assert_parsed_expression_is("(2/3)^2",
                              KPow(KParentheses(KDiv(2_e, 3_e)), 2_e));
  assert_parsed_expression_is("log(1+-2)", KLog(KAdd(1_e, KOpposite(2_e))));

  // Conjugate expressions
  assert_parsed_expression_is("conj(-3)+2", KAdd(KConj(KOpposite(3_e)), 2_e));
  assert_parsed_expression_is("2+conj(-3)", KAdd(2_e, KConj(KOpposite(3_e))));
  assert_parsed_expression_is("conj(-3)×2", KMult(KConj(KOpposite(3_e)), 2_e));
  assert_parsed_expression_is("2×conj(-3)", KMult(2_e, KConj(KOpposite(3_e))));
  assert_parsed_expression_is("conj(-3)-2", KSub(KConj(KOpposite(3_e)), 2_e));
  assert_parsed_expression_is("2-conj(-3)", KSub(2_e, KConj(KOpposite(3_e))));
  assert_parsed_expression_is("conj(2+3)^2", KPow(KConj(KAdd(2_e, 3_e)), 2_e));
  assert_parsed_expression_is("-conj(2+3)", KOpposite(KConj(KAdd(2_e, 3_e))));
  assert_parsed_expression_is("conj(2+3)!", KFact(KConj(KAdd(2_e, 3_e))));
}

QUIZ_CASE(pcj_parse_east_arrows) {
  assert_text_not_parsable("1↗2");
  assert_text_not_parsable("1↘2");
  assert_text_not_parsable("1↗2%%");
  assert_text_not_parsable("1↘2%%");
  assert_text_not_parsable("1↗2%*10");
  assert_text_not_parsable("1↘2%*10");
  assert_text_not_parsable("1↗10*2%");
  assert_text_not_parsable("1↘10*2%");
  assert_parsed_expression_is("1↗5%", KPercentAddition(1_e, 5_e));
  assert_parsed_expression_is("1↘5%", KPercentAddition(1_e, KOpposite(5_e)));
  assert_parsed_expression_is("2+1↗5%+4",
                              KAdd(KPercentAddition(KAdd(2_e, 1_e), 5_e), 4_e));
  assert_parsed_expression_is(
      "2+1↘5%+4", KAdd(KPercentAddition(KAdd(2_e, 1_e), KOpposite(5_e)), 4_e));
}

QUIZ_CASE(pcj_parse_logic) {
  assert_parsed_expression_is("True and False", KLogicalAnd(KTrue, KFalse));
  assert_parsed_expression_is("True or False", KLogicalOr(KTrue, KFalse));
  assert_parsed_expression_is("True xor False", KLogicalXor(KTrue, KFalse));
  assert_parsed_expression_is("True nor False", KLogicalNor(KTrue, KFalse));
  assert_parsed_expression_is("True nand False", KLogicalNand(KTrue, KFalse));
  assert_parsed_expression_is("not True", KLogicalNot(KTrue));
  assert_text_not_parsable("not");
  assert_text_not_parsable("and");
  assert_text_not_parsable("or True");
  assert_text_not_parsable("nor");
  assert_text_not_parsable("xor");
  assert_text_not_parsable("True nand");
  assert_text_not_parsable("True and or False");
  assert_text_not_parsable("True not True");
  // Operator prioritiy
  assert_parsed_expression_is("not True and False",
                              KLogicalAnd(KLogicalNot(KTrue), KFalse));
  assert_parsed_expression_is("True and False or True",
                              KLogicalOr(KLogicalAnd(KTrue, KFalse), KTrue));
  assert_parsed_expression_is("True or False and True",
                              KLogicalOr(KTrue, KLogicalAnd(KFalse, KTrue)));
  assert_parsed_expression_is("True nor False and True",
                              KLogicalNor(KTrue, KLogicalAnd(KFalse, KTrue)));
  assert_parsed_expression_is("True xor False and True",
                              KLogicalXor(KTrue, KLogicalAnd(KFalse, KTrue)));
  assert_parsed_expression_is("True or False nand True",
                              KLogicalOr(KTrue, KLogicalNand(KFalse, KTrue)));
}

QUIZ_CASE(pcj_parse_points) {
  assert_parsed_expression_is("(0,1)", KPoint(0_e, 1_e));
  assert_parsed_expression_is("(5,sin(3))", KPoint(5_e, KSin(3_e)));
  assert_parsed_expression_is("(0,1)cos(3)",
                              KMult(KPoint(0_e, 1_e), KCos(3_e)));
}
