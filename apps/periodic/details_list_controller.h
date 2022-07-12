#ifndef PERIODIC_DETAILS_LIST_CONTROLLER
#define PERIODIC_DETAILS_LIST_CONTROLLER

#include "single_element_view.h"
#include "table_cells.h"
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>
#include <escher/table_view_with_top_and_bottom_views.h>

namespace Periodic {

class DetailsListController : public Escher::ViewController, public Escher::MemoizedListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  DetailsListController(Escher::StackViewController * parentResponder);

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // Escher::ViewController
  const char * title() override;
  Escher::View * view() override { return &m_view; }

  // Escher::TableViewDataSource
  int numberOfRows() const override { return static_cast<int>(Row::NumberOfRows); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;

  // Escher::ListViewDataSource
  int typeAtIndex(int index) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  // Escher::MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;

private:
  constexpr static int k_bufferCellType = 0;
  constexpr static int k_separatorBufferCellType = 1;
  constexpr static int k_layoutTitleCellType = 2;
  constexpr static int k_separatorLayoutCellType = 3;
  constexpr static size_t k_numberOfBufferCells = 4; // TODO Tune
  constexpr static size_t k_numberOfSeparatorBufferCells = 3; // TODO Tune
  constexpr static size_t k_numberOfLayoutTitleCells = 4;// TODO Tune
  constexpr static size_t k_numberOfSeparatorLayoutCells = 2; // TODO Tune

  enum class Row : int {
    Z = 0,
    A,
    Mass,
    Configuration,
    Electronegativity,
    Group,
    Radius,
    State,
    MeltingPoint,
    BoilingPoint,
    Density,
    Affinity,
    Ionisation,
    NumberOfRows
  };

  Escher::StackViewController * stackViewController() const { return static_cast<Escher::StackViewController *>(parentResponder()); }

  Escher::SelectableTableView m_selectableTableView;
  SingleElementView m_topElementView;
  Escher::MessageTextView m_bottomMessageView;
  Escher::TableViewWithTopAndBottomViews m_view;
  Escher::MessageTableCellWithMessageWithBuffer m_bufferCells[k_numberOfBufferCells];
  MessageTableCellWithMessageWithBufferWithSeparator m_separatorBufferCells[k_numberOfSeparatorBufferCells];
  InertExpressionTableCell m_layoutTitleCells[k_numberOfLayoutTitleCells];
  MessageTableCellWithMessageWithExpressionWithSeparator m_separatorLayoutCells[k_numberOfSeparatorLayoutCells];
};

}

#endif
