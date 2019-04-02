#ifndef GRAPH_PREIMAGE_PARAMETER_CONTROLLER
#define GRAPH_PREIMAGE_PARAMETER_CONTROLLER

#include "../../shared/go_to_parameter_controller.h"
#include "preimage_graph_controller.h"

namespace Graph {

class PreimageParameterController : public Shared::GoToParameterController {
public:
  PreimageParameterController(
    Responder * parentResponder,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor,
    PreimageGraphController * preimageGraphController
  );
  const char * title() override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  void viewWillAppear() override;
private:
  void buttonAction() override;
  double parameterAtIndex(int index) override;
  bool setParameterAtIndex(int parameterIndex, double f) override;
  Ion::Storage::Record m_record;
  PreimageGraphController * m_preimageGraphController;
};

}

#endif
