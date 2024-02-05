#ifndef GRAPH_SHARED_DERIVATIVES_PARAMETER_CONTROLLER_H
#define GRAPH_SHARED_DERIVATIVES_PARAMETER_CONTROLLER_H

#include <apps/shared/continuous_function.h>
#include <apps/shared/expiring_pointer.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/switch_view.h>

namespace Graph {

class DerivativesParameterController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::SwitchView>,
          1> {
 public:
  DerivativesParameterController(Escher::Responder* parentResponder);

  const char* title() override {
    return I18n::translate(I18n::Message::GraphDerivative);
  }
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 private:
  Escher::StackViewController* stackController() const;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  void updateSwitch();

  Ion::Storage::Record m_record;
};

}  // namespace Graph

#endif
