#include <poincare/init.h>
#include <poincare_expressions.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/src/parsing/parser.h>
#include <apps/shared/global_context.h>
#include "tree/helpers.h"
#include "helper.h"

using namespace Poincare;

void assert_tokenizes_as(const Token::Type * tokenTypes, const char * string) {
  ParsingContext parsingContext(nullptr, ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer(string, &parsingContext);
  while (true) {
    Token token = tokenizer.popToken();
    quiz_assert_print_if_failure(token.type() == *tokenTypes, string);
    if (token.type() == Token::Type::EndOfStream) {
      return;
    }
    tokenTypes++;
  }
}

void assert_tokenizes_as_number(const char * string) {
  const Token::Type types[] = {Token::Type::Number, Token::Type::EndOfStream};
  assert_tokenizes_as(types, string);
}

void assert_tokenizes_as_unit(const char * string) {
  const Token::Type types[] = {Token::Type::Unit, Token::Type::EndOfStream};
  assert_tokenizes_as(types, string);
}

void assert_tokenizes_as_constant(const char * string) {
  const Token::Type types[] = {Token::Type::Constant, Token::Type::EndOfStream};
  assert_tokenizes_as(types, string);
}

void assert_tokenizes_as_undefined_token(const char * string) {
  ParsingContext parsingContext(nullptr, ParsingContext::ParsingMethod::Classic);
  Tokenizer tokenizer(string, &parsingContext);
  while (true) {
    Token token = tokenizer.popToken();
    if (token.type() == Token::Type::Undefined) {
      return;
    }
    quiz_assert_print_if_failure(token.type() != Token::Type::EndOfStream, string);
  }
}

void assert_text_not_parsable(const char * text, Context * context = nullptr) {
  Expression result = Expression::Parse(text, context);
  quiz_assert_print_if_failure(result.isUninitialized(), text);
}

void assert_parsed_expression_is(const char * expression, Poincare::Expression r, bool addParentheses = false, bool parseForAssignment = false, Preferences::MixedFractions mixedFractionsParameter = Preferences::MixedFractions::Enabled) {
  Shared::GlobalContext context;
  Preferences::sharedPreferences->enableMixedFractions(mixedFractionsParameter);
  Expression e = parse_expression(expression, &context, addParentheses, parseForAssignment);
  quiz_assert_print_if_failure(e.isIdenticalTo(r), expression);
}

void assert_parsed_expression_with_user_parentheses_is(const char * expression, Poincare::Expression r) { return assert_parsed_expression_is(expression, r, true); }

QUIZ_CASE(poincare_parsing_tokenize_numbers) {
  assert_tokenizes_as_number("1");
  assert_tokenizes_as_number("12");
  assert_tokenizes_as_number("123");
  assert_tokenizes_as_number("1.3");
  assert_tokenizes_as_number(".3");
  assert_tokenizes_as_number("1.3ᴇ3");
  assert_tokenizes_as_number("12.34ᴇ56");
  assert_tokenizes_as_number(".3ᴇ-32");
  assert_tokenizes_as_number("12.");
  assert_tokenizes_as_number(".999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999");
  assert_tokenizes_as_number("0.");
  assert_tokenizes_as_number("1.ᴇ-4");
  assert_tokenizes_as_number("1.ᴇ9999");

  const Token::Type binaryType[] = {Token::Type::BinaryNumber, Token::Type::EndOfStream};
  assert_tokenizes_as(binaryType, "0b0011010101");
  const Token::Type hexadecimalType[] = {Token::Type::HexadecimalNumber, Token::Type::EndOfStream};
  assert_tokenizes_as(hexadecimalType, "0x1234567890ABCDEF");

  assert_tokenizes_as_undefined_token("1ᴇ");
  assert_tokenizes_as_undefined_token("1..");
  assert_tokenizes_as_undefined_token("..");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ");
  assert_tokenizes_as_undefined_token("1.ᴇ");
  assert_tokenizes_as_undefined_token("1ᴇ--4");
  assert_tokenizes_as_undefined_token("1.ᴇᴇ4");
  assert_tokenizes_as_undefined_token("1ᴇ2ᴇ4");
}

QUIZ_CASE(poincare_parsing_memory_exhaustion) {
#if __EMSCRIPTEN__
  /* We skip the following test on the web simulator, as it depends on a rollback
   * using a longjump in ExceptionCheckpoint::Raise. This longjump was removed
   * from the web implementation, as it cannot be made to work reliably in the
   * version of emscripten we depend on.
   * Fortuitously, this test used to succeed on the web simulator, but we cannot
   * rely on this behaviour. */
#else
  int initialPoolSize = pool_size();
  assert_parsed_expression_is("2+3",Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3)));
  assert_pool_size(initialPoolSize);

  int memoryFailureHasBeenHandled = false;
  {
    Poincare::ExceptionCheckpoint ecp;
    if (ExceptionRun(ecp)) {
      Addition a = Addition::Builder();
      while (true) {
        Expression e = Expression::Parse("1+2+3+4+5+6+7+8+9+10", nullptr);
        a.addChildAtIndexInPlace(e, 0, a.numberOfChildren());
      }
    } else {
      memoryFailureHasBeenHandled = true;
    }
  }

  quiz_assert(memoryFailureHasBeenHandled);
  assert_pool_size(initialPoolSize);
  Expression e = Expression::Parse("1+1", nullptr);
  /* Stupid check to make sure the global variable generated by Bison is not
   * ruining everything */
#endif
}

QUIZ_CASE(poincare_parsing_parse_numbers) {
  // Parse decimal
  assert_parsed_expression_is("0", BasedInteger::Builder(0));
  assert_parsed_expression_is("0.1", Decimal::Builder(0.1));
  assert_parsed_expression_is("1.", BasedInteger::Builder(1));
  assert_parsed_expression_is(".1", Decimal::Builder(0.1));
  assert_parsed_expression_is("0ᴇ2", Decimal::Builder(0.0));
  assert_parsed_expression_is("0.1ᴇ2", Decimal::Builder(10.0));
  assert_parsed_expression_is("1.ᴇ2", Decimal::Builder(100.0));
  assert_parsed_expression_is(".1ᴇ2", Decimal::Builder(10.0));
  assert_parsed_expression_is("0ᴇ-2", Decimal::Builder(0.0));
  assert_parsed_expression_is("0.1ᴇ-2", Decimal::Builder(0.001));
  assert_parsed_expression_is("1.ᴇ-2", Decimal::Builder(0.01));
  assert_parsed_expression_is(".1ᴇ-2", Decimal::Builder(0.001));
  // Decimal with rounding when digits are above 14
  assert_parsed_expression_is("0.0000012345678901234", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("0.00000123456789012345", Decimal::Builder(Integer("12345678901235"), -6));
  assert_parsed_expression_is("0.00000123456789012341", Decimal::Builder(Integer("12345678901234"), -6));
  assert_parsed_expression_is("1234567890123.4", Decimal::Builder(Integer("12345678901234"), 12));
  assert_parsed_expression_is("123456789012345.2", Decimal::Builder(Integer("12345678901235"), 14));
  assert_parsed_expression_is("123456789012341.2", Decimal::Builder(Integer("12345678901234"), 14));
  assert_parsed_expression_is("12.34567", Decimal::Builder(Integer("1234567"), 1));
  assert_parsed_expression_is(".999999999999990", Decimal::Builder(Integer("99999999999999"), -1));
  assert_parsed_expression_is("9.99999999999994", Decimal::Builder(Integer("99999999999999"), 0));
  assert_parsed_expression_is("99.9999999999995", Decimal::Builder(Integer("100000000000000"), 2));
  assert_parsed_expression_is("999.999999999999", Decimal::Builder(Integer("100000000000000"), 3));
  assert_parsed_expression_is("9999.99199999999", Decimal::Builder(Integer("99999920000000"), 3));
  assert_parsed_expression_is("99299.9999999999", Decimal::Builder(Integer("99300000000000"), 4));

  // Parse integer
  assert_parsed_expression_is("123456789012345678765434567", BasedInteger::Builder("123456789012345678765434567"));
  assert_parsed_expression_is(MaxParsedIntegerString(), BasedInteger::Builder(MaxParsedIntegerString()));

  // Parsed Based integer
  assert_parsed_expression_is("0b1011", BasedInteger::Builder("1011", 4, OMG::Base::Binary));
  assert_parsed_expression_is("0x12AC", BasedInteger::Builder("12AC", 4, OMG::Base::Hexadecimal));

  // Integer parsed in Decimal because they overflow Integer
  assert_parsed_expression_is(ApproximatedParsedIntegerString(), Float<double>::Builder(1.0e30));

  // Infinity
  assert_parsed_expression_is("23ᴇ1000", Infinity::Builder(false));
  assert_parsed_expression_is("2.3ᴇ1000", Decimal::Builder(Integer(23), 1000));

  // Zero
  assert_parsed_expression_is("0.23ᴇ-1000", Decimal::Builder(Integer(0), 0));
  assert_parsed_expression_is("0.23ᴇ-999", Decimal::Builder(Integer(23), -1000));
}

QUIZ_CASE(poincare_parsing_parse) {
  assert_parsed_expression_is("1", BasedInteger::Builder(1));
  assert_parsed_expression_is("(1)", Parenthesis::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("((1))", Parenthesis::Builder((Expression)Parenthesis::Builder(BasedInteger::Builder(1))));
  assert_parsed_expression_is("1+2", Addition::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("(1)+2", Addition::Builder(Parenthesis::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("(1+2)", Parenthesis::Builder(Addition::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2))));
  Expression::Tuple one_two_three = {BasedInteger::Builder(1),BasedInteger::Builder(2),BasedInteger::Builder(3)};
  assert_parsed_expression_is("1+2+3", Addition::Builder(one_two_three));
  assert_parsed_expression_is("1+2+(3+4)", Addition::Builder({BasedInteger::Builder(1), BasedInteger::Builder(2), Parenthesis::Builder(Addition::Builder(BasedInteger::Builder(3),BasedInteger::Builder(4)))}));
  assert_parsed_expression_is("+2", BasedInteger::Builder(2));
  assert_parsed_expression_is("1×2", Multiplication::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1×2×3", Multiplication::Builder(one_two_three));
  assert_parsed_expression_is("1+2×3", Addition::Builder(BasedInteger::Builder(1), Multiplication::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))));
  assert_parsed_expression_is("1/2", Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("(1/2)", Parenthesis::Builder(Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2))));
  assert_parsed_expression_is("1/2/3", Division::Builder(Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3)));
  assert_parsed_expression_is("1/2×3", Multiplication::Builder(Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3)));
  assert_parsed_expression_is("(1/2×3)", Parenthesis::Builder(Multiplication::Builder(Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3))));
  assert_parsed_expression_is("1×2/3", Multiplication::Builder(BasedInteger::Builder(1),Division::Builder(BasedInteger::Builder(2),BasedInteger::Builder(3))));
  assert_parsed_expression_is("(1×2/3)", Parenthesis::Builder(Multiplication::Builder(BasedInteger::Builder(1),Division::Builder(BasedInteger::Builder(2),BasedInteger::Builder(3)))));
  assert_parsed_expression_is("(1/2/3)", Parenthesis::Builder(Division::Builder(Division::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3))));
  assert_parsed_expression_is("1^2", Power::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1^2^3", Power::Builder(BasedInteger::Builder(1),Power::Builder(BasedInteger::Builder(2),BasedInteger::Builder(3))));
  assert_parsed_expression_is("1=2", Comparison::Builder(BasedInteger::Builder(1), ComparisonNode::OperatorType::Equal, BasedInteger::Builder(2)));
  assert_text_not_parsable("=5");
  assert_parsed_expression_is("-1", Opposite::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("(-1)", Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(1))));
  assert_parsed_expression_is("1-2", Subtraction::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("-1-2", Subtraction::Builder(Opposite::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1-2-3", Subtraction::Builder(Subtraction::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3)));
  assert_parsed_expression_is("(1-2)", Parenthesis::Builder(Subtraction::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2))));
  assert_parsed_expression_is("1+-2", Addition::Builder(BasedInteger::Builder(1),Opposite::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("--1", Opposite::Builder((Expression)Opposite::Builder(BasedInteger::Builder(1))));
  assert_parsed_expression_is("(1+2)-3", Subtraction::Builder(Parenthesis::Builder(Addition::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2))),BasedInteger::Builder(3)));
  assert_parsed_expression_is("(2×-3)", Parenthesis::Builder(Multiplication::Builder(BasedInteger::Builder(2),Opposite::Builder(BasedInteger::Builder(3)))));
  assert_parsed_expression_is("1^(2)-3", Subtraction::Builder(Power::Builder(BasedInteger::Builder(1),Parenthesis::Builder(BasedInteger::Builder(2))),BasedInteger::Builder(3)));
  assert_parsed_expression_is("1^2-3", Subtraction::Builder(Power::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),BasedInteger::Builder(3)));
  assert_parsed_expression_is("2^-3", Power::Builder(BasedInteger::Builder(2),Opposite::Builder(BasedInteger::Builder(3))));
  assert_parsed_expression_is("2--2+-1", Addition::Builder(Subtraction::Builder(BasedInteger::Builder(2),Opposite::Builder(BasedInteger::Builder(2))),Opposite::Builder(BasedInteger::Builder(1))));
  assert_parsed_expression_is("2--2×-1", Subtraction::Builder(BasedInteger::Builder(2),Opposite::Builder(Multiplication::Builder(BasedInteger::Builder(2),Opposite::Builder(BasedInteger::Builder(1))))));
  assert_parsed_expression_is("-1^2", Opposite::Builder(Power::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2))));
  assert_parsed_expression_is("2e^(3)", Multiplication::Builder(BasedInteger::Builder(2),Power::Builder(Constant::ExponentialEBuilder(),Parenthesis::Builder(BasedInteger::Builder(3)))));
  assert_parsed_expression_is("2/-3/-4", Division::Builder(Division::Builder(BasedInteger::Builder(2),Opposite::Builder(BasedInteger::Builder(3))),Opposite::Builder(BasedInteger::Builder(4))));
  assert_parsed_expression_is("1×2-3×4", Subtraction::Builder(Multiplication::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)),Multiplication::Builder(BasedInteger::Builder(3),BasedInteger::Builder(4))));
  assert_parsed_expression_is("-1×2", Opposite::Builder(Multiplication::Builder(BasedInteger::Builder(1), BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!", Factorial::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("1+2!", Addition::Builder(BasedInteger::Builder(1),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!+2", Addition::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1!+2!", Addition::Builder(Factorial::Builder(BasedInteger::Builder(1)),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1×2!", Multiplication::Builder(BasedInteger::Builder(1),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!×2", Multiplication::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1!×2!", Multiplication::Builder(Factorial::Builder(BasedInteger::Builder(1)),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1-2!", Subtraction::Builder(BasedInteger::Builder(1),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!-2", Subtraction::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1!-2!", Subtraction::Builder(Factorial::Builder(BasedInteger::Builder(1)),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1/2!", Division::Builder(BasedInteger::Builder(1),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!/2", Division::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1!/2!", Division::Builder(Factorial::Builder(BasedInteger::Builder(1)),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1^2!", Power::Builder(BasedInteger::Builder(1),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!^2", Power::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1!^2!", Power::Builder(Factorial::Builder(BasedInteger::Builder(1)),Factorial::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("(1)!", Factorial::Builder(Parenthesis::Builder(BasedInteger::Builder(1))));
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
  assert_text_not_parsable("re^\u0012im(,0)\u0013");
}

Matrix BuildMatrix(int rows, int columns, Expression entries[]) {
  Matrix m = Matrix::Builder();
  int position = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < columns; j++) {
      m.addChildAtIndexInPlace(entries[position], position, position);
      position++;
    }
  }
  m.setDimensions(rows, columns);
  return m;
}

QUIZ_CASE(poincare_parsing_matrices) {
  Expression m1[] = {BasedInteger::Builder(1)};
  assert_parsed_expression_is("[[1]]", BuildMatrix(1,1,m1));
  Expression m2[] = {BasedInteger::Builder(1),BasedInteger::Builder(2),BasedInteger::Builder(3)};
  assert_parsed_expression_is("[[1,2,3]]", BuildMatrix(1,3,m2));
  Expression m3[] = {BasedInteger::Builder(1),BasedInteger::Builder(2),BasedInteger::Builder(3),BasedInteger::Builder(4),BasedInteger::Builder(5),BasedInteger::Builder(6)};
  assert_parsed_expression_is("[[1,2,3][4,5,6]]", BuildMatrix(2,3,m3));
  Expression m4[] = {BasedInteger::Builder(1), BuildMatrix(1,1,m1)};
  assert_parsed_expression_is("[[1,[[1]]]]", BuildMatrix(1,2,m4));
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

template<size_t N>
List BuildList(Expression (&elements)[N]) {
  List l = List::Builder();
  for (size_t i = 0; i < N; i++) {
    l.addChildAtIndexInPlace(elements[i], i, i);
  }
  return l;
}

QUIZ_CASE(poincare_parse_lists) {
  assert_parsed_expression_is("{}", List::Builder());
  {
    Expression elements[] = { BasedInteger::Builder(1) };
    assert_parsed_expression_is("{1}", BuildList(elements));
  }
  {
    Expression elements[] = { BasedInteger::Builder(1), BasedInteger::Builder(2), BasedInteger::Builder(3) };
    assert_parsed_expression_is("{1,2,3}", BuildList(elements));
  }
  {
    Expression inner[] = { BasedInteger::Builder(2), BasedInteger::Builder(3) };
    Expression outer[] = { BasedInteger::Builder(1), BuildList(inner), BasedInteger::Builder(4) };
    assert_parsed_expression_is("{1,{2,3},4}", BuildList(outer));
  }
  assert_text_not_parsable("{");
  assert_text_not_parsable("{{");
  assert_text_not_parsable("}");
  assert_text_not_parsable("}}");
  assert_text_not_parsable("}{");
  assert_text_not_parsable("{,}");
  assert_text_not_parsable("{1,2,}");
  assert_text_not_parsable("{1,,3}");
  assert_text_not_parsable("{,2,3}");
}

QUIZ_CASE(poincare_parse_lists_access) {
  {
    Expression elements[] = { BasedInteger::Builder(2) };
    assert_parsed_expression_is("{2}(1)", ListElement::Builder(BasedInteger::Builder(1), BuildList(elements)));
  }
  {
    Expression elements[] = { BasedInteger::Builder(3), BasedInteger::Builder(4), BasedInteger::Builder(5) };
    assert_parsed_expression_is("{3,4,5}(1,2)", ListSlice::Builder(BasedInteger::Builder(1), BasedInteger::Builder(2), BuildList(elements)));
  }
}

QUIZ_CASE(poincare_parsing_constants) {
  for (ConstantNode::ConstantInfo info : ConstantNode::k_constants) {
    for (const char * constantNameAlias : info.m_aliasesList) {
      assert_tokenizes_as_constant(constantNameAlias);
    }
  }
}

QUIZ_CASE(poincare_parsing_units) {
  // Units
  Shared::GlobalContext context;
  for (int i = 0; i < Unit::Representative::k_numberOfDimensions; i++) {
    const Unit::Representative * dim = Unit::Representative::DefaultRepresentatives()[i];
    for (int j = 0; j < dim->numberOfRepresentatives(); j++) {
      const Unit::Representative * rep = dim->representativesOfSameDimension() + j;
      constexpr static size_t bufferSize = 10;
      char buffer[bufferSize];
      Unit::Builder(rep, Unit::Prefix::EmptyPrefix()).serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, Preferences::VeryShortNumberOfSignificantDigits);
      Expression unit = parse_expression(buffer, nullptr, false);
      quiz_assert_print_if_failure(unit.type() == ExpressionNode::Type::Unit, "Should be parsed as a Unit");
      // Try without '_'. This need a context or everything without '_' is understood as variable.
      unit = parse_expression(buffer + 1, &context, false);
      quiz_assert_print_if_failure(unit.type() == ExpressionNode::Type::Unit, "Should be parsed as a Unit");
      if (rep->isInputPrefixable()) {
        for (size_t i = 0; i < Unit::Prefix::k_numberOfPrefixes; i++) {
          const Unit::Prefix * pre = Unit::Prefix::Prefixes();
          Unit::Builder(rep, pre).serialize(buffer, bufferSize, Preferences::PrintFloatMode::Decimal, Preferences::VeryShortNumberOfSignificantDigits);
          Expression unit = parse_expression(buffer, nullptr, false);
          quiz_assert_print_if_failure(unit.type() == ExpressionNode::Type::Unit, "Should be parsed as a Unit");
          // Try without '_'. This need a context or everything without '_' is understood as variable.
          unit = parse_expression(buffer, &context, false);
          quiz_assert_print_if_failure(unit.type() == ExpressionNode::Type::Unit, "Should be parsed as a Unit");
        }
      }
    }
  }

  // Non-existing units are not parsable
  assert_text_not_parsable("_n");
  assert_text_not_parsable("_a");

  assert_text_not_parsable("°");

  // Any identifier starting with '_' is tokenized as a unit
  assert_tokenizes_as_unit("_m");
  assert_tokenizes_as_unit("_A");

  // Can parse implicit multiplication with units
  Expression kilometer = Expression::Parse("_km", nullptr);
  Expression second = Expression::Parse("_s", nullptr);
  assert_parsed_expression_is("_kmπ", Multiplication::Builder(kilometer, Constant::PiBuilder()));
  assert_parsed_expression_is("π_km", Multiplication::Builder(Constant::PiBuilder(), kilometer));
  assert_parsed_expression_is("_s_km", Multiplication::Builder(second, kilometer));
  assert_parsed_expression_is("3_s", Multiplication::Builder(BasedInteger::Builder(3), second));

  // "l" is alias for "L"
  Expression liter = Expression::Parse("_L", nullptr);
  assert_parsed_expression_is("l", liter);
  assert_parsed_expression_is("_l", liter);

  assert_parsed_expression_is("kmπ", Multiplication::Builder(kilometer, Constant::PiBuilder()));
  assert_parsed_expression_is("πkm", Multiplication::Builder(Constant::PiBuilder(), kilometer));
  assert_parsed_expression_is("skm", Multiplication::Builder(second, kilometer));
  assert_parsed_expression_is("3s", Multiplication::Builder(BasedInteger::Builder(3), second));

  // Angle units
  Expression degree = Expression::Parse("_°", nullptr);
  Expression arcsecond = Expression::Parse("_\"", nullptr);
  assert_parsed_expression_is("3°", Multiplication::Builder(BasedInteger::Builder(3), degree));
  assert_parsed_expression_is("3\"", Multiplication::Builder(BasedInteger::Builder(3), arcsecond));
  assert_parsed_expression_is("3\"+a\"", Addition::Builder(Multiplication::Builder(BasedInteger::Builder(3), arcsecond), Multiplication::Builder(Symbol::Builder("a", 1), arcsecond.clone())));
  assert_parsed_expression_is("3\"abc\"", Multiplication::Builder(BasedInteger::Builder(3), Symbol::Builder("\"abc\"", 5)));
  assert_parsed_expression_is("x\"abc\"", Multiplication::Builder(Symbol::Builder("x", 1), Symbol::Builder("\"abc\"", 5)));
  assert_text_not_parsable("\"0");

  // Implicit addition
  Expression minute = Expression::Parse("_min", nullptr);
  Expression hour = Expression::Parse("_h", nullptr);
  assert_parsed_expression_is("3h40min5s", Addition::Builder({Multiplication::Builder(BasedInteger::Builder(3), hour), Multiplication::Builder(BasedInteger::Builder(40), minute), Multiplication::Builder(BasedInteger::Builder(5), second)}));
  assert_parsed_expression_is("5mi4yd2ft3in", Addition::Builder({Multiplication::Builder(BasedInteger::Builder(5), Expression::Parse("_mi", nullptr)),Multiplication::Builder(BasedInteger::Builder(4), Expression::Parse("_yd", nullptr)), Multiplication::Builder(BasedInteger::Builder(2), Expression::Parse("_ft", nullptr)), Multiplication::Builder(BasedInteger::Builder(3), Expression::Parse("_in", nullptr))}));
  assert_parsed_expression_is("5lb4oz", Addition::Builder({Multiplication::Builder(BasedInteger::Builder(5), Expression::Parse("_lb", nullptr)), Multiplication::Builder(BasedInteger::Builder(4), Expression::Parse("_oz", nullptr))}));
  assert_parsed_expression_is("5°4'3\"", Addition::Builder({Multiplication::Builder(BasedInteger::Builder(5), Expression::Parse("_°", nullptr)), Multiplication::Builder(BasedInteger::Builder(4), Expression::Parse("_'", nullptr)), Multiplication::Builder(BasedInteger::Builder(3), Expression::Parse("_\"", nullptr))}));
  // Works with decimal numbers
  assert_parsed_expression_is("3.5h40.3min5.1s", Addition::Builder({Multiplication::Builder(Decimal::Builder(3.5), hour), Multiplication::Builder(Decimal::Builder(40.3), minute), Multiplication::Builder(Decimal::Builder(5.1), second)}));
  // Has priority over other operation
  assert_parsed_expression_is("2×3h5s", Multiplication::Builder(BasedInteger::Builder(2), Addition::Builder({Multiplication::Builder(BasedInteger::Builder(3), hour), Multiplication::Builder(BasedInteger::Builder(5), second)})));
  assert_parsed_expression_is("4h-3h5s", Subtraction::Builder(Multiplication::Builder(BasedInteger::Builder(4), hour.clone()), Addition::Builder({Multiplication::Builder(BasedInteger::Builder(3), hour), Multiplication::Builder(BasedInteger::Builder(5), second)})));
  // Does not work with fractions or other numbers
  assert_parsed_expression_is("(3/5)hπs", Multiplication::Builder({Parenthesis::Builder(Division::Builder(BasedInteger::Builder(3), BasedInteger::Builder(5))), hour, Constant::PiBuilder(), second}));
  // Does not work with any unit
  assert_parsed_expression_is("3km4m", Multiplication::Builder({BasedInteger::Builder(3), Symbol::Builder("k",1), Symbol::Builder("m4", 2), Expression::Parse("_m", nullptr)})); // 3*k*m4*m
  // Does not work in any order
  assert_parsed_expression_is("3s5h", Multiplication::Builder({BasedInteger::Builder(3), Symbol::Builder("s5", 2), hour})); // 3*s5*h
  // Does not allow repetition
  assert_parsed_expression_is("3s5s", Multiplication::Builder({BasedInteger::Builder(3), Symbol::Builder("s5", 2), second})); // 3*s5*s
}

QUIZ_CASE(poincare_parsing_identifiers) {
  // Custom variables without storage
  assert_parsed_expression_is("a", Symbol::Builder("a", 1));
  assert_parsed_expression_is("x", Symbol::Builder("x", 1));
  assert_parsed_expression_is("\"toot\"", Symbol::Builder("\"toot\"", 6));
  assert_parsed_expression_is("\"tot12\"", Symbol::Builder("\"tot12\"", 7));
  assert_parsed_expression_is("f(f)", Multiplication::Builder(Symbol::Builder("f", 1), Parenthesis::Builder(Symbol::Builder("f", 1))));
  assert_parsed_expression_is("f((1))", Multiplication::Builder(Symbol::Builder("f", 1), Parenthesis::Builder( Parenthesis::Builder(BasedInteger::Builder(1)))));
  assert_text_not_parsable("_a");
  assert_text_not_parsable("abcdefgh");
  assert_text_not_parsable("f(1,2,3)");

  // User-defined functions
  assert_parsed_expression_is("1→f(x)", Store::Builder(BasedInteger::Builder(1), Function::Builder("f", 1, Symbol::Builder("x",1))));
  assert_parsed_expression_is("1→ab12AB_(x)", Store::Builder(BasedInteger::Builder(1), Function::Builder("ab12AB_", 7, Symbol::Builder("x",1))));

  // Reserved symbols
  assert_parsed_expression_is("Ans", Symbol::Builder("Ans", 3));
  assert_parsed_expression_is("ans", Symbol::Builder("Ans", 3));
  assert_parsed_expression_is("i", Constant::ComplexIBuilder());
  assert_parsed_expression_is("π", Constant::PiBuilder());
  assert_parsed_expression_is("pi", Constant::PiBuilder());
  assert_parsed_expression_is("e", Constant::ExponentialEBuilder());
  assert_parsed_expression_is("∞", Infinity::Builder(false));
  assert_parsed_expression_is("+∞", Infinity::Builder(false));
  assert_parsed_expression_is("inf", Infinity::Builder(false));
  assert_parsed_expression_is("+inf", Infinity::Builder(false));
  assert_parsed_expression_is(Undefined::Name(), Undefined::Builder());

  // Reserved functions
  assert_parsed_expression_is("arccos(1)", ArcCosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("acos(1)", ArcCosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("cos^\u0012-1\u0013(1)", ArcCosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arcosh(1)", HyperbolicArcCosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arccot(1)", ArcCotangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arccsc(1)", ArcCosecant::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("abs(1)", AbsoluteValue::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arg(1)", ComplexArgument::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arcsec(1)", ArcSecant::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("asin(1)", ArcSine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arcsin(1)", ArcSine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sin^\u0012-1\u0013(1)", ArcSine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arsinh(1)", HyperbolicArcSine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("arctan(1)", ArcTangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("atan(1)", ArcTangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("tan^\u0012-1\u0013(1)", ArcTangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("artanh(1)", HyperbolicArcTangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("binomial(2,1)", BinomialCoefficient::Builder(BasedInteger::Builder(2),BasedInteger::Builder(1)));
  assert_parsed_expression_is("ceil(1)", Ceiling::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("cross(1,1)", VectorCross::Builder(BasedInteger::Builder(1),BasedInteger::Builder(1)));
  assert_text_not_parsable("diff(1,2,3)");
  assert_text_not_parsable("diff(0,_s,0)");
  assert_parsed_expression_is("diff(1,x,3)", Derivative::Builder(BasedInteger::Builder(1),Symbol::Builder("x",1),BasedInteger::Builder(3)));
  assert_parsed_expression_is("diff(xa,xa,3)", Derivative::Builder(Symbol::Builder("xa",2),Symbol::Builder("xa",2),BasedInteger::Builder(3)));
  assert_parsed_expression_is("diff(xaxaxa,xaxax,1)", Derivative::Builder(Multiplication::Builder(Symbol::Builder("xaxax",5),Symbol::Builder("a",1)),Symbol::Builder("xaxax",5),BasedInteger::Builder(1)));
  assert_parsed_expression_is("diff(diff(yb,yb,xa),xa,3)", Derivative::Builder(Derivative::Builder(Symbol::Builder("yb",2),Symbol::Builder("yb",2),Symbol::Builder("xa",2)),Symbol::Builder("xa",2),BasedInteger::Builder(3)));
  assert_parsed_expression_is("dim(1)", Dimension::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("conj(1)", Conjugate::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("cot(1)", Cotangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("det(1)", Determinant::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("dot(1,1)", VectorDot::Builder(BasedInteger::Builder(1),BasedInteger::Builder(1)));
  assert_parsed_expression_is("cos(1)", Cosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("cosh(1)", HyperbolicCosine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("csc(1)", Cosecant::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("factor(1)", Factor::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("floor(1)", Floor::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("frac(1)", FracPart::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("gcd(1,2,3)", GreatCommonDivisor::Builder({BasedInteger::Builder(1),BasedInteger::Builder(2),BasedInteger::Builder(3)}));
  assert_text_not_parsable("gcd(1)");
  assert_text_not_parsable("gcd()");
  assert_parsed_expression_is("im(1)", ImaginaryPart::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("int(1,x,2,3)", Integral::Builder(BasedInteger::Builder(1),Symbol::Builder("x",1),BasedInteger::Builder(2),BasedInteger::Builder(3)));
  assert_text_not_parsable("int(1,2,3,4)");
  assert_text_not_parsable("int(1,_s,3,4)");
  assert_parsed_expression_is("inverse(1)", MatrixInverse::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("lcm(1,2,3)", LeastCommonMultiple::Builder({BasedInteger::Builder(1),BasedInteger::Builder(2),BasedInteger::Builder(3)}));
  assert_text_not_parsable("lcm(1)");
  assert_text_not_parsable("lcm()");
  assert_parsed_expression_is("ln(1)", NaperianLogarithm::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("log(1)", Logarithm::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("log(1,2)", Logarithm::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("log\u0014{2\u0014}(1)", Logarithm::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("\u0014{3\u0014}log(7)", Logarithm::Builder(BasedInteger::Builder(7), BasedInteger::Builder(3)));
  assert_parsed_expression_is("5\u0014{3\u0014}log(7)", Multiplication::Builder(BasedInteger::Builder(5), Logarithm::Builder(BasedInteger::Builder(7), BasedInteger::Builder(3))));
  {
    Shared::GlobalContext context;
    // A context is passed so that the expression is not parsed as a sequence
    assert_text_not_parsable("og\u0014{2\u0014}(1)", &context);
  }
  assert_parsed_expression_is("norm(1)", VectorNorm::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("permute(2,1)", PermuteCoefficient::Builder(BasedInteger::Builder(2),BasedInteger::Builder(1)));
  assert_text_not_parsable("piecewise(2,1+1,3)");
  assert_parsed_expression_is("product(1,n,2,3)", Product::Builder(BasedInteger::Builder(1),Symbol::Builder("n",1),BasedInteger::Builder(2),BasedInteger::Builder(3)));
  assert_text_not_parsable("product(1,2,3,4)");
  assert_text_not_parsable("product(1,_s,3,4)");
  assert_parsed_expression_is("quo(1,2)", DivisionQuotient::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("random()", Random::Builder());
  assert_parsed_expression_is("randint(1,2)", Randint::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("randint(2)", Randint::Builder(BasedInteger::Builder(2)));
  assert_parsed_expression_is("randintnorep(1,10,3)", RandintNoRepeat::Builder(BasedInteger::Builder(1), BasedInteger::Builder(10), BasedInteger::Builder(3)));
  assert_parsed_expression_is("re(1)", RealPart::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("ref(1)", MatrixRowEchelonForm::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("rem(1,2)", DivisionRemainder::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("root(1,2)", NthRoot::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("round(1,2)", Round::Builder(BasedInteger::Builder(1),BasedInteger::Builder(2)));
  assert_parsed_expression_is("rref(1)", MatrixReducedRowEchelonForm::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sec(1)", Secant::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sin(1)", Sine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sign(1)", SignFunction::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sinh(1)", HyperbolicSine::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("sum(1,n,2,3)", Sum::Builder(BasedInteger::Builder(1),Symbol::Builder("n",1),BasedInteger::Builder(2),BasedInteger::Builder(3)));
  assert_text_not_parsable("sum(1,2,3,4)");
  assert_text_not_parsable("sum(1,_s,3,4)");
  assert_parsed_expression_is("tan(1)", Tangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("tanh(1)", HyperbolicTangent::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("trace(1)", MatrixTrace::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("transpose(1)", MatrixTranspose::Builder(BasedInteger::Builder(1)));
  assert_parsed_expression_is("√(1)", SquareRoot::Builder(BasedInteger::Builder(1)));
  assert_text_not_parsable("cos(1,2)");
  assert_text_not_parsable("quo()");
  assert_text_not_parsable("log(1,2,3)");
  assert_text_not_parsable("sinh^\u0012-1\u0013(2)");
  assert_parsed_expression_is("\u0014dep(x,{})", Dependency::Builder(Symbol::Builder("x", 1), List::Builder()));
  assert_text_not_parsable("\u0014cos(x)");
  assert_text_not_parsable("\u0014cod(x)");

  // Powered reserved functions (integer powers other than -1)
  assert_parsed_expression_is("cos^\u00120\u0013(1)", Power::Builder(Cosine::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(0)));
  assert_parsed_expression_is("sin^\u00121\u0013(1)", Power::Builder(Sine::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(1)));
  assert_parsed_expression_is("tan^\u00122\u0013(1)", Power::Builder(Tangent::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("arccos^\u00123\u0013(1)", Power::Builder(ArcCosine::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(3)));
  assert_parsed_expression_is("arcsin^\u00124\u0013(1)", Power::Builder(ArcSine::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(4)));
  assert_parsed_expression_is("arctan^\u00125\u0013(1)", Power::Builder(ArcTangent::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(5)));
  assert_parsed_expression_is("cot^\u00126\u0013(1)", Power::Builder(Cotangent::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(6)));
  assert_parsed_expression_is("sec^\u00127\u0013(1)", Power::Builder(Secant::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(7)));
  assert_parsed_expression_is("csc^\u00128\u0013(1)", Power::Builder(Cosecant::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(8)));
  assert_parsed_expression_is("arccot^\u00129\u0013(1)", Power::Builder(ArcCotangent::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(9)));
  assert_parsed_expression_is("arcsec^\u001210\u0013(1)", Power::Builder(ArcSecant::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(10)));
  assert_parsed_expression_is("arccsc^\u001211\u0013(1)", Power::Builder(ArcCosecant::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(11)));
  assert_parsed_expression_is("cosh^\u0012-2\u0013(1)", Power::Builder(HyperbolicCosine::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("sinh^\u0012-3\u0013(1)", Power::Builder(HyperbolicSine::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(3))));
  assert_parsed_expression_is("tanh^\u0012-4\u0013(1)", Power::Builder(HyperbolicTangent::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(4))));
  assert_parsed_expression_is("arsinh^\u0012-5\u0013(1)", Power::Builder(HyperbolicArcSine::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(5))));
  assert_parsed_expression_is("arcosh^\u0012-6\u0013(1)", Power::Builder(HyperbolicArcCosine::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(6))));
  assert_parsed_expression_is("artanh^\u0012-7\u0013(1)", Power::Builder(HyperbolicArcTangent::Builder(BasedInteger::Builder(1)),Opposite::Builder(BasedInteger::Builder(7))));
  assert_text_not_parsable("ln^\u00122\u0013(2)");

  // Custom identifiers with storage
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension("ab", "exp", "", 0);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension("bacos", "func", "", 0);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension("azfoo", "exp", "", 0);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension("foobar", "func", "", 0);
  Ion::Storage::FileSystem::sharedFileSystem->createRecordWithExtension("a3b", "exp", "", 0);
  assert_parsed_expression_is("xyz", Multiplication::Builder(Symbol::Builder("x", 1), Symbol::Builder("y", 1), Symbol::Builder("z", 1)));
  assert_parsed_expression_is("xy123z", Multiplication::Builder(Symbol::Builder("x", 1), Symbol::Builder("y123", 4), Symbol::Builder("z", 1)));
  assert_parsed_expression_is("3→xyz", Store::Builder(BasedInteger::Builder(3), Symbol::Builder("xyz", 3)));
  assert_parsed_expression_is("ab", Symbol::Builder("ab", 2));
  assert_parsed_expression_is("ab3", Multiplication::Builder(Symbol::Builder("a", 1), Symbol::Builder("b3", 2)));
  assert_parsed_expression_is("a3b", Symbol::Builder("a3b", 3));
  assert_parsed_expression_is("aacos(x)", Multiplication::Builder(Symbol::Builder("a", 1), ArcCosine::Builder(Symbol::Builder("x", 1))));
  assert_parsed_expression_is("bacos(x)", Function::Builder("bacos", 5, Symbol::Builder("x", 1)));
  assert_parsed_expression_is("azfoobar(x)", Multiplication::Builder(Symbol::Builder("a", 1), Symbol::Builder("z", 1), Function::Builder("foobar", 6, Symbol::Builder("x", 1))));
  Ion::Storage::FileSystem::sharedFileSystem->destroyAllRecords();
}

QUIZ_CASE(poincare_parsing_parse_store) {
  Expression ton = Expression::Parse("_t", nullptr);
  assert_parsed_expression_is("1→a", Store::Builder(BasedInteger::Builder(1),Symbol::Builder("a",1)));
  assert_parsed_expression_is("t→a", Store::Builder(ton, Symbol::Builder("a",1)));
  assert_parsed_expression_is("1→g", Store::Builder(BasedInteger::Builder(1),Symbol::Builder("g",1)));
  assert_parsed_expression_is("1→f(x)", Store::Builder(BasedInteger::Builder(1),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("x→f(x)", Store::Builder(Symbol::Builder("x",1),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("n→f(x)", Store::Builder(Symbol::Builder("n",1),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("ab→f(ab)", Store::Builder(Symbol::Builder("ab",2),Function::Builder("f",1,Symbol::Builder("ab",2))));
  assert_parsed_expression_is("ab→f(x)", Store::Builder(Multiplication::Builder(Symbol::Builder("a",1), Symbol::Builder("b",1)),Function::Builder("f",1,Symbol::Builder("x",1))));
  assert_parsed_expression_is("t→f(t)", Store::Builder(Symbol::Builder("t",1),Function::Builder("f",1,Symbol::Builder("t",1))));
  assert_parsed_expression_is("t→f(x)", Store::Builder(ton, Function::Builder("f",1,Symbol::Builder("x",1))));
  Expression m0[] = {Symbol::Builder('x')};
  assert_parsed_expression_is("[[x]]→f(x)", Store::Builder(BuildMatrix(1,1,m0), Function::Builder("f", 1, Symbol::Builder('x'))));
  assert_text_not_parsable("a→b→c");
  assert_text_not_parsable("0→0→c");
  assert_text_not_parsable("1→");
  assert_text_not_parsable("→2");
  assert_text_not_parsable("(1→a)");
  assert_text_not_parsable("1→\1"); // UnknownX
  assert_text_not_parsable("1→\2"); // UnknownN
  assert_text_not_parsable("1→acos");
  assert_text_not_parsable("x→tan(x)");
  assert_text_not_parsable("3→min");
  assert_text_not_parsable("1→f(f)");
}

QUIZ_CASE(poincare_parsing_parse_unit_convert) {
  Expression ton = Expression::Parse("_t", nullptr);
  Expression kilogram = Expression::Parse("_kg", nullptr);
  Expression degree = Expression::Parse("_°", nullptr);
  Expression minute = Expression::Parse("_'", nullptr);
  Expression radian = Expression::Parse("_rad", nullptr);
  assert_parsed_expression_is("3t→kg", UnitConvert::Builder(Multiplication::Builder(BasedInteger::Builder(3), ton), kilogram));
  assert_parsed_expression_is("3t→kg^2", UnitConvert::Builder(Multiplication::Builder(BasedInteger::Builder(3), ton), Power::Builder(kilogram, BasedInteger::Builder(2))));
  assert_parsed_expression_is("3°4'→rad", UnitConvert::Builder(Addition::Builder(Multiplication::Builder(BasedInteger::Builder(3), degree), Multiplication::Builder(BasedInteger::Builder(4), minute)), radian));
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
  assert_reduce_and_store("_m→a", Radian, MetricUnitFormat, Real);
  assert_reduce_and_store("_m→b", Radian, MetricUnitFormat, Real);
  assert_text_not_parsable("1_km→a×b");
  assert_reduce_and_store("2→a");
  assert_text_not_parsable("3_m→a×_km");
  assert_reduce_and_store("2→f(x)");
  assert_text_not_parsable("3_m→f(2)×_km");
  // Clean the storage for other tests
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("a.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("b.exp").destroy();
  Ion::Storage::FileSystem::sharedFileSystem->recordNamed("f.func").destroy();
}

QUIZ_CASE(poincare_parsing_implicit_multiplication) {
  assert_text_not_parsable(".1.2");
  assert_text_not_parsable("1 2");
  assert_parsed_expression_is("1x", Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1)));
  assert_parsed_expression_is("1Ans", Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("Ans", 3)));
  // Special identifiers can be implicitly multiplied from the left
  assert_parsed_expression_is("Ans5", Multiplication::Builder(Symbol::Builder("Ans", 3), BasedInteger::Builder(5)));
  // Fallback from binary number
  assert_parsed_expression_is("0b2", Multiplication::Builder(BasedInteger::Builder(0),Symbol::Builder("b2", 2)));
  assert_parsed_expression_is("0xG", Multiplication::Builder(BasedInteger::Builder(0), Symbol::Builder("x", 1),Symbol::Builder("G", 1)));
  assert_parsed_expression_is("1x+2", Addition::Builder(Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1π", Multiplication::Builder(BasedInteger::Builder(1),Constant::PiBuilder()));
  assert_parsed_expression_is("1x-2", Subtraction::Builder(Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("-1x", Opposite::Builder(Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("2×1x", Multiplication::Builder(BasedInteger::Builder(2),Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("2^1x", Multiplication::Builder(Power::Builder(BasedInteger::Builder(2),BasedInteger::Builder(1)),Symbol::Builder("x", 1)));
  assert_parsed_expression_is("1x^2", Multiplication::Builder(BasedInteger::Builder(1),Power::Builder(Symbol::Builder("x", 1),BasedInteger::Builder(2))));
  assert_parsed_expression_is("2/1x", Division::Builder(BasedInteger::Builder(2),Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1))));
  assert_parsed_expression_is("1x/2", Division::Builder(Multiplication::Builder(BasedInteger::Builder(1),Symbol::Builder("x", 1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("(1)2", Multiplication::Builder(Parenthesis::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1(2)", Multiplication::Builder(BasedInteger::Builder(1),Parenthesis::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("sin(1)2", Multiplication::Builder(Sine::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("1cos(2)", Multiplication::Builder(BasedInteger::Builder(1),Cosine::Builder(BasedInteger::Builder(2))));
  assert_parsed_expression_is("1!2", Multiplication::Builder(Factorial::Builder(BasedInteger::Builder(1)),BasedInteger::Builder(2)));
  assert_parsed_expression_is("2e^(3)", Multiplication::Builder(BasedInteger::Builder(2),Power::Builder(Constant::ExponentialEBuilder(),Parenthesis::Builder(BasedInteger::Builder(3)))));
  assert_parsed_expression_is("\u00122^3\u00133", Multiplication::Builder(Power::Builder(BasedInteger::Builder(2),BasedInteger::Builder(3)), BasedInteger::Builder(3)));
  Expression m1[] = {BasedInteger::Builder(1)}; Matrix M1 = BuildMatrix(1,1,m1);
  Expression m2[] = {BasedInteger::Builder(2)}; Matrix M2 = BuildMatrix(1,1,m2);
  assert_parsed_expression_is("[[1]][[2]]", Multiplication::Builder(M1,M2));
  Expression l1[] = {BasedInteger::Builder(1), BasedInteger::Builder(2)};
  assert_parsed_expression_is("2{1,2}", Multiplication::Builder(BasedInteger::Builder(2), BuildList(l1)));
  Expression l2[] = {BasedInteger::Builder(1), BasedInteger::Builder(2)};
  assert_parsed_expression_is("{1,2}2", Multiplication::Builder(BuildList(l2), BasedInteger::Builder(2)));
  assert_parsed_expression_is("\u0014{2\u0014}log(3)log\u0014{2\u0014}(3)", Multiplication::Builder(Logarithm::Builder(BasedInteger::Builder(3),BasedInteger::Builder(2)), Logarithm::Builder(BasedInteger::Builder(3),BasedInteger::Builder(2))));
}

QUIZ_CASE(poincare_parsing_adding_missing_parentheses) {
  assert_parsed_expression_with_user_parentheses_is("1+-2", Addition::Builder(BasedInteger::Builder(1),Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("1--2", Subtraction::Builder(BasedInteger::Builder(1),Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("1+conj(-2)", Addition::Builder(BasedInteger::Builder(1),Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(2))))));
  assert_parsed_expression_with_user_parentheses_is("1-conj(-2)", Subtraction::Builder(BasedInteger::Builder(1),Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(2))))));
  assert_parsed_expression_with_user_parentheses_is("3conj(1+i)", Multiplication::Builder(BasedInteger::Builder(3), Parenthesis::Builder(Conjugate::Builder(Addition::Builder(BasedInteger::Builder(1), Constant::ComplexIBuilder())))));
  assert_parsed_expression_with_user_parentheses_is("2×-3", Multiplication::Builder(BasedInteger::Builder(2), Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(3)))));
  assert_parsed_expression_with_user_parentheses_is("2×-3", Multiplication::Builder(BasedInteger::Builder(2), Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(3)))));
  assert_parsed_expression_with_user_parentheses_is("--2", Opposite::Builder(Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(2)))));
  assert_parsed_expression_with_user_parentheses_is("\u00122/3\u0013^2", Power::Builder(Parenthesis::Builder(Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))), BasedInteger::Builder(2)));
  assert_parsed_expression_with_user_parentheses_is("log(1+-2)", Logarithm::Builder(Addition::Builder(BasedInteger::Builder(1),Parenthesis::Builder(Opposite::Builder(BasedInteger::Builder(2))))));

  // Conjugate expressions
  assert_parsed_expression_with_user_parentheses_is("conj(-3)+2", Addition::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))), BasedInteger::Builder(2)));
  assert_parsed_expression_with_user_parentheses_is("2+conj(-3)", Addition::Builder(BasedInteger::Builder(2), Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))))));
  assert_parsed_expression_with_user_parentheses_is("conj(-3)×2", Multiplication::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))), BasedInteger::Builder(2)));
  assert_parsed_expression_with_user_parentheses_is("2×conj(-3)", Multiplication::Builder(BasedInteger::Builder(2), Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))))));
  assert_parsed_expression_with_user_parentheses_is("conj(-3)-2", Subtraction::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))), BasedInteger::Builder(2)));
  assert_parsed_expression_with_user_parentheses_is("2-conj(-3)", Subtraction::Builder(BasedInteger::Builder(2), Parenthesis::Builder(Conjugate::Builder(Opposite::Builder(BasedInteger::Builder(3))))));
  assert_parsed_expression_with_user_parentheses_is("conj(2+3)^2", Power::Builder(Parenthesis::Builder(Conjugate::Builder(Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3)))), BasedInteger::Builder(2)));
  assert_parsed_expression_with_user_parentheses_is("-conj(2+3)", Opposite::Builder(Parenthesis::Builder(Conjugate::Builder(Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))))));
  assert_parsed_expression_with_user_parentheses_is("conj(2+3)!", Factorial::Builder(Parenthesis::Builder(Conjugate::Builder(Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))))));
}

QUIZ_CASE(poincare_parse_mixed_fraction) {
 assert_parsed_expression_is("1 2/3", MixedFraction::Builder(BasedInteger::Builder(1), Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))));
 assert_parsed_expression_is("1\u0012\u00122\u0013/\u00123\u0013\u0013", MixedFraction::Builder(BasedInteger::Builder(1), Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))));
 assert_parsed_expression_is("1\u00122/3\u00132", Multiplication::Builder(MixedFraction::Builder(BasedInteger::Builder(1), Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))), BasedInteger::Builder(2)));
 assert_parsed_expression_is("1\u00122/3\u0013\u00122/3\u0013", Multiplication::Builder(MixedFraction::Builder(BasedInteger::Builder(1), Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))), Division::Builder(BasedInteger::Builder(2), BasedInteger::Builder(3))));
 assert_parsed_expression_is("1\u0012e/3\u0013", Multiplication::Builder(BasedInteger::Builder(1),Division::Builder(Constant::ExponentialEBuilder(), BasedInteger::Builder(3))));
 assert_parsed_expression_is("1\u00122.5/3\u0013", Multiplication::Builder(BasedInteger::Builder(1),Division::Builder(Decimal::Builder(2.5), BasedInteger::Builder(3))));
}

QUIZ_CASE(poincare_parse_function_assignment) {
  assert_parsed_expression_is("f(x)=xy", Comparison::Builder(Multiplication::Builder(Symbol::Builder("f", 1), Parenthesis::Builder(Symbol::Builder("x", 1))), ComparisonNode::OperatorType::Equal, Multiplication::Builder(Symbol::Builder("x", 1), Symbol::Builder("y", 1))));
  assert_parsed_expression_is("f(x)=xy", Comparison::Builder(Function::Builder("f", 1, Symbol::Builder("x", 1)), ComparisonNode::OperatorType::Equal, Multiplication::Builder(Symbol::Builder("x", 1), Symbol::Builder("y", 1))), false, true);

  // Without assignment "f(x)=4=3" is "Comparison(f*(x), equal, 4, equal, 3)"
  Comparison comparison = Comparison::Builder(Multiplication::Builder(Symbol::Builder("f", 1), Parenthesis::Builder(Symbol::Builder("x", 1))), ComparisonNode::OperatorType::Equal, BasedInteger::Builder(4));
  comparison = comparison.addComparison(ComparisonNode::OperatorType::Equal, BasedInteger::Builder(3));
  assert_parsed_expression_is("f(x)=4=3", comparison);
  // When assigning, "f(x)=4=3" is "Assignment(f(x), Comparison(4, equal, 3))"
  assert_parsed_expression_is("f(x)=4=3", Comparison::Builder(Function::Builder("f", 1, Symbol::Builder("x", 1)), ComparisonNode::OperatorType::Equal, Comparison::Builder(BasedInteger::Builder(4), ComparisonNode::OperatorType::Equal, BasedInteger::Builder(3))), false, true);
}

QUIZ_CASE(poincare_parsing_east_arrows) {
  assert_text_not_parsable("1↗2");
  assert_text_not_parsable("1↘2");
  assert_text_not_parsable("1↗2%%");
  assert_text_not_parsable("1↘2%%");
  assert_text_not_parsable("1↗2%*10");
  assert_text_not_parsable("1↘2%*10");
  assert_text_not_parsable("1↗10*2%");
  assert_text_not_parsable("1↘10*2%");
  assert_parsed_expression_is("1↗5%", PercentAddition::Builder(BasedInteger::Builder(1), BasedInteger::Builder(5)));
  assert_parsed_expression_is("1↘5%", PercentAddition::Builder(BasedInteger::Builder(1), Opposite::Builder(BasedInteger::Builder(5))));
  assert_parsed_expression_is("2+1↗5%+4", Addition::Builder(PercentAddition::Builder(Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(1)), BasedInteger::Builder(5)), BasedInteger::Builder(4)));
  assert_parsed_expression_is("2+1↘5%+4", Addition::Builder(PercentAddition::Builder(Addition::Builder(BasedInteger::Builder(2), BasedInteger::Builder(1)), Opposite::Builder(BasedInteger::Builder(5))), BasedInteger::Builder(4)));
}

QUIZ_CASE(poincare_parse_logic) {
 assert_parsed_expression_is("True and False", BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::And));
 assert_parsed_expression_is("True or False", BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::Or));
 assert_parsed_expression_is("True xor False", BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::Xor));
 assert_parsed_expression_is("True nor False", BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::Nor));
 assert_parsed_expression_is("True nand False", BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::Nand));
 assert_parsed_expression_is("not True", LogicalOperatorNot::Builder(Boolean::Builder(true)));
 assert_text_not_parsable("not");
 assert_text_not_parsable("and");
 assert_text_not_parsable("or True");
 assert_text_not_parsable("nor");
 assert_text_not_parsable("xor");
 assert_text_not_parsable("True nand");
 assert_text_not_parsable("True and or False");
 assert_text_not_parsable("True not True");
 // Operator prioritiy
 assert_parsed_expression_is("not True and False", BinaryLogicalOperator::Builder(LogicalOperatorNot::Builder(Boolean::Builder(true)), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::And));
 assert_parsed_expression_is("True and False or True", BinaryLogicalOperator::Builder(BinaryLogicalOperator::Builder(Boolean::Builder(true), Boolean::Builder(false), BinaryLogicalOperatorNode::OperatorType::And), Boolean::Builder(true), BinaryLogicalOperatorNode::OperatorType::Or));
 assert_parsed_expression_is("True or False and True", BinaryLogicalOperator::Builder(Boolean::Builder(true), BinaryLogicalOperator::Builder(Boolean::Builder(false), Boolean::Builder(true), BinaryLogicalOperatorNode::OperatorType::And), BinaryLogicalOperatorNode::OperatorType::Or));
 assert_parsed_expression_is("True nor False and True", BinaryLogicalOperator::Builder(Boolean::Builder(true), BinaryLogicalOperator::Builder(Boolean::Builder(false), Boolean::Builder(true), BinaryLogicalOperatorNode::OperatorType::And), BinaryLogicalOperatorNode::OperatorType::Nor));
 assert_parsed_expression_is("True xor False and True", BinaryLogicalOperator::Builder(Boolean::Builder(true), BinaryLogicalOperator::Builder(Boolean::Builder(false), Boolean::Builder(true), BinaryLogicalOperatorNode::OperatorType::And), BinaryLogicalOperatorNode::OperatorType::Xor));
 assert_parsed_expression_is("True or False nand True", BinaryLogicalOperator::Builder(Boolean::Builder(true), BinaryLogicalOperator::Builder(Boolean::Builder(false), Boolean::Builder(true), BinaryLogicalOperatorNode::OperatorType::Nand), BinaryLogicalOperatorNode::OperatorType::Or));
}
