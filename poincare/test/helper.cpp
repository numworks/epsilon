#include <quiz.h>
#include <poincare.h>
#include <string.h>
#include <ion.h>
#include <stdlib.h>
#include <assert.h>
#include <cmath>

using namespace Poincare;

void assert_parsed_expression_evaluate_to(const char * expression, Complex * results, int numberOfEntries, Expression::AngleUnit angleUnit) {
  char buffer[200];
  strlcpy(buffer, expression, 200);
  for (size_t i=0; i<strlen(buffer); i++) {
    if (buffer[i] == 'E') {
      buffer[i] = Ion::Charset::Exponent;
    }
    if (buffer[i] == 'X') {
      buffer[i] = Ion::Charset::Exponential;
    }
    if (buffer[i] == 'I') {
      buffer[i] = Ion::Charset::IComplex;
    }
    if (buffer[i] == 'R') {
      buffer[i] = Ion::Charset::Root;
    }
    if (buffer[i] == 'P') {
      buffer[i] = Ion::Charset::SmallPi;
    }
  }
  GlobalContext globalContext;
  Expression * a = Expression::parse(buffer);
  Evaluation * m = a->evaluate(globalContext, angleUnit);
  for (int i = 0; i < numberOfEntries; i++) {
    assert(std::fabs(m->complexOperand(i)->a() - results[i].a()) < 0.0001f);
    assert(std::fabs(m->complexOperand(i)->b() - results[i].b()) < 0.0001f);
  }
  delete a;
  delete m;
}
