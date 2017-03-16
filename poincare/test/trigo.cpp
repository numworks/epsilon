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

QUIZ_CASE(poincare_trigo_approximate) {
  Expression * e = Expression::parse("cos(3)");
  GlobalContext globalContext;
  assert(e->approximate(globalContext, Expression::AngleUnit::Radian) == -0.9899924966f);
  delete e;

  e = Expression::parse("sin(3)");
  assert(e->approximate(globalContext, Expression::AngleUnit::Radian) == 0.1411200081f);
  delete e;

  e = Expression::parse("tan(3)");
  assert(e->approximate(globalContext, Expression::AngleUnit::Radian) == -0.1425465431f);
  delete e;

  e = Expression::parse("cosh(3)");
  assert(e->approximate(globalContext) == 10.067662f);
  delete e;

  e = Expression::parse("sinh(3)");
  assert(e->approximate(globalContext) == 10.01787493f);
  delete e;

  e = Expression::parse("tanh(3)");
  assert(0.9950547537f - 0.0000001f <= e->approximate(globalContext) &&
    e->approximate(globalContext) <= 0.9950547537f + 0.0000001f);
  delete e;

  e = Expression::parse("acos(0.3)");
  assert(fabsf(1.266103673f - e->approximate(globalContext, Expression::AngleUnit::Radian)) <= 0.000001f);
  delete e;

  e = Expression::parse("asin(0.3)");
  assert(fabsf(0.304692654f - e->approximate(globalContext, Expression::AngleUnit::Radian)) <= 0.000001f);
  delete e;

  e = Expression::parse("atan(0.3)");
  assert(fabsf(0.2914567945f - e->approximate(globalContext, Expression::AngleUnit::Radian)) <= 0.000001f);
  delete e;

  e = Expression::parse("acosh(3)");
  assert(fabsf(1.762747174f - e->approximate(globalContext)) <= 0.000001f);
  delete e;

  e = Expression::parse("asinh(0.3)");
  assert(fabsf(0.2956730476f - e->approximate(globalContext)) <= 0.000001f);
  delete e;

  e = Expression::parse("atanh(0.3)");
  assert(fabsf(0.3095196042f - e->approximate(globalContext)) <= 0.000001f);
  delete e;

}

QUIZ_CASE(poincare_trigo_evaluate) {
  GlobalContext globalContext;
  Expression * a = Expression::parse("cos(2)");
  Expression * e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((-0.4161468365f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (-0.4161468365f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText1[10] ={'c','o', 's', '(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText1);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((-1.0086248134f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (-1.0086248134f + 0.000001f) &&
      -0.8893951958f - 0.000001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= -0.8893951958f + 0.000001f);
  delete a;
  delete e;

  a = Expression::parse("sin(2)");
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((0.9092974268f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (0.9092974268f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText2[10] ={'s','i', 'n', '(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText2);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((1.16780727488f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (1.16780727488f + 0.000001f) &&
      -0.768162763456f - 0.000001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= -0.768162763456f + 0.000001f);
  delete a;
  delete e;

  a = Expression::parse("tan(2)");
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((-2.185039863f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (-2.185039863f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText3[10] ={'t','a', 'n', '(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText3);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert(-0.27355308280f - 0.000001f <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= -0.27355308280f + 0.000001f &&
      1.00281050758f - 0.000001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <=  1.00281050758f + 0.000001f);
  delete a;
  delete e;

  a = Expression::parse("cosh(2)");
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((3.762195691f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (3.762195691f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText4[10] ={'c','o', 's', 'h','(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText4);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((14.754701170f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (14.754701170f + 0.000001f) &&
      -22.963673499f - 0.000001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= -22.963673499f + 0.000001f);
  delete a;
  delete e;

  a = Expression::parse("sinh(2)");
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((3.626860408f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (3.626860408f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText5[10] ={'s','i', 'n', 'h', '(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText5);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((-14.74480518855f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (-14.74480518855f + 0.000001f) &&
      22.97908557788f - 0.00001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <= 22.97908557788f + 0.00001f);
  delete a;
  delete e;

  a = Expression::parse("tanh(2)");
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert((0.9640275801f - 0.000001f) <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= (0.9640275801f + 0.000001f) && ((Complex *)e)->b() == 0.0f);
  delete a;
  delete e;

  char expText6[10] ={'t','a', 'n', 'h','(', Ion::Charset::IComplex, '-', '4',')', 0};
  a = Expression::parse(expText6);
  e = a->evaluate(globalContext, Expression::AngleUnit::Radian);
  assert(-1.0002790562f - 0.000001f <= (((Complex *)e)->a()) &&
     (((Complex *)e)->a()) <= -1.0002790562f + 0.000001f &&
      0.000610240921f - 0.000001f <= (((Complex *)e)->b()) &&
     (((Complex *)e)->b()) <=  0.000610240921f + 0.000001f);
  delete a;
  delete e;
}
