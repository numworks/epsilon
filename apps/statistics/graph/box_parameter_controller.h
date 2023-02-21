#ifndef STATISTICS_BOX_PARAMETER_CONTROLLER_H
#define STATISTICS_BOX_PARAMETER_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/message_table_cell_with_switch.h>
#include <escher/selectable_list_view_controller.h>

#include "../store.h"

namespace Statistics {

constexpr static int k_numberOfParameterCells = 1;

class BoxParameterController
    : public Escher::SelectableCellListPage<Escher::MessageTableCellWithSwitch,
                                            k_numberOfParameterCells,
                                            Escher::RegularListViewDataSource> {
 public:
  BoxParameterController(Escher::Responder* parentResponder, Store* store,
                         int* selectedBoxCalculation);
  void viewWillAppear() override;
  const char* title() override {
    return I18n::translate(I18n::Message::StatisticsGraphSettings);
  }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

 private:
  bool handleEvent(Ion::Events::Event event) override;
  Store* m_store;
  int* m_selectedBoxCalculation;
};

}  // namespace Statistics

#endif
