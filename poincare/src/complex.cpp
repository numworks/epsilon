extern "C" {
#include <assert.h>
#include <float.h>
#include <stdlib.h>
}
#include <poincare/complex.h>
//#include <poincare/division.h>
//#include <poincare/matrix.h>
#include <poincare/expression_reference.h>
#include <poincare/undefined.h>
#include <poincare/infinity.h>
//#include <poincare/decimal.h>
//#include <poincare/multiplication.h>
#include <poincare/opposite.h>
//#include <poincare/addition.h>
//#include <poincare/subtraction.h>
//#include <poincare/matrix.h>
//#include <poincare/power.h>
#include <poincare/symbol.h>
#include <ion.h>
#include <cmath>

namespace Poincare {

template<typename T>
void ComplexNode<T>::setComplex(std::complex<T> c) {
  this->real(c.real());
  this->imag(c.imag());
  if (this->real() == -0) {
    this->real(0);
  }
  if (this->imag() == -0) {
    this->imag(0);
  }
}

template<typename T>
T ComplexNode<T>::toScalar() const {
  if (this->imag() == 0.0) {
    return this->real();
  }
  return NAN;
}

template <typename T>
static ExpressionReference CreateDecimal(T f) {
  if (std::isnan(f)) {
    return UndefinedReference();
  }
  if (std::isinf(f)) {
    return InfiniteReference(f < 0.0);
  }
  return DecimalReference(f);
}

template<typename T>
ExpressionReference ComplexNode<T>::complexToExpression(Preferences::ComplexFormat complexFormat) const {
  if (std::isnan(this->real()) || std::isnan(this->imag())) {
    return UndefinedReference();
  }
  switch (complexFormat) {
    case Preferences::ComplexFormat::Cartesian:
    {
      ExpressionReference real(nullptr);
      ExpressionReference imag(nullptr);
      if (this->real() != 0 || this->imag() == 0) {
        real = CreateDecimal<T>(this->real());
      }
      if (this->imag() != 0) {
        if (this->imag() == 1.0 || this->imag() == -1) {
          imag = SymbolReference(Ion::Charset::IComplex);
        } else if (this->imag() > 0) {
          imag = MultiplicationReference(CreateDecimal(this->imag()), SymbolReference(Ion::Charset::IComplex));
        } else {
          imag = MultiplicationReference(CreateDecimal(-this->imag()), SymbolReference(Ion::Charset::IComplex));
        }
      }
      if (!imag.isDefined()) {
        return real;
      } else if (!real.isDefined()) {
        if (this->imag() > 0) {
          return imag;
        } else {
          return OppositeReference(imag);
        }
        return imag;
      } else if (this->imag() > 0) {
        return AdditionReference(real, imag);
      } else {
        return SubtractionReference(real, imag);
      }
    }
    default:
    {
      assert(complexFormat == Preferences::ComplexFormat::Polar);
      ExpressionReference norm(nullptr);
      ExpressionReference exp(nullptr);
      T r = std::abs(*this);
      T th = std::arg(*this);
      if (r != 1 || th == 0) {
        norm = CreateDecimal(r);
      }
      if (r != 0 && th != 0) {
        ExpressionReference arg(nullptr);
        if (th == 1.0) {
          arg = SymbolReference(Ion::Charset::IComplex);
        } else if (th == -1.0) {
          arg = OppositeReference(SymbolReference(Ion::Charset::IComplex));
        } else if (th > 0) {
          arg = MultiplicationReference(CreateDecimal(th), SymbolReference(Ion::Charset::IComplex));
        } else {
          arg = OppositeRefrence(MultiplicationReference(CreateDecimal(-th), SymbolReference(Ion::Charset::IComplex)));
        }
        exp = PowerReference(SymbolReference(Ion::Charset::Exponential), arg);
      }
      if (!exp.isDefined()) {
        return norm;
      } else if (!norm.isDefined()) {
        return exp;
      } else {
        return MultiplicationReference(norm, exp);
      }
    }
  }
}

template<typename T>
EvaluationReference<T> ComplexNode<T>::inverse() const {
  return ComplexReference<T>(Division::compute(std::complex<T>(1.0), *this));
}

template<typename T>
ComplexReference<T>::ComplexReference(std::complex<T> c) :
  EvaluationReference<T>()
{
  TreeNode * node = TreePool::sharedPool()->createTreeNode<ComplexNode<T>>();
  this->m_identifier = node->identifier();
  if (!(node->isAllocationFailure())) {
    static_cast<ComplexNode<T> *>(node)->setComplex(c);
  }
}

template class ComplexReference<float>;
template class ComplexReference<double>;

}
