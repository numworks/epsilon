#include "chi2_test.h"
#include "goodness_test.h"
#include "homogeneity_test.h"
#include "interfaces/significance_tests.h"

#include <algorithm>
#include <cmath>

namespace Inference {

Chi2Test::Chi2Test() : Test() {
  m_hypothesisParams.setComparisonOperator(HypothesisParams::ComparisonOperator::Higher);  // Always higher
}

bool Chi2Test::initializeCategoricalType(CategoricalType type) {
  if (type == categoricalType()) {
    return false;
  }
  this->~Chi2Test();
  switch (type) {
    case CategoricalType::Homogeneity:
      new (this) HomogeneityTest();
      break;
    case CategoricalType::GoodnessOfFit:
      new (this) GoodnessTest();
      break;
    default:
      assert(false);
  }
  return true;
}

Chi2Test::Index2D Chi2Test::computeInnerDimensions() const {
  int maxCol = -1, maxRow = -1;
  for (int row = 0; row < maxNumberOfRows(); row++) {
    for (int col = 0; col < maxNumberOfColumns(); col++) {
      double p = parameterAtPosition(row, col);
      if (!std::isnan(p)) {
        if (row > maxRow) {
          maxRow = row;
        }
        if (col > maxCol) {
          maxCol = col;
        }
      }
    }
  }
  return Index2D{.row = maxRow + 1, .col = maxCol + 1};
}

Chi2Test::Index2D Chi2Test::computeDimensions() const {
  Chi2Test::Index2D dim = computeInnerDimensions();
  bool displayLastEmptyRow = dim.row < maxNumberOfRows();
  bool displayLastEmptyColumn = dim.col < maxNumberOfColumns();
  Chi2Test::Index2D initialDim = initialDimensions();
  return Index2D{.row = std::max(initialDim.row, dim.row + displayLastEmptyRow), .col = std::max(initialDim.col, dim.col + displayLastEmptyColumn)};
}

double Chi2Test::computeChi2() {
  double z = 0;
  int n = numberOfValuePairs();
  for (int i = 0; i < n; i++) {
    z += std::pow(expectedValue(i) - observedValue(i), 2) / expectedValue(i);
  }
  return z;
}

void Chi2Test::setParameterAtPosition(double p, int row, int column) {
  assert(index2DToIndex(row, column) < indexOfThreshold());
  setParameterAtIndex(p, index2DToIndex(row, column));
}

void Chi2Test::setParameterAtIndex(double p, int i) {
  if (i == indexOfDegreeOfFreedom()) {
    m_degreesOfFreedom = p;
  } else {
    return Statistic::setParameterAtIndex(p, i);
  }
}

double Chi2Test::parameterAtPosition(int row, int column) const {
  return parameterAtIndex(index2DToIndex(row, column));
}

bool Chi2Test::authorizedParameterAtPosition(double p, int row, int column) const {
  return authorizedParameterAtIndex(p, index2DToIndex(row, column));
}

bool Chi2Test::authorizedParameterAtIndex(double p, int i) const {
  if (i == indexOfDegreeOfFreedom() && (p != std::round(p) || p < 1.0)) {
    return false;
  }
  if (i == indexOfThreshold() && !SignificanceTest::ValidThreshold(p)) {
    return false;
  }
  return Inference::authorizedParameterAtIndex(p, i);
}

void Chi2Test::deleteParametersInColumn(int column) {
  for (int i = 0; i < maxNumberOfRows(); i++) {
    setParameterAtPosition(k_undefinedValue, i, column);
  }
}

bool Chi2Test::deleteParameterAtPosition(int row, int column) {
  if (std::isnan(parameterAtPosition(row, column))) {
    // Param is already deleted
    return false;
  }
  setParameterAtPosition(k_undefinedValue, row, column);
  for (int i = 0; i < maxNumberOfColumns(); i++) {
    if (i != column && !std::isnan(parameterAtPosition(row, i))) {
      // There is another non deleted value in this row
      return false;
    }
  }
  return true;
}

Chi2Test::Index2D Chi2Test::indexToIndex2D(int index) const {
  return Chi2Test::Index2D{
      .row = index / maxNumberOfColumns(),
      .col = index % maxNumberOfColumns()
    };
}

int Chi2Test::index2DToIndex(Index2D indexes) const {
  return index2DToIndex(indexes.row, indexes.col);
}

int Chi2Test::index2DToIndex(int row, int column) const {
  return column + row * maxNumberOfColumns();
}

}  // namespace Inference
