#ifndef REGRESSION_DATA_H
#define REGRESSION_DATA_H

#include "../curve_view_window.h"
#include "../data.h"

namespace Regression {

class Data : public CurveViewWindow, public ::Data {
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
  float xCursorPosition();
  float yCursorPosition();

  //CurveViewWindow
  float xMin() override;
  float xMax() override;
  float yMin() override;
  float yMax() override;
  float xGridUnit() override;
  float yGridUnit() override;

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
  float covariance();
  float slope();
  float yIntercept();

  constexpr static int k_maxNumberOfPairs = 500;
private:
  float maxXValue();
  float maxYValue();
  float minXValue();
  float minYValue();

  void initWindowParameters();
  void initCursorPosition();
  // Raw numeric data
  int m_xValues[k_maxNumberOfPairs];
  float m_yValues[k_maxNumberOfPairs];
  // Cursor
  float m_xCursorPosition;
  float m_yCursorPosition;
  // Window bounds of the data
  float m_xMin;
  float m_xMax;
  float m_yMin;
  float m_yMax;
  float m_xGridUnit;
  float m_yGridUnit;
};

}

#endif
