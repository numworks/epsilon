#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <ion/events.h>

#include "categorical_type_controller.h"
#include "hypothesis_controller.h"

class TestController : public Escher::SelectableListViewController {
 public:
  TestController(Escher::Responder * parentResponder, HypothesisController * hypothesisController,
                 CategoricalTypeController * categoricalController);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;

 private:
  constexpr static int k_numberOfCells = 5;
  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = 1;
  constexpr static int k_indexOfTwoProps = 2;
  constexpr static int k_indexOfTwoMeans = 3;
  constexpr static int k_indexOfCategorical = 4;
  Escher::MessageTableCellWithChevronAndMessage m_cells[k_numberOfCells];

  HypothesisController * m_hypothesisController;
  CategoricalTypeController * m_categoricalController;
};

#endif /* TEST_CONTROLLER_H */
