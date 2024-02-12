#include <poincare/range.h>

namespace PoincareJ {

// Range1D

template <typename T>
Range1D<T> Range1D<T>::ValidRangeBetween(T a, T b, T limit) {
  assert(!std::isnan(a) && !std::isnan(b));
  T min = (a < b ? a : b);
  T max = (a < b ? b : a);
  min = std::clamp(min, -limit, limit);
  max = std::clamp(max, -limit, limit);
  Range1D<T> res(min, max, limit);
  res.stretchIfTooSmall(-1.f, limit);

  assert(res.isValid());
  assert(-limit <= res.min() && res.min() <= limit);
  assert(-limit <= res.max() && res.max() <= limit);

  return res;
}

template <typename T>
void Range1D<T>::extend(T t, T limit) {
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

template <typename T>
void Range1D<T>::zoom(T ratio, T center) {
  m_min = (m_min - center) * ratio + center;
  m_max = (m_max - center) * ratio + center;
}

template <typename T>
void Range1D<T>::stretchEachBoundBy(T shift, T limit) {
  assert(shift >= k_zero);
  m_min = std::max(m_min - shift, -limit);
  m_max = std::min(m_max + shift, limit);
  assert(std::isnan(length()) || length() >= k_minLength);
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

template <typename T>
void Range1D<T>::stretchIfTooSmall(T shift, T limit) {
  /* Handle cases where limits are too close or equal.
   * They are both shifted by shift. */
  if (length() < k_minLength) {
    shift = std::max(shift, DefaultLengthAt(m_min) / static_cast<T>(2.));
    stretchEachBoundBy(shift, limit);
  }
  assert(std::isnan(length()) || length() >= k_minLength);
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

template <typename T>
void Range1D<T>::privateSetKeepingValid(T t, bool isMin, T limit) {
  assert(isValid());
  assert(!std::isnan(t));
  T* bound = isMin ? &m_min : &m_max;
  T* otherBound = isMin ? &m_max : &m_min;
  assert(limit > k_zero);
  *bound = std::clamp(t, -limit, limit);
  if (length() < k_zero) {
    *otherBound = *bound;
  }
  assert(length() >= k_zero);
  if (length() < k_minLength) {
    /* If the new bound is too close from the other one, set the other one to a
     * distance DefaultLength.
     * We don't call stretchIfTooSmall yet since we want to avoid to modify the
     * bound that is being set.  */
    T l = DefaultLengthAt(m_min) * (isMin ? 1 : -1);
    *otherBound = std::clamp(*bound + l, -limit, limit);

    if (length() < k_minLength) {
      /* The bounds are still too close (the other bound is at the limit). This
       * means the bound can't be set to t and needs to be shifted a bit. */
      stretchIfTooSmall(static_cast<T>(-1.), limit);
    }
  }
  assert(isValid());
  assert(-limit <= m_min && m_min <= limit);
  assert(-limit <= m_max && m_max <= limit);
}

// Range2D
template <typename T>
int Range2D<T>::NormalizationSignificantBits(T xMin, T xMax, T yMin, T yMax) {
  T xr = std::fabs(xMin) > std::fabs(xMax) ? xMax / xMin : xMin / xMax;
  T yr = std::fabs(yMin) > std::fabs(yMax) ? yMax / yMin : yMin / yMax;
  /* The subtraction x - y induces a loss of significance of -log2(1-x/y)
   * bits. Since normalizing requires computing xMax - xMin and yMax - yMin,
   * the ratio of the normalized range will deviate from the normal ratio. We
   * add an extra two lost bits to account for loss of precision from other
   * sources. */
  T loss = std::log2(std::min(1.f - xr, 1.f - yr));
  if (loss > 0.f) {
    loss = 0.f;
  }
  return std::max(static_cast<int>(std::floor(loss + 23.f - 2.f)), 0);
}

template <typename T>
bool Range2D<T>::ratioIs(T r) const {
  int significantBits =
      NormalizationSignificantBits(xMin(), xMax(), yMin(), yMax());
  T thisRatio = ratio();
  /* The last N (= 23 - significantBits) bits of "ratio" mantissa have become
   * insignificant. "tolerance" is the difference between ratio with those N
   * bits set to 1, and ratio with those N bits set to 0 ; i.e. a measure of
   * the interval in which numbers are indistinguishable from ratio with this
   * level of precision. */
  T tolerance =
      std::pow(static_cast<T>(2.),
               OMG::IEEE754<T>::exponent(thisRatio) - significantBits);
  return std::fabs(thisRatio - r) <= tolerance;
}

template <typename T>
bool Range2D<T>::setRatio(T r, bool shrink, T limit) {
  T currentR = ratio();
  Range1D<T>* toEdit;
  T newLength;
  if ((currentR < r) == shrink) {
    toEdit = &m_x;
    newLength = m_y.length() / r;
  } else {
    toEdit = &m_y;
    newLength = m_x.length() * r;
  }
  assert((shrink && newLength <= toEdit->length()) ||
         (!shrink && newLength >= toEdit->length()));
  if (newLength < Range1D<T>::k_minLength) {
    assert(shrink);
    return setRatio(r, false, limit);
  }
  T c = toEdit->center();
  newLength *= static_cast<T>(0.5);
  if (c == toEdit->min() || c == toEdit->max() ||
      c - newLength == c + newLength || c - newLength < -limit ||
      c + newLength > limit) {
    // Precision is to small for the edited range or limits are overstepped
    return false;
  }
  *toEdit = Range1D<T>(c - newLength, c + newLength);
  assert(std::isnan(toEdit->length()) ||
         toEdit->length() >= Range1D<T>::k_minLength);
  return true;
}

template class Range1D<float>;
template class Range1D<double>;
template class Range2D<float>;
template class Range2D<double>;

}  // namespace PoincareJ
