#ifndef SHARED_GO_TO_PARAMETER_CONTROLLER_H
#define SHARED_GO_TO_PARAMETER_CONTROLLER_H

#include <escher.h>
#include "float_parameter_controller.h"
#include "curve_view_cursor.h"
#include "interactive_curve_view_range.h"

namespace Shared {

class GoToParameterController : public FloatParameterController<double> {
public:
  GoToParameterController(Responder * parentResponder, InputEventHandlerDelegate * inputEventHandlerDelegate, InteractiveCurveViewRange * graphRange, CurveViewCursor * cursor);
  int numberOfRows() const override { return 2; }
  bool handleEvent(Ion::Events::Event event) override;
protected:
  void setParameterName(I18n::Message message) { m_parameterCell.setMessage(message); }
  void viewWillAppear() override;
  // extractParameterAtIndex extracts the current value of the parameter
  virtual double extractParameterAtIndex(int index) = 0;
  // confirmParameterAtIndex updates the current value of the parameter
  virtual bool confirmParameterAtIndex(int parameterIndex, double f) = 0;
  // parameterAtIndex and setParameterAtIndex manipulate m_tempParameter only
  double parameterAtIndex(int index) override {
    assert(index == 0);
    return m_tempParameter;
  }
  bool setParameterAtIndex(int parameterIndex, double f) override;
  CurveViewCursor * m_cursor;
  InteractiveCurveViewRange * m_graphRange;
  double m_tempParameter;
private:
  void buttonAction() override;
  HighlightCell * reusableParameterCell(int index, int type) override;
  int reusableParameterCellCount(int type) override { return 1; }
  MessageTableCellWithEditableText m_parameterCell;
};

}

#endif
