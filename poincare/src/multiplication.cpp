#include <poincare/multiplication.h>
#include <poincare/multiplication_explicite.h>
#include <poincare/addition.h>
#include <poincare/arithmetic.h>
#include <poincare/division.h>
#include <poincare/matrix.h>
#include <poincare/opposite.h>
#include <poincare/parenthesis.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/subtraction.h>
#include <poincare/tangent.h>
#include <poincare/undefined.h>
#include <cmath>
#include <ion.h>
#include <assert.h>

namespace Poincare {

ExpressionNode::Sign MultiplicationNode::sign(Context * context) const {
  if (numberOfChildren() == 0) {
    return Sign::Unknown;
  }
  int sign = 1;
  for (ExpressionNode * c : children()) {
    sign *= (int)(c->sign(context));
  }
  if (sign == 0) {
    return ExpressionNode::sign(context);
  }
  return (Sign)sign;
}

int MultiplicationNode::polynomialDegree(Context * context, const char * symbolName) const {
  int degree = 0;
  for (ExpressionNode * c : children()) {
    int d = c->polynomialDegree(context, symbolName);
    if (d < 0) {
      return -1;
    }
    degree += d;
  }
  return degree;
}

int MultiplicationNode::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  return Multiplication(this).getPolynomialCoefficients(context, symbolName, coefficients);
}

template<typename T>
MatrixComplex<T> MultiplicationNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat) {
  if (m.numberOfColumns() != n.numberOfRows()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int i = 0; i < m.numberOfRows(); i++) {
    for (int j = 0; j < n.numberOfColumns(); j++) {
      std::complex<T> c(0.0);
      for (int k = 0; k < m.numberOfColumns(); k++) {
        c += m.complexAtIndex(i*m.numberOfColumns()+k)*n.complexAtIndex(k*n.numberOfColumns()+j);
      }
      result.addChildAtIndexInPlace(Complex<T>::Builder(c), i*n.numberOfColumns()+j, result.numberOfChildren());
    }
  }
  result.setDimensions(m.numberOfRows(), n.numberOfColumns());
  return result;
}

bool MultiplicationNode::childNeedsUserParentheses(const Expression & child) const {
  if ((child.isNumber() && static_cast<const Number &>(child).sign() == Sign::Negative)
        || child.type() == ExpressionNode::Type::Opposite)
  {
    if (child.node() == childAtIndex(0)) {
      return false;
    }
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Addition};
  return child.isOfType(types, 2);
}

/* Multiplication */

template<typename T>
void Multiplication::computeOnArrays(T * m, T * n, T * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns) {
  for (int i = 0; i < mNumberOfRows; i++) {
    for (int j = 0; j < nNumberOfColumns; j++) {
      T res = 0.0f;
      for (int k = 0; k < mNumberOfColumns; k++) {
        res+= m[i*mNumberOfColumns+k]*n[k*nNumberOfColumns+j];
      }
      result[i*nNumberOfColumns+j] = res;
    }
  }
}

int Multiplication::getPolynomialCoefficients(Context * context, const char * symbolName, Expression coefficients[]) const {
  int deg = polynomialDegree(context, symbolName);
  if (deg < 0 || deg > Expression::k_maxPolynomialDegree) {
    return -1;
  }
  // Initialization of coefficients
  for (int i = 1; i <= deg; i++) {
    coefficients[i] = Rational::Builder(0);
  }
  coefficients[0] = Rational::Builder(1);

  Expression intermediateCoefficients[Expression::k_maxNumberOfPolynomialCoefficients];
  // Let's note result = a(0)+a(1)*X+a(2)*X^2+a(3)*x^3+..
  for (int i = 0; i < numberOfChildren(); i++) {
    // childAtIndex(i) = b(0)+b(1)*X+b(2)*X^2+b(3)*x^3+...
    int degI = childAtIndex(i).getPolynomialCoefficients(context, symbolName, intermediateCoefficients);
    assert(degI <= Expression::k_maxPolynomialDegree);
    for (int j = deg; j > 0; j--) {
      // new coefficients[j] = b(0)*a(j)+b(1)*a(j-1)+b(2)*a(j-2)+...
      Addition a = Addition::Builder();
      int jbis = j > degI ? degI : j;
      for (int l = 0; l <= jbis ; l++) {
        // Always copy the a and b coefficients are they are used multiple times
        a.addChildAtIndexInPlace(MultiplicationExplicite::Builder(intermediateCoefficients[l].clone(), coefficients[j-l].clone()), a.numberOfChildren(), a.numberOfChildren());
      }
      /* a(j) and b(j) are used only to compute coefficient at rank >= j, we
       * can delete them as we compute new coefficient by decreasing ranks. */
      coefficients[j] = a;
    }
    // new coefficients[0] = a(0)*b(0)
    coefficients[0] = MultiplicationExplicite::Builder(coefficients[0], intermediateCoefficients[0]);
  }
  return deg;
}

template MatrixComplex<float> MultiplicationNode::computeOnComplexAndMatrix<float>(std::complex<float> const, const MatrixComplex<float>, Preferences::ComplexFormat);
template MatrixComplex<double> MultiplicationNode::computeOnComplexAndMatrix<double>(std::complex<double> const, const MatrixComplex<double>, Preferences::ComplexFormat);
template Complex<float> MultiplicationNode::compute<float>(const std::complex<float>, const std::complex<float>, Preferences::ComplexFormat);
template Complex<double> MultiplicationNode::compute<double>(const std::complex<double>, const std::complex<double>, Preferences::ComplexFormat);
template void Multiplication::computeOnArrays<double>(double * m, double * n, double * result, int mNumberOfColumns, int mNumberOfRows, int nNumberOfColumns);

}
