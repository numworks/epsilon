#include <poincare/range.h>

namespace Poincare {

void Range1D::extend(float t) {
  assert(!std::isnan(t));
  /* Using this syntax for the comparison takes care of the NAN. */
  if (!(t >= m_min)) {
    m_min = t;
  }
  if (!(t <= m_max)) {
    m_max = t;
  }
}

void Range1D::zoom(float ratio, float center) {
  m_min = (m_min - center) / ratio + center;
  m_max = (m_max - center) / ratio + center;
}

}
