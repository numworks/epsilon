#ifndef APPS_CURVE_VIEW_RANGE_H
#define APPS_CURVE_VIEW_RANGE_H

class CurveViewRange {
public:
  enum class Axis {
    X,
    Y
  };
  virtual float xMin() = 0;
  virtual float xMax() = 0;
  virtual float yMin() = 0;
  virtual float yMax() = 0;
  virtual float xGridUnit() = 0;
  virtual float yGridUnit();
  float computeGridUnit(Axis axis, float min, float max);
protected:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_minNumberOfYGridUnits = 5.0f;
  constexpr static float k_maxNumberOfYGridUnits = 13.0f;
  constexpr static float k_oneUnit = 1.0f;
  constexpr static float k_twoUnit = 2.0f;
  constexpr static float k_fiveUnit = 5.0f;
};

#endif
