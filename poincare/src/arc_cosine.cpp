#include <poincare/arc_cosine.h>
#include <poincare/undefined.h>
#include <poincare/trigonometry.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ArcCosine::type() const {
  return Type::ArcCosine;
}

Expression * ArcCosine::clone() const {
  ArcCosine * a = new ArcCosine(m_operands, true);
  return a;
}

Expression * ArcCosine::immediateSimplify(Context& context, AngleUnit angleUnit) {
  float approxOp = operand(0)->approximate<float>(context, angleUnit);
  if (approxOp > 1.0f || approxOp < -1.0f) {
    return replaceWith(new Undefined(), true);
  }
  if (operand(0)->type() == Type::Cosine) {
    float cosOp = operand(0)->operand(0)->approximate<float>(context, angleUnit);
    if (cosOp >= 0.0f && cosOp <= M_PI) {
      return replaceWith(const_cast<Expression *>(operand(0)->operand(0)), true);
    } else if (!isnan(cosOp)) {
      return replaceWith(new Undefined(), true);
    }
  }
  Expression * lookup = Trigonometry::table(operand(0), Trigonometry::Function::Cosine, true, context, angleUnit);
  if (lookup != nullptr) {
    return replaceWith(lookup, true);
  }
  return this;
}

template<typename T>
Complex<T> ArcCosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  T result = std::acos(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex<T>::Float(result*180/M_PI);
  }
  return Complex<T>::Float(result);
}

}
