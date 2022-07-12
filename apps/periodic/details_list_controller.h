#ifndef PERIODIC_DETAILS_LIST_CONTROLLER
#define PERIODIC_DETAILS_LIST_CONTROLLER

#include "table_cells.h"
#include <escher/expression_table_cell_with_message_with_buffer.h>
#include <escher/selectable_list_view_controller.h>

namespace Periodic {

class DetailsListController : public Escher::SelectableListViewController<Escher::MemoizedListViewDataSource> {
public:
  DetailsListController(Escher::Responder * parentResponder) : Escher::SelectableListViewController<Escher::MemoizedListViewDataSource>(parentResponder) {}

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

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
  constexpr static size_t k_numberOfSeparatorLayoutCells = 1; // TODO Tune

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

  Escher::MessageTableCellWithMessageWithBuffer m_bufferCells[k_numberOfBufferCells];
  MessageTableCellWithMessageWithBufferWithSeparator m_separatorBufferCells[k_numberOfSeparatorBufferCells];
  Escher::ExpressionTableCellWithMessageWithBuffer m_layoutTitleCells[k_numberOfLayoutTitleCells];
  MessageTableCellWithMessageWithExpressionWithSeparator m_separatorLayoutCells[k_numberOfSeparatorLayoutCells];
};

}

#endif
