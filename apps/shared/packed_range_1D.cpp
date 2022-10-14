#include "packed_range_1D.h"

using namespace Poincare;

namespace Shared {

void PackedRange1D::privateSet(float t, float limit, void (Range1D::*setter)(float, float)) {
  Range1D range(m_min, m_max);
  (range.*setter)(t, limit);
  m_min = range.min();
  m_max = range.max();
}

}
