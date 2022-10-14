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

void Range1D::privateSet(float t, float * bound, float limit) {
  if (std::isnan(t)) {
    m_min = t;
    m_max = t;
    return;
  }

  assert(bound == &m_min || bound == &m_max);
  bool isMin = bound == &m_min;
  float * other = isMin ? &m_max : &m_min;
  *bound = std::clamp(t, -limit, limit);
  if (!(m_min <= m_max)) {
    *other = *bound;
  }
  if (length() < k_minLength) {
    float l = std::max(DefaultLengthAt(m_min), k_minLength);
    if (m_min != limit && (isMin || m_max == -limit)) {
      m_max += l;
    } else {
      m_min -= l;
    }
  }
  assert(isValid() && !isEmpty());
}

}
