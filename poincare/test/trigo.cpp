#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>

using namespace Poincare;

QUIZ_CASE(poincare_parse_trigo) {
  {
    Expression * e = Expression::parse("sin(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Sine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("cos(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Cosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("tan(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::Tangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("cosh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("sinh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("tanh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicTangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("acos(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("asin(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("atan(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::ArcTangent);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("acosh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcCosine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("asinh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcSine);
    delete e;
    delete e2;
  }
  {
    Expression * e = Expression::parse("atanh(0)");
    Expression * e2 = e->simplify();
    assert(e2->type() == Expression::Type::HyperbolicArcTangent);
    delete e;
    delete e2;
  }
}

void assert_parsed_expression_approximate_to(const char * exp, float res) {
  Expression * e = Expression::parse(exp);
  GlobalContext globalContext;
  assert(fabsf(e->approximate(globalContext, Expression::AngleUnit::Radian) - res) < 0.0001f);
  delete e;
}

QUIZ_CASE(poincare_trigo_approximate) {
  assert_parsed_expression_approximate_to("cos(3)",-0.98999249660044542f);
  assert_parsed_expression_approximate_to("sin(3)", 0.14112000805986721f);
  assert_parsed_expression_approximate_to("tan(3)",-0.1425465430742778f);
  assert_parsed_expression_approximate_to("cosh(3)",10.067661995777765f);
  assert_parsed_expression_approximate_to("cosh(3)",10.067661995777765f);
  assert_parsed_expression_approximate_to("sinh(3)", 10.017874927409903f);
  assert_parsed_expression_approximate_to("tanh(3)", 0.9950547537f);
  assert_parsed_expression_approximate_to("acos(0.3)", 1.266103672779499f);
  assert_parsed_expression_approximate_to("asin(0.3)", 0.304692654f);
  assert_parsed_expression_approximate_to("atan(0.3)", 0.2914567945f);
  assert_parsed_expression_approximate_to("acosh(3)", 1.762747174f);
  assert_parsed_expression_approximate_to("asinh(0.3)", 0.2956730476f);
  assert_parsed_expression_approximate_to("atanh(0.3)", 0.3095196042f);
}

void assert_parsed_expression_evaluates_to(const char * exp, float a, float b) {
  char buffer[100];
  strlcpy(buffer, exp, 100);
  for (uint16_t i=0; i<strlen(buffer); i++) {
    if (buffer[i] == 'E') {
      buffer[i] = Ion::Charset::Exponent;
    }
    if (buffer[i] == 'e') {
      buffer[i] = Ion::Charset::Exponential;
    }
    if (buffer[i] == 'j') {
      buffer[i] = Ion::Charset::IComplex;
    }
  }
  Expression * f = Expression::parse(buffer);
  GlobalContext globalContext;
  Expression * e = f->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert(fabsf((((Complex *)e)->a()) - a) < 0.0001f);
  assert(fabsf((((Complex *)e)->b()) - b) < 0.0001f);
  delete f;
  delete e;
}

QUIZ_CASE(poincare_trigo_evaluate) {
  assert_parsed_expression_evaluates_to("cos(2)", -0.4161468365f, 0.0f);
  assert_parsed_expression_evaluates_to("cos(j-4)",-1.0086248134f, -0.8893951958f);
  assert_parsed_expression_evaluates_to("sin(2)",0.9092974268f, 0.0f);
  assert_parsed_expression_evaluates_to("sin(j-4)", 1.16780727488f, -0.768162763456f);
  assert_parsed_expression_evaluates_to("tan(2)",-2.18503986326151899f, 0.0f);
  assert_parsed_expression_evaluates_to("tan(j-4)", -0.27355308280730f, 1.002810507583504f);
  assert_parsed_expression_evaluates_to("cosh(2)", 3.762195691f, 0.0f);
  assert_parsed_expression_evaluates_to("cosh(j-4)",14.754701170483756280f,-22.96367349919304059f);
  assert_parsed_expression_evaluates_to("sinh(2)",3.62686040784701876f, 0.0f);
  assert_parsed_expression_evaluates_to("sinh(j-4)", -14.744805188558725031023f, 22.979085577886129555168f);
  assert_parsed_expression_evaluates_to("tanh(2)",0.9640275800758168839464f, 0.0f);
  assert_parsed_expression_evaluates_to("tanh(j-4)", -1.00027905623446556836909f, 0.000610240921376259f);
}
