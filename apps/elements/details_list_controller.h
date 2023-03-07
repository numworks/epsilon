#ifndef ELEMENTS_DETAILS_LIST_CONTROLLER
#define ELEMENTS_DETAILS_LIST_CONTROLLER

#include <escher/list_view_with_top_and_bottom_views.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "physical_quantity_cell.h"
#include "single_element_view.h"

namespace Elements {

class DetailsListController : public Escher::ViewController,
                              public Escher::MemoizedListViewDataSource,
                              public Escher::SelectableListViewDataSource {
 public:
  DetailsListController(Escher::StackViewController *parentResponder);

  // Escher::Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;

  // Escher::ViewController
  const char *title() override;
  Escher::View *view() override { return &m_view; }

  // Escher::TableViewDataSource
  int numberOfRows() const override { return k_numberOfRows; }
  Escher::HighlightCell *reusableCell(int index, int type) override;
  int reusableCellCount(int type) override;

  // Escher::ListViewDataSource
  int typeAtIndex(int index) const override;
  void willDisplayCellForIndex(Escher::HighlightCell *cell, int index) override;

 private:
  constexpr static int k_normalCellType = 0;
  constexpr static int k_separatorCellType = 1;
  constexpr static size_t k_numberOfRows = 13;
  constexpr static size_t k_numberOfNormalCells = 4;
  constexpr static size_t k_numberOfSeparatorCells = 4;

  static const DataField *DataFieldForRow(int row);

  // Escher::MemoizedListViewDataSource
  KDCoordinate nonMemoizedRowHeight(int j) override;

  Escher::StackViewController *stackViewController() const {
    return static_cast<Escher::StackViewController *>(parentResponder());
  }

  Escher::SelectableListView m_selectableListView;
  SingleElementView m_topElementView;
  Escher::MessageTextView m_bottomMessageView;
  Escher::ListViewWithTopAndBottomViews m_view;
  PhysicalQuantityCell m_normalCells[k_numberOfNormalCells];
  PhysicalQuantityCellWithSeparator m_separatorCells[k_numberOfSeparatorCells];
};

}  // namespace Elements

#endif
