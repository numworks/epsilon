#include <stdlib.h>
#include <string.h>
extern "C" {
#include <assert.h>
}

#include <poincare/float.h>

Float::Float(float f) : m_float(f) {
}

Float::~Float() {
}

Expression * Float::clone() {
  return new Float(m_float);
}

float Float::approximate(Context& context) {
  return m_float;
}

Expression::Type Float::type() {
  return Expression::Type::Float;
}

ExpressionLayout * Float::createLayout() {
  assert(0); // Should not come here, ever...
  return nullptr;
}

bool Float::valueEquals(Expression * e) {
  assert(e->type() == Expression::Type::Float);
  return m_float == ((Float *)e)->m_float;
}
