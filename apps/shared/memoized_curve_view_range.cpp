#include "memoized_curve_view_range.h"
#include <cmath>
#include <assert.h>
#include <ion.h>

namespace Shared {

MemoizedCurveViewRange::MemoizedCurveViewRange() :
  m_xRange(),
  m_yRange(),
  m_xGridUnit(2.0f),
  m_yGridUnit(2.0f)
{
}

void MemoizedCurveViewRange::protectedSetXMin(float xMin, float lowerMaxFloat, float upperMaxFloat) {
  m_xRange.setMin(xMin, lowerMaxFloat, upperMaxFloat);
  m_xGridUnit = CurveViewRange::xGridUnit();
}

void MemoizedCurveViewRange::protectedSetXMax(float xMax, float lowerMaxFloat, float upperMaxFloat) {
  m_xRange.setMax(xMax, lowerMaxFloat, upperMaxFloat);
  m_xGridUnit = CurveViewRange::xGridUnit();
}

void MemoizedCurveViewRange::protectedSetYMin(float yMin, float lowerMaxFloat, float upperMaxFloat) {
  m_yRange.setMin(yMin, lowerMaxFloat, upperMaxFloat);
  m_yGridUnit = CurveViewRange::yGridUnit();
}

void MemoizedCurveViewRange::protectedSetYMax(float yMax, float lowerMaxFloat, float upperMaxFloat) {
  m_yRange.setMax(yMax, lowerMaxFloat, upperMaxFloat);
  m_yGridUnit = CurveViewRange::yGridUnit();
}

}
