#include "scrollable_output_expressions_view.h"
#include <assert.h>
using namespace Poincare;

namespace Calculation {

ScrollableOutputExpressionsView::ScrollableOutputExpressionsView(Responder * parentResponder) :
  ScrollableView(parentResponder, &m_outputView, this),
  m_outputView(this)
{
}

OutputExpressionsView * ScrollableOutputExpressionsView::outputView() {
  return &m_outputView;
}

void ScrollableOutputExpressionsView::didBecomeFirstResponder() {
  app()->setFirstResponder(&m_outputView);
}

KDSize ScrollableOutputExpressionsView::minimalSizeForOptimalDisplay() const {
  return m_outputView.minimalSizeForOptimalDisplay();
}

KDPoint ScrollableOutputExpressionsView::manualScrollingOffset() const {
  return m_manualScrollingOffset;
}

}
