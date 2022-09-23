#ifndef GRAPH_PREIMAGE_PARAMETER_CONTROLLER
#define GRAPH_PREIMAGE_PARAMETER_CONTROLLER

#include "../../shared/go_to_parameter_controller.h"
#include "preimage_graph_controller.h"

namespace Graph {

class PreimageParameterController : public Shared::GoToParameterController {
public:
  PreimageParameterController(
    Escher::Responder * parentResponder,
    Escher::InputEventHandlerDelegate * inputEventHandlerDelegate,
    Shared::InteractiveCurveViewRange * graphRange,
    Shared::CurveViewCursor * cursor,
    PreimageGraphController * preimageGraphController
  );
  const char * title() override { return I18n::translate(I18n::Message::Preimage); }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  void viewWillAppear() override;
  TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayLastThreeTitles; }
private:
  void buttonAction() override;
  double extractParameterAtIndex(int index) override {
    assert(index == 0);
    return m_preimageGraphController->image();
  }
  bool confirmParameterAtIndex(int parameterIndex, double f) override;
  Ion::Storage::Record m_record;
  PreimageGraphController * m_preimageGraphController;
};

}

#endif
