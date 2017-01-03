#ifndef REGRESSION_DATA_H
#define REGRESSION_DATA_H

#include "../curve_view_window_with_cursor.h"
#include "../data.h"

namespace Regression {

class Data : public ::Data, public CurveViewWindowWithCursor {
public:
  Data();
  // Delete the implicit copy constructor: the object is heavy
  Data(const Data&) = delete;

  // Raw numeric data
  float xValueAtIndex(int index) override;
  float yValueAtIndex(int index) override;
  void setXValueAtIndex(float value, int index) override;
  void setYValueAtIndex(float value, int index) override;
  void deletePairAtIndex(int index) override;

  // Cursor
  int moveCursorHorizontally(int direction) override;
  // the result of moveCursorVertically means:
  // 0-> the window has not changed  1->the window changed
  int moveCursorVertically(int direction) override;

  // Window
  void setDefault();

  // Calculation
  float xSum();
  float ySum();
  float xSquaredValueSum();
  float ySquaredValueSum();
  float xyProductSum();
  float xMean();
  float yMean();
  float xVariance();
  float yVariance();
  float xStandardDeviation();
  float yStandardDeviation();
  float covariance();
  float slope();
  float yIntercept();
  float yValueForXValue(float x);
  float correlationCoefficient();
  float squaredCorrelationCoefficient();

  constexpr static int k_maxNumberOfPairs = 500;
private:
  float maxXValue();
  float maxYValue();
  float minXValue();
  float minYValue();

  void initWindowParameters();
  bool computeYaxis() override;
  void initCursorPosition() override;

  bool selectClosestDotRelativelyToCurve(int direction);
  bool selectNextDot(int direction);

  // Raw numeric data
  int m_xValues[k_maxNumberOfPairs];
  float m_yValues[k_maxNumberOfPairs];
  // Cursor
  bool m_dotsSelected;
};

}

#endif
