#include "plot_store.h"
#include <algorithm>

namespace Matplotlib {

PlotStore::PlotStore() : Shared::InteractiveCurveViewRange(),
  m_show(false)
{
  flush();
}

void PlotStore::flush() {
  m_dots = mp_obj_new_list(0, nullptr);
  m_segments = mp_obj_new_list(0, nullptr);
  m_rects = mp_obj_new_list(0, nullptr);
  m_labels = mp_obj_new_list(0, nullptr);
  m_axesRequested = true;
  m_axesAuto = true;
  m_gridRequested = false;
}

// Iterators

template <class T>
PlotStore::ListIterator<T> PlotStore::ListIterator<T>::Begin(mp_obj_t list) {
  ListIterator<T> it;
  mp_obj_list_get(list, &(it.m_numberOfTuples), &(it.m_tuples));
  return it;
}

template <class T>
PlotStore::ListIterator<T> PlotStore::ListIterator<T>::End(mp_obj_t list) {
  ListIterator<T> it;
  mp_obj_list_get(list, &(it.m_numberOfTuples), &(it.m_tuples));
  if (it.m_numberOfTuples > 0) {
    it.m_tupleIndex = it.m_numberOfTuples;
  }
  return it;
}

template <class T>
PlotStore::ListIterator<T> & PlotStore::ListIterator<T>::operator++() {
  if (m_tupleIndex < m_numberOfTuples) {
    m_tupleIndex++;
  }
  return *this;
}

template <class T>
bool PlotStore::ListIterator<T>::operator!=(const PlotStore::ListIterator<T> & it) const {
  return m_tupleIndex != it.m_tupleIndex;
};

template <class T>
T PlotStore::ListIterator<T>::operator*() {
  return T(m_tuples[m_tupleIndex]);
};


// Dot

template class PlotStore::ListIterator<PlotStore::Dot>;

PlotStore::Dot::Dot(mp_obj_t tuple) {
  mp_obj_t * elements;
  mp_obj_get_array_fixed_n(tuple, 3, &elements);
  m_x = mp_obj_get_float(elements[0]);
  m_y = mp_obj_get_float(elements[1]);
  m_color = KDColor::RGB16(mp_obj_get_int(elements[2]));
}

void PlotStore::addDot(mp_obj_t x, mp_obj_t y, KDColor c) {
  mp_obj_t color = mp_obj_new_int(c);
  mp_obj_t items[3] = {x, y, color};
  mp_obj_t tuple = mp_obj_new_tuple(3, items);
  mp_obj_list_append(m_dots, tuple);
}

// Segment

template class PlotStore::ListIterator<PlotStore::Segment>;

PlotStore::Segment::Segment(mp_obj_t tuple) {
  mp_obj_t * elements;
  mp_obj_get_array_fixed_n(tuple, 6 , &elements);
  m_xStart = mp_obj_get_float(elements[0]);
  m_yStart = mp_obj_get_float(elements[1]);
  m_xEnd = mp_obj_get_float(elements[2]);
  m_yEnd = mp_obj_get_float(elements[3]);
  m_arrowWidth = mp_obj_get_float(elements[4]);
  m_color = KDColor::RGB16(mp_obj_get_int(elements[5]));
}

void PlotStore::addSegment(mp_obj_t xStart, mp_obj_t yStart, mp_obj_t xEnd, mp_obj_t yEnd, KDColor c, mp_obj_t arrowWidth) {
  mp_obj_t color = mp_obj_new_int(c);
  mp_obj_t items[6] = {xStart, yStart, xEnd, yEnd, arrowWidth, color};
  mp_obj_t tuple = mp_obj_new_tuple(6, items);
  mp_obj_list_append(m_segments, tuple);
}

// Rect

template class PlotStore::ListIterator<PlotStore::Rect>;

PlotStore::Rect::Rect(mp_obj_t tuple) {
  mp_obj_t * elements;
  mp_obj_get_array_fixed_n(tuple, 5, &elements);
  m_left = mp_obj_get_float(elements[0]);
  m_right = mp_obj_get_float(elements[1]);
  m_top = mp_obj_get_float(elements[2]);
  m_bottom = mp_obj_get_float(elements[3]);
  m_color = KDColor::RGB16(mp_obj_get_int(elements[4]));
}

void PlotStore::addRect(mp_obj_t left, mp_obj_t right, mp_obj_t top, mp_obj_t bottom, KDColor c) {
  mp_obj_t color = mp_obj_new_int(c);
  mp_obj_t items[5] = {left, right, top, bottom, color};
  mp_obj_t tuple = mp_obj_new_tuple(5, items);
  mp_obj_list_append(m_rects, tuple);
}

// Label

template class PlotStore::ListIterator<PlotStore::Label>;

PlotStore::Label::Label(mp_obj_t tuple) {
  mp_obj_t * elements;
  mp_obj_get_array_fixed_n(tuple, 3, &elements);
  m_x = mp_obj_get_float(elements[0]);
  m_y = mp_obj_get_float(elements[1]);
  m_string = mp_obj_str_get_str(elements[2]);
}

void PlotStore::addLabel(mp_obj_t x, mp_obj_t y, mp_obj_t string) {
  mp_obj_t items[3] = {x, y, string};
  if (!mp_obj_is_str(string)) {
    mp_raise_TypeError("argument should be a string");
  }
  mp_obj_t tuple = mp_obj_new_tuple(3, items);
  mp_obj_list_append(m_labels, tuple);
}

// Axes

void updateRange(float * xMin, float * xMax, float * yMin, float * yMax, float x, float y) {
  if (!std::isnan(x) && !std::isinf(x) && !std::isnan(y) && !std::isinf(y)) {
    *xMin = std::min(*xMin, x);
    *xMax = std::max(*xMax, x);
    *yMin = std::min(*yMin, y);
    *yMax = std::max(*yMax, y);
  }
}

void checkPositiveRangeAndAddMargin(float * min, float * max) {
  if (*min > *max) {
    *min = - Poincare::Range1D::k_defaultHalfLength;
    *max = Poincare::Range1D::k_defaultHalfLength;
    return;
  }
  // Add margins
  float margin = (*max - *min)/10.0f;
  if (margin < FLT_EPSILON) {
    margin = 1.0f;
  }
  *min -= margin;
  *max += margin;
}

void PlotStore::initRange() {
  if (m_axesAuto) {
    float xMin = FLT_MAX;
    float xMax = -FLT_MAX;
    float yMin = FLT_MAX;
    float yMax = -FLT_MAX;
    for (PlotStore::Dot dot : dots()) {
      updateRange(&xMin, &xMax, &yMin, &yMax, dot.x(), dot.y());
    }
    for (PlotStore::Label label : labels()) {
      updateRange(&xMin, &xMax, &yMin, &yMax, label.x(), label.y());
    }
    for (PlotStore::Segment segment : segments()) {
      updateRange(&xMin, &xMax, &yMin, &yMax, segment.xStart(), segment.yStart());
      updateRange(&xMin, &xMax, &yMin, &yMax, segment.xEnd(), segment.yEnd());
    }
    for (PlotStore::Rect rectangle : rects()) {
      updateRange(&xMin, &xMax, &yMin, &yMax, rectangle.left(), rectangle.top());
      updateRange(&xMin, &xMax, &yMin, &yMax, rectangle.right(), rectangle.bottom());
    }
    checkPositiveRangeAndAddMargin(&xMin, &xMax);
    checkPositiveRangeAndAddMargin(&yMin, &yMax);
    setXMin(xMin);
    setXMax(xMax);
    setYMin(yMin);
    setYMax(yMax);
  }
}

}
