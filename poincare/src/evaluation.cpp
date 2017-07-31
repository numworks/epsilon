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
#include <math.h>
#include <float.h>
#include <string.h>

namespace Poincare {

Expression::Type Evaluation::type() const {
  return Type::Evaluation;
}

bool Evaluation::hasValidNumberOfArguments() const {
  return true;
}

const Expression * Evaluation::operand(int i) const {
  return complexOperand(i);
}

Evaluation * Evaluation::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(false);
  return this->clone();
}

Evaluation * Evaluation::createTrace() const {
  if (numberOfColumns() != numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  int dim = numberOfRows();
  Complex c = Complex::Float(0.0f);
  for (int i = 0; i < dim; i++) {
    c = Addition::compute(c, *complexOperand(i*dim+i));
  }
  return new Complex(c);
}

// TODO: implement determinant for complex matrix?
Evaluation * Evaluation::createDeterminant() const {
  if (numberOfColumns() != numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  int dim = numberOfRows();
  float ** tempMat = new float*[dim];
  for (int i = 0; i < dim; i++) {
    tempMat[i] = new float[dim];
  }
  float det = 1.0f;
  /* Copy the matrix */
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      tempMat[i][j] = complexOperand(i*dim+ j)->toFloat();
    }
  }

  /* Main Loop: Gauss pivot */
  for (int i = 0; i < dim-1; i++) {
    /* Search for pivot */
    int rowWithPivot = i;
    for (int row = i+1; row < dim; row++) {
      if (fabsf(tempMat[rowWithPivot][i]) < fabsf(tempMat[row][i])) {
        rowWithPivot = row;
      }
    }
    float valuePivot = tempMat[rowWithPivot][i];
    /* if the pivot is null, det = 0. */
    if (fabsf(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(tempMat[i]);
      }
      free(tempMat);
      return new Complex(Complex::Float(0.0f));
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < dim; col++) {
        float temp = tempMat[i][col];
        tempMat[i][col] = tempMat[rowWithPivot][col];
        tempMat[rowWithPivot][col] = temp;
      }
      det *= -1;
    }
    det *= valuePivot;
    /* Set to 0 all A[][i] by linear combination */
    for (int row = i+1; row < dim; row++) {
      float factor = tempMat[row][i]/valuePivot;
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
  return new Complex(Complex::Float(det));
}

Evaluation * Evaluation::createInverse() const {
  if (numberOfColumns() != numberOfRows()) {
    return new Complex(Complex::Float(NAN));
  }
  int dim = numberOfRows();
  /* Create the matrix inv = (A|I) with A the input matrix and I the dim identity matrix */
  float ** inv = new float*[dim];
  for (int i = 0; i < dim; i++) {
    inv[i] = new float [2*dim];
  }
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      inv[i][j] = complexOperand(i*dim+j)->toFloat();
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
      if (fabsf(inv[rowWithPivot][i]) < fabsf(inv[row][i])) {
        rowWithPivot = row;
      }
    }
    float valuePivot = inv[rowWithPivot][i];
    /* if the pivot is null, the matrix in not invertible. */
    if (fabsf(valuePivot) <= FLT_EPSILON) {
      for (int i = 0; i < dim; i++) {
        free(inv[i]);
      }
      free(inv);
      return new Complex(Complex::Float(NAN));
    }
    /* Switch rows to have the pivot row as first row */
    if (rowWithPivot != i) {
      for (int col = i; col < 2*dim; col++) {
        float temp = inv[i][col];
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
      float factor = inv[row][i];
      for (int col = 0; col < 2*dim; col++) {
        inv[row][col] -= factor*inv[i][col];
      }
    }
  }
  Complex * operands = new Complex[numberOfOperands()];
  for (int i = 0; i < dim; i++) {
    for (int j = 0; j < dim; j++) {
      operands[i*dim+j] = Complex(Complex::Float(inv[i][j+dim]));
    }
  }
  for (int i = 0; i < dim; i++) {
    delete[] inv[i];
  }
  delete[] inv;
  Evaluation * matrix = new ComplexMatrix(operands, numberOfColumns(), numberOfRows());
  delete[] operands;
  return matrix;
}

Evaluation * Evaluation::createTranspose() const {
  Complex * operands = new Complex[numberOfOperands()];
  for (int i = 0; i < numberOfRows(); i++) {
    for (int j = 0; j < numberOfColumns(); j++) {
      operands[j*numberOfRows()+i] = *(complexOperand(i*numberOfColumns()+j));
    }
  }
  Evaluation * matrix = new ComplexMatrix(operands, numberOfRows(), numberOfColumns());
  delete[] operands;
  return matrix;
}

}
