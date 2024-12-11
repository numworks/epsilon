#ifndef STATISTICS_BOX_PARAMETER_CONTROLLER_H
#define STATISTICS_BOX_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/menu_cell.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/switch_view.h>

#include "../store.h"
#include "data_view_controller.h"

namespace Statistics {

class BoxParameterController
    : public Escher::UniformSelectableListController<
          Escher::MenuCell<Escher::MessageTextView, Escher::EmptyCellWidget,
                           Escher::SwitchView>,
          1> {
 public:
  BoxParameterController(Escher::Responder* parentResponder, Store* store,
                         DataViewController* dataViewController);
  void viewWillAppear() override;
  const char* title() const override {
    return I18n::translate(I18n::Message::StatisticsGraphSettings);
  }

 private:
  bool handleEvent(Ion::Events::Event event) override;
  void updateDisplayOutliersSwitch() {
    cell(0)->accessory()->setState(m_store->displayOutliers());
  }
  Store* m_store;
  DataViewController* m_dataViewController;
};

}  // namespace Statistics

#endif
