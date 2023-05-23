#ifndef ELEMENTS_DETAILS_LIST_CONTROLLER
#define ELEMENTS_DETAILS_LIST_CONTROLLER

#include <escher/layout_view.h>
#include <escher/menu_cell.h>
#include <escher/message_text_view.h>
#include <escher/selectable_list_view_with_top_and_bottom_views.h>
#include <escher/stack_view_controller.h>

#include "single_element_view.h"

namespace Elements {

class DetailsListController
    : public Escher::SelectableListViewWithTopAndBottomViews {
 public:
  DetailsListController(Escher::StackViewController *parentResponder);

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;

  // Escher::ViewController
  const char *title() override;

  // Escher::TableViewDataSource
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell *reusableCell(int index, int type) override {
    return m_cells + index;
  }
  int reusableCellCount(int type) override {
    return k_maxNumberOfDisplayedRows;
  }
  KDCoordinate separatorBeforeRow(int index) override;

  // Escher::ListViewDataSource
  int typeAtIndex(int index) const override { return 0; }
  void willDisplayCellForIndex(Escher::HighlightCell *cell, int index) override;

  bool canStoreContentOfCell(Escher::SelectableListView *l,
                             int row) const override;

 private:
  constexpr static size_t k_numberOfRows = 13;
  constexpr static size_t k_maxNumberOfDisplayedRows =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::StackTitleHeight);

  static const DataField *DataFieldForRow(int row);

  // Escher::MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;

  Escher::StackViewController *stackViewController() const {
    return static_cast<Escher::StackViewController *>(parentResponder());
  }

  using PhysicalQuantityCell =
      Escher::MenuCell<Escher::LayoutView, Escher::MessageTextView,
                       Escher::LayoutView>;

  SingleElementView m_topElementView;
  Escher::MessageTextView m_bottomMessageView;
  PhysicalQuantityCell m_cells[k_maxNumberOfDisplayedRows];
};

}  // namespace Elements

#endif
