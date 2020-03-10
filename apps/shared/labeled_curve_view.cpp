#include "labeled_curve_view.h"

namespace Shared {

char * HorizontallyLabeledCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Horizontal) {
    assert(index < k_maxNumberOfXLabels);
    return m_xLabels[index];
  }
  return nullptr;
}

char * VerticallyLabeledCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Vertical) {
    assert(index < k_maxNumberOfYLabels);
    return m_yLabels[index];
  }
  return nullptr;
}

char * LabeledCurveView::label(Axis axis, int index) const {
  if (axis == Axis::Horizontal) {
    assert(index < k_maxNumberOfXLabels);
    return m_xLabels[index];
  } else {
    assert(index < k_maxNumberOfYLabels);
    return m_yLabels[index];
  }
}

}
