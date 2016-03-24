#include <poincare/float.h>
#include <assert.h>

Float::Float(float f) : m_float(f) {
}

Float::~Float() {
}

float Float::approximate(Context& context) {
  return m_float;
}

Expression::Type Float::type() {
  return Expression::Type::Float;
}

ExpressionLayout * Float::createLayout(ExpressionLayout * parent) {
  assert(0); // Should not come here, ever...
  return nullptr;
}
