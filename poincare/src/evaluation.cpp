#include <poincare/evaluation.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/complex_matrix.h>
#include <poincare/addition.h>
#include <poincare/complex.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

template<typename T>
bool Evaluation<T>::hasValidNumberOfArguments() const {
  return true;
}

template<typename T>
const Expression * Evaluation<T>::operand(int i) const {
  return complexOperand(i);
}

template<typename T>
Evaluation<T> * Evaluation<T>::createTrace() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  Complex<T> c = Complex<T>::Float(0);
  for (int i = 0; i < dim; i++) {
    c = Addition::compute(c, *complexOperand(i*dim+i));
  }
  return new Complex<T>(c);
}

template<typename T>
// TODO: implement determinant for complex matrix?
Evaluation<T> * Evaluation<T>::createDeterminant() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  T ** tempMat = new T*[dim];
  for (int i = 0; i < dim; i++) {
    tempMat[i] = new T[dim];
  }
  T det = 1;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      tempMat[i][j] = complexOperand(i*dim+ j)->toScalar();
    }
  }

  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim-1; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::fabs(tempMat[rowWithPivot][i]) < std::fabs(tempMat[row][i])) {
        rowWithPivot = row;
      }
    }
    T valuePivot = tempMat[rowWithPivot][i];
    /* if the pivot is null, det = 0. */
    if (std::fabs(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(tempMat[i]);
      }
      free(tempMat);
      return new Complex<T>(Complex<T>::Float(0.0f));
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        T temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
    /* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      T factor = tempMat[row][i]/valuePivot;
      for (int col = i; col < dim; col++) {
        tempMat[row][col] -= factor*tempMat[i][col];
      }
    }
  }
  det *= tempMat[dim-1][dim-1];
  for (int i = 0; i < dim; i++) {
    delete[] tempMat[i];
  }
  delete[] tempMat;
  return new Complex<T>(Complex<T>::Float(det));
}

template<typename T>
Evaluation<T> * Evaluation<T>::createInverse() const {
  if (numberOfRows() != numberOfColumns()) {
    return new Complex<T>(Complex<T>::Float(NAN));
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  T ** inv = new T*[dim];
  for (int i = 0; i < dim; i++) {
    inv[i] = new T [2*dim];
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      inv[i][j] = complexOperand(i*dim+j)->toScalar();
    }
    for (int j = dim; j < 2*dim; j++) {
      inv[i][j] = (i+dim == j);
    }
  }
  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (std::fabs(inv[rowWithPivot][i]) < std::fabs(inv[row][i])) {
        rowWithPivot = row;
      }
    }
    T valuePivot = inv[rowWithPivot][i];
    /* if the pivot is null, the matrix in not invertible. */
    if (std::fabs(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(inv[i]);
      }
      free(inv);
      return new Complex<T>(Complex<T>::Float(NAN));
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        T temp = inv[i][col];
        inv[i][col] = inv[rowWithPivot][col];
        inv[rowWithPivot][col] = temp;
      }
    }
    /* A[pivot][] = A[pivot][]/valuePivot */
    for (int col = 0; col < 2*dim; col++) {
      inv[i][col] /= valuePivot;
    }
    /* Set to 0 all A[][row] by linear combination */
    for (int row = 0; row < dim; row++) {
      if (row == i) {
        continue;
      }
      T factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  Complex<T> * operands = new Complex<T>[numberOfOperands()];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = Complex<T>(Complex<T>::Float(inv[i][j+dim]));
    }
  }
  for (int i = 0; i < dim; i++) {
    delete[] inv[i];
  }
  delete[] inv;
  // Intentionally swapping dimensions for inverse, although it doesn't make a difference because it is square
  Evaluation<T> * matrix = new ComplexMatrix<T>(operands, numberOfColumns(), numberOfRows());
  delete[] operands;
  return matrix;
}

template<typename T>
Evaluation<T> * Evaluation<T>::createTranspose() const {
  Complex<T> * operands = new Complex<T>[numberOfOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = *(complexOperand(i*numberOfColumns()+j));
    }
  }
  // Intentionally swapping dimensions for transpose
  Evaluation<T> * matrix = new ComplexMatrix<T>(operands, numberOfColumns(), numberOfRows());
  delete[] operands;
  return matrix;
}

template class Poincare::Evaluation<float>;
template class Poincare::Evaluation<double>;

}
