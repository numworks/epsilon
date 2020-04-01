#ifndef SHARED_LABELED_CURVE_VIEW_H
#define SHARED_LABELED_CURVE_VIEW_H

#include "curve_view.h"

/* This CurveView subclass provides label storage for common use cases */

namespace Shared {

class HorizontallyLabeledCurveView : public CurveView {
public:
  using CurveView::CurveView;
private:
  char * label(Axis axis, int index) const override;
  mutable char m_xLabels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
};

class VerticallyLabeledCurveView : public CurveView {
public:
  using CurveView::CurveView;
private:
  char * label(Axis axis, int index) const override;
  mutable char m_yLabels[k_maxNumberOfYLabels][k_labelBufferMaxSize];
};

class LabeledCurveView : public CurveView {
public:
  using CurveView::CurveView;
private:
  char * label(Axis axis, int index) const override;
  mutable char m_xLabels[k_maxNumberOfXLabels][k_labelBufferMaxSize];
  mutable char m_yLabels[k_maxNumberOfYLabels][k_labelBufferMaxSize];
};

}

#endif
