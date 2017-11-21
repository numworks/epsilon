#ifndef CALCULATION_SCROLLABLE_OUTPUT_EXPRESSIONS_VIEW_H
#define CALCULATION_SCROLLABLE_OUTPUT_EXPRESSIONS_VIEW_H

#include <escher.h>
#include "output_expressions_view.h"

namespace Calculation {

class ScrollableOutputExpressionsView : public ScrollableView, public ScrollViewDataSource {
public:
  ScrollableOutputExpressionsView(Responder * parentResponder);
  OutputExpressionsView * outputView();
  void didBecomeFirstResponder() override;
  KDSize minimalSizeForOptimalDisplay() const override;
  KDPoint manualScrollingOffset() const;
private:
  OutputExpressionsView m_outputView;
};

}

#endif
