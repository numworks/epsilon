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
          4> {
 public:
  DerivativesParameterController(Escher::Responder* parentResponder);

  const char* title() const override {
    return I18n::translate(I18n::Message::GraphDerivatives);
  }
  TitlesDisplay titlesDisplay() const override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void viewWillAppear() override;
  void setRecord(Ion::Storage::Record record);

 private:
  Escher::StackViewController* stackController() const;
  Shared::ExpiringPointer<Shared::ContinuousFunction> function() const;
  void updateSwitchs();
  void toggleSwitch(int row);
  bool switchState(int row) const;

  constexpr static int k_indexOfFirstDerivativeValue = 0;
  constexpr static int k_indexOfFirstDerivativePlot = 1;
  constexpr static int k_indexOfSecondDerivativeValue = 2;
  constexpr static int k_indexOfSecondDerivativePlot = 3;

  Ion::Storage::Record m_record;
};

}  // namespace Graph

#endif
