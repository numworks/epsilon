#ifndef CALCULATION_EDITABLE_EXPRESSION_VIEW_H
#define CALCULATION_EDITABLE_EXPRESSION_VIEW_H

#include <escher.h>

namespace Calculation {

class EditableExpressionView : public ::EditableExpressionView {
  using ::EditableExpressionView::EditableExpressionView;
protected:
  bool privateHandleEvent(Ion::Events::Event event) override;
};

}

#endif
