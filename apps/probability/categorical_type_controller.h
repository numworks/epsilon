#ifndef CATEGORICAL_TYPE_CONTROLLER_H
#define CATEGORICAL_TYPE_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/responder.h>
#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>

class CategoricalTypeController : public Escher::SelectableListViewController {
public:
  CategoricalTypeController(Escher::Responder * parent);
  Escher::HighlightCell * reusableCell(int i, int type) override;
  int numberOfRows() const override {return k_numberOfCells; }

private:
  constexpr static int k_numberOfCells = 2;
  constexpr static int k_indexOfGoodness    = 0;
  constexpr static int k_indexOfHomogeneity = 1;
  Escher::MessageTableCellWithChevron m_cells[k_numberOfCells];

};

#endif /* CATEGORICAL_TYPE_CONTROLLER_H */
