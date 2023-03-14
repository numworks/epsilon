#ifndef INFERENCE_STATISTIC_TYPE_CONTROLLER_H
#define INFERENCE_STATISTIC_TYPE_CONTROLLER_H

#include <escher/highlight_cell.h>
#include <escher/message_table_cell_with_chevron.h>
#include <escher/message_text_view.h>
#include <escher/selectable_table_view.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include <ion/events.h>

#include "input_controller.h"
#include "test/hypothesis_controller.h"

namespace Inference {

class TypeController : public Escher::SelectableListViewController<
                           Escher::MemoizedListViewDataSource> {
 public:
  TypeController(Escher::StackViewController* parent,
                 HypothesisController* hypothesisController,
                 InputController* intervalInputController,
                 Statistic* statistic);
  Escher::View* view() override { return &m_selectableListView; }
  const char* title() override;
  ViewController::TitlesDisplay titlesDisplay() override {
    return ViewController::TitlesDisplay::DisplayLastTitle;
  }
  void stackOpenPage(Escher::ViewController* nextPage) override;
  void didBecomeFirstResponder() override;
  // ListViewDataSource
  int numberOfRows() const override;
  Escher::HighlightCell* reusableCell(int i, int type) override {
    return &m_cells[i];
  }
  bool handleEvent(Ion::Events::Event event) override;
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int i) override;

  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;

 private:
  constexpr static int k_numberOfRows = 3;

  int indexOfZTest() const { return numberOfRows() - 1; }

  HypothesisController* m_hypothesisController;
  InputController* m_inputController;

  Escher::MessageTableCellWithChevron m_cells[k_numberOfRows];

  constexpr static int k_titleBufferSize =
      sizeof("intervalle pour une moyenne à deux échantillons");
  char m_titleBuffer[k_titleBufferSize];

  Statistic* m_statistic;
};

}  // namespace Inference

#endif
