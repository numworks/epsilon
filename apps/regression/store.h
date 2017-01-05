#ifndef REGRESSION_STORE_H
#define REGRESSION_STORE_H

#include "../curve_view_window_with_cursor.h"
#include "../float_pair_store.h"

namespace Regression {

class Store : public CurveViewWindowWithCursor, public FloatPairStore {
public:
  Store();
  // Cursor
  void initCursorPosition() override;
  // the result of moveCursorVertically means:
  // 0-> the window has not changed  1->the window changed
  int moveCursorVertically(int direction) override;
  int moveCursorHorizontally(int direction) override;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int selectedDotIndex();
  void setCursorPositionAtAbscissa(float abscissa);
  void setCursorPositionAtOrdinate(float ordinate);

  // Window
  void initWindowParameters();
  void setDefault();

  // Calculation
  float squaredValueSumOfColumn(int i);
  float columnProductSum();
  float meanOfColumn(int i);
  float varianceOfColumn(int i);
  float standardDeviationOfColumn(int i);
  float covariance();
  float slope();
  float yIntercept();
  float yValueForXValue(float x);
  float xValueForYValue(float y);
  float correlationCoefficient();
  float squaredCorrelationCoefficient();
private:
  float maxValueOfColumn(int i);
  float minValueOfColumn(int i);

  bool computeYaxis() override;
  int selectClosestDotRelativelyToCurve(int direction);
  int selectNextDot(int direction);

  // Cursor
  int m_selectedDotIndex;
};

}

#endif
