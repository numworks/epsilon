#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <math.h>
#include <assert.h>

using namespace Poincare;

constexpr Expression::AngleUnit Degree = Expression::AngleUnit::Degree;
constexpr Expression::AngleUnit Radian = Expression::AngleUnit::Radian;

void assert_expression_parses_to(const char * text, float result, Context * context, Expression::AngleUnit angleUnit = Degree) {
  char buffer_text[100] = {};
  for (int i=0; i<strlen(text); i++) {
    buffer_text[i] = text[i];
    if (text[i] == 'E') {
      buffer_text[i] = Ion::Charset::Exponent;
    }
    if (text[i] == 'e') {
      buffer_text[i] = Ion::Charset::Exponential;
    }
    if (text[i] == 'I') {
      buffer_text[i] = Ion::Charset::IComplex;
    }
  }
  Expression * a = Expression::parse(buffer_text);
  assert(fabsf(a->approximate(*context, angleUnit) - result) < 0.0001f);
}

QUIZ_CASE(poincare_parser) {
  GlobalContext globalContext;
  assert_expression_parses_to("1.2*e^(1)", 1.2*M_E, &globalContext);
  assert_expression_parses_to("e^2*e^(1)", powf(M_E, 2.0f)*M_E, &globalContext);
  assert_expression_parses_to("2*3^4+2", 2.0f*powf(3.0f, 4.0f)+2.0f, &globalContext);
  assert_expression_parses_to("-2*3^4+2", -2.0f*powf(3.0f, 4.0f)+2.0f, &globalContext);
  assert_expression_parses_to("-sin(3)*2-3", -sinf(3.0f)*2.0f-3.0f, &globalContext, Radian);
  assert_expression_parses_to("-.003", -0.003f, &globalContext);
  assert_expression_parses_to(".02E2", 2.0f, &globalContext);
  assert_expression_parses_to("5-2/3", 5.0f-2.0f/3.0f, &globalContext);
  assert_expression_parses_to("2/3-5", 2.0f/3.0f-5.0f, &globalContext);
  assert_expression_parses_to("-2/3-5", -2.0f/3.0f-5.0f, &globalContext);
  assert_expression_parses_to("sin(3)2(4+2)", sinf(3.0f)*2.0f*(4.0f+2.0f), &globalContext, Radian);
  assert_expression_parses_to("4/2*(2+3)", 4.0f/2.0f*(2.0f+3.0f), &globalContext, Radian);
  assert_expression_parses_to("4/2*(2+3)", 4.0f/2.0f*(2.0f+3.0f), &globalContext, Radian);
}
