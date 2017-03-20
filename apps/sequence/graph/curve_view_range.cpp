#include "curve_view_range.h"
#include <math.h>
#include <ion.h>
#include <poincare.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

CurveViewRange::CurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate) :
  InteractiveCurveViewRange(cursor, delegate)
{
  m_xMin = -k_displayLeftMarginRatio*m_xMax;
}

void CurveViewRange::roundAbscissa() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  m_xMin = roundf((xMin+xMax)/2) - (float)Ion::Display::Width/2.0f;
  m_xMax = roundf((xMin+xMax)/2) + (float)Ion::Display::Width/2.0f-1.0f;
  if (m_xMin < 0.0f) {
    m_xMin = -k_displayLeftMarginRatio*(float)Ion::Display::Width;
    m_xMax = m_xMin+(float)Ion::Display::Width;
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

void CurveViewRange::normalize() {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  m_xMin = (xMin+xMax)/2 - 5.3f;
  m_xMax = (xMin+xMax)/2 + 5.3f;
  if (m_xMin < 0.0f) {
    m_xMin = -k_displayLeftMarginRatio*2.0f*5.3f;
    m_xMax = m_xMin + 2.0f*5.3f;
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = (yMin+yMax)/2 - 3.1f;
  m_yMax = (yMin+yMax)/2 + 3.1f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void CurveViewRange::setTrigonometric() {
  m_xMin = -k_displayLeftMarginRatio*21.0f;
  m_xMax = 21.0f;
  if (Preferences::sharedPreferences()->angleUnit() == Expression::AngleUnit::Degree) {
    m_xMin = -k_displayLeftMarginRatio*1200;
    m_xMax = 1200;
  }
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  m_yAuto = false;
  m_yMin = -1.6f;
  m_yMax = 1.6f;
  m_yGridUnit = computeGridUnit(Axis::Y, m_yMin, m_yMax);
}

void CurveViewRange::setDefault() {
  m_xMax = 10.0f;
  m_xMin = -k_displayLeftMarginRatio*m_xMax;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  setYAuto(true);
}

}
