#ifndef TYPE_CONTROLLER_H
#define TYPE_CONTROLLER_H

#include <escher/container.h>
#include <escher/highlight_cell.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/message_table_cell.h>
#include <escher/message_table_cell_with_chevron_and_message.h>
#include <escher/message_text_view.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/view_controller.h>
#include <ion/events.h>

#include "../gui/vertical_layout.h"
#include "hypothesis_controller.h"

using namespace Escher;

/* Simple view to include list and description below */
class TypeView : public VerticalLayout<2> {
 public:
  TypeView(SelectableTableView * list, MessageTextView * description) : m_list(list), m_description(description) {
    setView(m_list, 0);
    setView(m_description, 1);
  }

 private:
  SelectableTableView * m_list;
  MessageTextView * m_description;
};

class TypeController : public SelectableListViewController {
 public:
  TypeController(Responder * parent, HypothesisController * hypothesisController);
  View * view() override { return &m_contentView; }
  void didBecomeFirstResponder() override {
    m_selectableTableView.selectRow(0);
    Container::activeApp()->setFirstResponder(&m_selectableTableView);
  }
  // ListViewDataSource
  int numberOfRows() const override { return k_numberOfRows; };
  HighlightCell * reusableCell(int i, int type) override;

 private:
  constexpr static int k_numberOfRows = 3;
  constexpr static int k_indexOfTTest = 0;
  constexpr static int k_indexOfPooledTest = 1;
  constexpr static int k_indexOfZTest = 2;
  constexpr static int k_indexOfDescription = 3;

  MessageTableCellWithChevronAndMessage m_cells[k_numberOfRows];

  HypothesisController * m_hypothesisController;

  TypeView m_contentView;
  MessageTextView m_description;
};

#endif /* TYPE_CONTROLLER_H */
