#pragma once

#include <apps/shared/go_to_parameter_controller.h>

#include "preimage_graph_controller.h"

namespace Graph {

class PreimageParameterController : public Shared::GoToParameterController {
 public:
  PreimageParameterController(Escher::Responder* parentResponder,
                              Shared::InteractiveCurveViewRange* graphRange,
                              Shared::CurveViewCursor* cursor,
                              PreimageGraphController* preimageGraphController);
  const char* title() const override {
    return I18n::translate(I18n::Message::Preimage);
  }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
  void viewWillAppear() override;
  TitlesDisplay titlesDisplay() const override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }

 private:
  void buttonAction() override;
  double extractParameterAtIndex(int index) override {
    assert(index == 0);
    return m_preimageGraphController->image();
  }
  bool confirmParameterAtIndex(int parameterIndex, double f) override;
  Ion::Storage::Record m_record;
  PreimageGraphController* m_preimageGraphController;
};

}  // namespace Graph
