#ifndef TEST_CONTROLLER_H
#define TEST_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/responder.h>

class TestController : public Escher::SelectableListViewController {
public:
  TestController(Escher::Responder * parentResponder);
  int numberOfRows() const override { return k_numberOfCells; }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void didBecomeFirstResponder() override;

private:
  constexpr static int k_numberOfCells = 5;
  constexpr static int k_indexOfOneProp = 0;
  constexpr static int k_indexOfOneMean = k_indexOfOneProp + 1;
  constexpr static int k_indexOfTwoProps = k_indexOfOneProp + 1;
  constexpr static int k_indexOfTwoMeans = k_indexOfTwoProps + 1;
  constexpr static int k_indexOfCategorical = k_indexOfTwoMeans + 1;
  Escher::MessageTableCellWithChevronAndMessage m_cells[k_numberOfCells];
};

#endif /* TEST_CONTROLLER_H */
