#include <poincare/arc_tangent.h>
#include <poincare/arc_tangent.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include <poincare/derivative.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/square_root.h>
#include <poincare/trigonometry.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper ArcCotangent::s_functionHelper;

int ArcCotangentNode::numberOfChildren() const { return ArcCotangent::s_functionHelper.numberOfChildren(); }

template<typename T>
Complex<T> ArcCotangentNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) {
  if (c == (T)0.0) {
    return Complex<T>::Undefined();
  }
  return ArcTangentNode::computeOnComplex<T>(std::complex<T>(1) / c, complexFormat, angleUnit);
}

Layout ArcCotangentNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(ArcCotangent(this), floatDisplayMode, numberOfSignificantDigits, ArcCotangent::s_functionHelper.name());
}

int ArcCotangentNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, ArcCotangent::s_functionHelper.name());
}

Expression ArcCotangentNode::shallowReduce(ReductionContext reductionContext) {
  return ArcCotangent(this).shallowReduce(reductionContext);
}

Expression ArcCotangent::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = SimplificationHelper::defaultShallowReduce(*this);
    if (!e.isUninitialized()) {
      return e;
    }
  }
  return Trigonometry::shallowReduceInverseAdvancedFunction(*this, reductionContext);
}

}
