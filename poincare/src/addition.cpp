#include <poincare/addition.h>
#include <poincare/complex_matrix.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include "layout/horizontal_layout.h"
#include "layout/string_layout.h"
#include "layout/parenthesis_layout.h"

namespace Poincare {

Expression::Type Addition::type() const {
  return Type::Addition;
}

template<typename T>
Complex<T> Addition::compute(const Complex<T> c, const Complex<T> d) {
  return Complex<T>::Cartesian(c.a()+d.a(), c.b()+d.b());
}

template<typename T>
Evaluation<T> * Addition::computeOnMatrices(Evaluation<T> * m, Evaluation<T> * n) {
  Addition a;
  return a.computeOnComplexMatrices(m,n);
}

template<typename T>
Evaluation<T> * Addition::computeOnComplexAndMatrix(const Complex<T> * c, Evaluation<T> * m) {
  Addition a;
  return a.computeOnComplexAndComplexMatrix(c,m);
}

Expression * Addition::cloneWithDifferentOperands(Expression** newOperands,
    int numberOfOperands, bool cloneOperands) const {
  return new Addition(newOperands, numberOfOperands, cloneOperands);
}

char Addition::operatorChar() const {
  return '+';
}

template Poincare::Complex<float> Poincare::Addition::compute<float>(Poincare::Complex<float>, Poincare::Complex<float>);
template Poincare::Complex<double> Poincare::Addition::compute<double>(Poincare::Complex<double>, Poincare::Complex<double>);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnMatrices<float>(Poincare::Evaluation<float>*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnMatrices<double>(Poincare::Evaluation<double>*, Poincare::Evaluation<double>*);

template Poincare::Evaluation<float>* Poincare::Addition::computeOnComplexAndMatrix<float>(Poincare::Complex<float> const*, Poincare::Evaluation<float>*);
template Poincare::Evaluation<double>* Poincare::Addition::computeOnComplexAndMatrix<double>(Poincare::Complex<double> const*, Poincare::Evaluation<double>*);

}
