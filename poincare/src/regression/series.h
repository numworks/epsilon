#ifndef POINCARE_REGRESSION_SERIES_H
#define POINCARE_REGRESSION_SERIES_H

#include <assert.h>

namespace Poincare::Regression {

// Apps will provide series to the regression algorithm with Series
class Series {
 public:
  virtual double getX(int i) const = 0;
  virtual double getY(int i) const = 0;

#if TARGET_POINCARE_JS
  virtual ~Series() = default;
#endif

  double get(int column, int i) const {
    assert(column == 0 || column == 1);
    return column == 0 ? getX(i) : getY(i);
  }
  virtual int numberOfPairs() const = 0;
  bool numberOfDistinctAbscissaeGreaterOrEqualTo(int i) const;
};

}  // namespace Poincare::Regression
#endif
