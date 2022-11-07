#include <poincare/range.h>

namespace Poincare {

// Range1D

Range1D Range1D::RangeBetween(float a, float b, float limit) {
  assert(!std::isnan(a) && !std::isnan(b));
  a = std::clamp(a, -limit, limit);
  Range1D res(a, a);
  (res.*(b < a ? &Range1D::setMin : &Range1D::setMax))(b, limit);
  return res;
}

void Range1D::extend(float t) {
  if (std::isnan(t)) {
    return;
  }
  /* Using this syntax for the comparison takes care of the NAN. */
  if (!(t >= m_min)) {
    m_min = t;
  }
  if (!(t <= m_max)) {
    m_max = t;
  }
}

void Range1D::zoom(float ratio, float center) {
  m_min = (m_min - center) * ratio + center;
  m_max = (m_max - center) * ratio + center;
}

void Range1D::privateSet(float t, bool isMin, float limit) {
  if (std::isnan(t)) {
    m_min = t;
    m_max = t;
    return;
  }
  float * bound = isMin ? &m_min : &m_max;
  assert(limit > 0.0);
  *bound = std::clamp(t, -limit, limit);
  if (!(m_min <= m_max)) {
    (isMin ? m_max : m_min) = *bound;
  }
  if (length() < k_minLength) {
    float l = std::max(DefaultLengthAt(m_min), k_minLength);
    if (m_min != limit && (isMin || m_max == -limit)) {
      m_max += l;
    } else {
      m_min -= l;
    }
  }
  assert(isValid() && !(isEmpty() && std::isfinite(m_max)));
}

// Range2D

static int normalizationSignificantBits(float xMin, float xMax, float yMin, float yMax) {
  float xr = std::fabs(xMin) > std::fabs(xMax) ? xMax / xMin : xMin / xMax;
  float yr = std::fabs(yMin) > std::fabs(yMax) ? yMax / yMin : yMin / yMax;
  /* The subtraction x - y induces a loss of significance of -log2(1-x/y)
   * bits. Since normalizing requires computing xMax - xMin and yMax - yMin,
   * the ratio of the normalized range will deviate from the normal ratio. We
   * add an extra two lost bits to account for loss of precision from other
   * sources. */
  float loss = std::log2(std::min(1.f - xr, 1.f - yr));
  if (loss > 0.f) {
    loss = 0.f;
  }
  return  std::floor(loss + 23.f - 2.f);
}

bool Range2D::ratioIs(float r) const {
  int significantBits = normalizationSignificantBits(xMin(), xMax(), yMin(), yMax());
  if (significantBits <= 0) {
    return false;
  }
  float thisRatio = ratio();
  /* The last N (= 23 - significantBits) bits of "ratio" mantissa have become
   * insignificant. "tolerance" is the difference between ratio with those N
   * bits set to 1, and ratio with those N bits set to 0 ; i.e. a measure of
   * the interval in which numbers are indistinguishable from ratio with this
   * level of precision. */
  float tolerance = std::pow(2.f, IEEE754<float>::exponent(thisRatio) - significantBits);
  return std::fabs(thisRatio - r) <= tolerance;
}

void Range2D::setRatio(float r, bool shrink) {
  float currentR = ratio();
  Range1D * toEdit;
  float newLength;
  if ((currentR < r) == shrink) {
    toEdit = &m_x;
    newLength = m_y.length() / r;
  } else {
    toEdit = &m_y;
    newLength = m_x.length() * r;
  }
  float c = toEdit->center();
  newLength *= 0.5f;
  *toEdit = Range1D(c - newLength, c + newLength);
}

}
