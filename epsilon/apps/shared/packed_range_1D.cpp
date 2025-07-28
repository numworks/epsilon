#include "packed_range_1D.h"

using namespace Poincare;

namespace Shared {

void PackedRange1D::privateSet(float t, float limit,
                               void (Range1D<float>::*setter)(float, float)) {
  Range1D<float> range(m_min, m_max, limit);
  (range.*setter)(t, limit);
  m_min = range.min();
  m_max = range.max();
}

}  // namespace Shared
