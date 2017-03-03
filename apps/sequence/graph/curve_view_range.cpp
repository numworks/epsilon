#include "curve_view_range.h"

using namespace Shared;

namespace Sequence {

CurveViewRange::CurveViewRange(CurveViewCursor * cursor, InteractiveCurveViewRangeDelegate * delegate) :
  InteractiveCurveViewRange(cursor, delegate)
{
  m_xMin = -k_displayLeftMarginRatio*m_xMax;
}

void CurveViewRange::setDefault() {
  m_xMax = 10.0f;
  m_xMin = -k_displayLeftMarginRatio*m_xMax;
  m_xGridUnit = computeGridUnit(Axis::X, m_xMin, m_xMax);
  setYAuto(true);
}

}
