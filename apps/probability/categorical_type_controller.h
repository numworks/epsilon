#ifndef CATEGORICAL_TYPE_CONTROLLER_H
#define CATEGORICAL_TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/responder.h>

#include "gui/selectable_cell_list_controller.h"

constexpr static int k_numberOfCategoricalCells = 2;

class CategoricalTypeController
    : public SelectableCellListController<Escher::MessageTableCellWithChevron, k_numberOfCategoricalCells> {
 public:
  CategoricalTypeController(Escher::Responder * parent);
  void didBecomeFirstResponder() override {
    // TODO factor out
    if (selectedRow() == -1) {
      selectCellAtLocation(0, 0);
    } else {
      selectCellAtLocation(selectedColumn(), selectedRow());
    }
    Escher::Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }

 private:
  constexpr static int k_indexOfGoodness = 0;
  constexpr static int k_indexOfHomogeneity = 1;
};

#endif /* CATEGORICAL_TYPE_CONTROLLER_H */
