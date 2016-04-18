extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
}

#include <poincare/float.h>

Float::Float(float f) : m_float(f) {
}

Float::~Float() {
}

Expression * Float::clone() const {
  return new Float(m_float);
}

float Float::approximate(Context& context) const {
  return m_float;
}

Expression::Type Float::type() const {
  return Expression::Type::Float;
}

ExpressionLayout * Float::createLayout() const {
  assert(0); // Should not come here, ever...
  return nullptr;
}

bool Float::valueEquals(const Expression * e) const {
  assert(e->type() == Expression::Type::Float);
  return m_float == ((Float *)e)->m_float;
}
