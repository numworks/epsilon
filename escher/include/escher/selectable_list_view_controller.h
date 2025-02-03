#ifndef ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H
#define ESCHER_SELECTABLE_LIST_VIEW_CONTROLLER_H

#include <escher/explicit_list_view_data_source.h>
#include <escher/highlight_cell.h>
#include <escher/list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_list_view.h>
#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/view_controller.h>

#include <type_traits>

namespace Escher {

class SelectableViewController : public ViewController,
                                 public SelectableListViewDataSource {
 public:
  SelectableViewController(Responder* parentResponder)
      : ViewController(parentResponder) {}

 protected:
  void centerTable(KDCoordinate availableHeight);
};

template <typename DataSource>
class SelectableListViewController : public SelectableViewController,
                                     public DataSource {
 public:
  SelectableListViewController(
      Responder* parentResponder,
      SelectableListViewDelegate* listDelegate = nullptr)
      : SelectableViewController(parentResponder),
        m_selectableListView(this, this, this, listDelegate) {}
  /* ViewController */
  View* view() override { return &m_selectableListView; }
  SelectableListView* selectableListView() { return &m_selectableListView; }

 protected:
  SelectableListView m_selectableListView;
  void handleResponderChainEvent(
      Responder::ResponderChainEvent event) override {
    if (event.type == ResponderChainEventType::HasBecomeFirst) {
      App::app()->setFirstResponder(&m_selectableListView);
    } else {
      SelectableViewController::handleResponderChainEvent(event);
    }
  }
};

class ExplicitSelectableListViewController
    : public SelectableListViewController<ExplicitListViewDataSource> {
 protected:
  using SelectableListViewController::SelectableListViewController;
  HighlightCell* selectedCell() { return cell(selectedRow()); }
};

template <typename Cell, int NumberOfCells>
class UniformSelectableListController
    : public ExplicitSelectableListViewController {
 public:
  constexpr static int k_numberOfCells = NumberOfCells;

  UniformSelectableListController(
      Responder* parent, SelectableListViewDelegate* tableDelegate = nullptr)
      : ExplicitSelectableListViewController(parent, tableDelegate) {}
  int numberOfRows() const override final { return NumberOfCells; }
  Cell* cell(int row) override final {
    assert(0 <= row && row < NumberOfCells);
    return &m_cells[row];
  }

 private:
  Cell m_cells[NumberOfCells];
};

}  // namespace Escher

#endif
