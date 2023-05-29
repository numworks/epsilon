#ifndef GRAPH_SHARED_DOMAIN_PARAMETER_CONTROLLER_H
#define GRAPH_SHARED_DOMAIN_PARAMETER_CONTROLLER_H

#include <apps/graph/shared/function_toolbox.h>
#include <apps/shared/continuous_function.h>
#include <apps/shared/expiring_pointer.h>
#include <apps/shared/input_event_handler_delegate.h>
#include <apps/shared/interactive_curve_view_controller.h>
#include <apps/shared/single_range_controller.h>

namespace Graph {

class DomainParameterController : public Shared::SingleRangeController,
                                  public Shared::InputEventHandlerDelegate {
 public:
  DomainParameterController(Escher::Responder* parentResponder);

  // ViewController
  const char* title() override {
    return I18n::translate(I18n::Message::FunctionDomain);
  }
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }
  TELEMETRY_ID("DomainParameter");

  // TextFieldDelegate
  bool textFieldDidReceiveEvent(Escher::AbstractTextField* textField,
                                Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(Escher::AbstractTextField* textField,
                                 const char* text,
                                 Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(Escher::AbstractTextField* textField) override;

  void setRecord(Ion::Storage::Record record) { m_record = record; }
  bool isVisible() const {
    return m_record.isNull() ? false
                             : function()->properties().canHaveCustomDomain();
  }

  // InputEventHandlerDelegate
  FunctionToolbox* toolbox() override;

 private:
  I18n::Message parameterMessage(int index) const override;
  // Return false if temporary parameters and function parameters are equal.
  bool parametersAreDifferent() override;
  // Extracts parameters from function, setting m_tempDomain parameters.
  void extractParameters() override;
  void setAutoRange() override;
  float limit() const override { return INFINITY; }
  // Applies temporary parameters to function.
  void confirmParameters() override;
  void pop(bool onConfirmation) override {
    onConfirmation ? stackController()->popUntilDepth(
                         Shared::InteractiveCurveViewController::
                             k_graphControllerStackDepth,
                         true)
                   : stackController()->pop();
  }
  InfinityTolerance infinityAllowanceForRow(int row) const override;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  void switchToolboxContent(Escher::AbstractTextField* textField,
                            bool setSpecificContent);

  Shared::MessagePopUpController m_confirmPopUpController;
  Ion::Storage::Record m_record;
};

}  // namespace Graph

#endif
