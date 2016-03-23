#include <poincare/float.h>
#include <assert.h>

Float::Float(float f) : m_float(f) {
}

Float::~Float() {
}

float Float::approximate(Context& context) {
  return m_float;
}

expression_type_t Float::type() {
  return Float::Type;
}


ExpressionLayout * Float::createLayout(ExpressionLayout * parent) {
  assert(0); // Should not come here, ever...
  return nullptr;
}
