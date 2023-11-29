#include <poincare/range.h>

namespace Poincare {

// Range1D

Range1D Range1D::ValidRangeBetween(float a, float b, float limit) {
  assert(!std::isnan(a) && !std::isnan(b));
  float min = (a < b ? a : b);
  float max = (a < b ? b : a);
  min = std::clamp(min, -limit, limit);
  max = std::clamp(max, -limit, limit);
  Range1D res(min, max, limit);
  res.stretchIfTooSmall(-1.f, limit);

  assert(res.isValid());
  assert(-limit <= res.min() && res.min() <= limit);
  assert(-limit <= res.max() && res.max() <= limit);

  return res;
}

void Range1D::extend(float t, float limit) {
  if (std::isnan(t)) {
    return;
  }
  t = std::clamp(t, -limit, limit);
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

void Range1D::stretchEachBoundBy(float shift, float limit) {
  assert(shift >= 0.0f);
  m_min = std::max(m_min - shift, -limit);
  m_max = std::min(m_max + shift, limit);
  assert(std::isnan(length()) || length() >= k_minLength);
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

void Range1D::stretchIfTooSmall(float shift, float limit) {
  /* Handle cases where limits are too close or equal.
   * They are both shifted by shift. */
  if (length() < k_minLength) {
    shift = std::max(shift, DefaultLengthAt(m_min) / 2.0f);
    stretchEachBoundBy(shift, limit);
  }
  assert(std::isnan(length()) || length() >= k_minLength);
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

void Range1D::privateSetKeepingValid(float t, bool isMin, float limit) {
  assert(isValid());
  assert(!std::isnan(t));
  float* bound = isMin ? &m_min : &m_max;
  float* otherBound = isMin ? &m_max : &m_min;
  assert(limit > 0.0);
  *bound = std::clamp(t, -limit, limit);
  if (length() < 0) {
    *otherBound = *bound;
  }
  assert(length() >= 0);
  if (length() < k_minLength) {
    /* If the new bound is too close from the other one, set the other one to a
     * distance DefaultLength.
     * We don't call stretchIfTooSmall yet since we want to avoid to modify the
     * bound that is being set.  */
    float l = DefaultLengthAt(m_min) * (isMin ? 1 : -1);
    *otherBound = std::clamp(*bound + l, -limit, limit);

    if (length() < k_minLength) {
      /* The bounds are still too close (the other bound is at the limit). This
       * means the bound can't be set to t and needs to be shifted a bit. */
      stretchIfTooSmall(-1.0f, limit);
    }
  }
  assert(isValid());
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

// Range2D

static int normalizationSignificantBits(float xMin, float xMax, float yMin,
                                        float yMax) {
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
  return std::max(static_cast<int>(std::floor(loss + 23.f - 2.f)), 0);
}

bool Range2D::ratioIs(float r) const {
  int significantBits =
      normalizationSignificantBits(xMin(), xMax(), yMin(), yMax());
  float thisRatio = ratio();
  /* The last N (= 23 - significantBits) bits of "ratio" mantissa have become
   * insignificant. "tolerance" is the difference between ratio with those N
   * bits set to 1, and ratio with those N bits set to 0 ; i.e. a measure of
   * the interval in which numbers are indistinguishable from ratio with this
   * level of precision. */
  float tolerance =
      std::pow(2.f, OMG::IEEE754<float>::exponent(thisRatio) - significantBits);
  return std::fabs(thisRatio - r) <= tolerance;
}

bool Range2D::setRatio(float r, bool shrink, float limit) {
  float currentR = ratio();
  Range1D* toEdit;
  float newLength;
  if ((currentR < r) == shrink) {
    toEdit = &m_x;
    newLength = m_y.length() / r;
  } else {
    toEdit = &m_y;
    newLength = m_x.length() * r;
  }
  assert((shrink && newLength <= toEdit->length()) ||
         (!shrink && newLength >= toEdit->length()));
  if (newLength < Range1D::k_minLength) {
    assert(shrink);
    return setRatio(r, false, limit);
  }
  float c = toEdit->center();
  newLength *= 0.5f;
  if (c == toEdit->min() || c == toEdit->max() ||
      c - newLength == c + newLength || c - newLength < -limit ||
      c + newLength > limit) {
    // Precision is to small for the edited range or limits are overstepped
    return false;
  }
  *toEdit = Range1D(c - newLength, c + newLength);
  assert(std::isnan(toEdit->length()) ||
         toEdit->length() >= Range1D::k_minLength);
  return true;
}

}  // namespace Poincare
