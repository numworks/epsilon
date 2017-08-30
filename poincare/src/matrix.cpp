extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <poincare/matrix.h>
#include <poincare/complex.h>
#include "layout/grid_layout.h"
#include "layout/bracket_layout.h"
#include <cmath>
#include <float.h>
#include <string.h>

namespace Poincare {

int Matrix::numberOfOperands() const {
  return numberOfRows() * numberOfColumns();
}

ExpressionLayout * Matrix::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  ExpressionLayout ** childrenLayouts = new ExpressionLayout * [numberOfOperands()];
  for (int i = 0; i < numberOfOperands(); i++) {
    childrenLayouts[i] = operand(i)->createLayout(floatDisplayMode, complexFormat);
  }
  ExpressionLayout * layout = new BracketLayout(new GridLayout(childrenLayouts, numberOfRows(), numberOfColumns()));
  delete [] childrenLayouts;
  return layout;
}

int Matrix::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize) {
    return 0;
  }
  buffer[currentChar++] = '[';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  for (int i = 0; i < numberOfRows(); i++) {
    buffer[currentChar++] = '[';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    currentChar += operand(i*numberOfColumns())->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    for (int j = 1; j < numberOfColumns(); j++) {
      buffer[currentChar++] = ',';
      if (currentChar >= bufferSize) {
        return currentChar;
      }
      currentChar += operand(i*numberOfColumns()+j)->writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, numberOfSignificantDigits);
      if (currentChar >= bufferSize) {
        return currentChar;
      }
    }
    currentChar = strlen(buffer);
    if (currentChar >= bufferSize) {
      return currentChar;
    }
    buffer[currentChar++] = ']';
    if (currentChar >= bufferSize) {
      return currentChar;
    }
  }
  buffer[currentChar++] = ']';
  if (currentChar >= bufferSize) {
    return currentChar;
  }
  buffer[currentChar] = 0;
  return currentChar;
}

}
