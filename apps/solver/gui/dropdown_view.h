#ifndef SOLVER_GUI_DROPDOWN_VIEW_H
#define SOLVER_GUI_DROPDOWN_VIEW_H

#include <escher/bordered.h>
#include <escher/image_view.h>
#include <escher/list_view_data_source.h>
#include <escher/memoized_list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/view.h>
#include <ion/events.h>

#include "bordering_view.h"
#include "highlight_image_cell.h"

namespace Solver {

// TODO: refactor

/* Wraps a HighlightCell to add margins and an optional caret. */
class PopupItemView : public Escher::HighlightCell, public Escher::Bordered {
public:
  PopupItemView(Escher::HighlightCell * cell = nullptr);
  void setHighlighted(bool highlighted) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override;
  Escher::View * subviewAtIndex(int i) override;
  Escher::HighlightCell * innerCell() { return m_cell; }
  void setInnerCell(Escher::HighlightCell * cell) { m_cell = cell; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setPopping(bool popping) { m_isPoppingUp = popping; }

private:
  constexpr static int k_marginCaretRight = 2;
  constexpr static int k_marginImageHorizontal = 3;
  constexpr static int k_marginImageVertical = 2;
  Escher::HighlightCell * m_cell;
  HighlightImageCell m_caret;
  bool m_isPoppingUp;
};

class DropdownCallback {
public:
  virtual void onDropdownSelected(int selectedRow) = 0;
  virtual bool popupDidReceiveEvent(Ion::Events::Event event) { return false; }
};

/* A Dropdown is a view that, when clicked on, displays a list of views to choose from
 * It requires a DropdownDataSource to provide a list of views */
class Dropdown : public PopupItemView, public Escher::Responder {
public:
  Dropdown(Escher::Responder * parentResponder,
           Escher::ListViewDataSource * listDataSource,
           DropdownCallback * callback = nullptr);
  Escher::Responder * responder() override { return this; }
  bool handleEvent(Ion::Events::Event e) override;
  void registerCallback(DropdownCallback * callback) { m_popup.registerCallback(callback); }
  void reloadAllCells();
  void init();
  int selectedRow() { return m_popup.selectedRow(); }
  void selectRow(int row) { m_popup.selectRow(row); }

  void open();
  void close();

private:
  /* List of PopupViews shown in a modal view + Wraps a ListViewDataSource to return PopupViews. */

  class DropdownPopupController : public Escher::ViewController, public Escher::MemoizedListViewDataSource {
  public:
    friend Dropdown;
    DropdownPopupController(Escher::Responder * parentResponder,
                            Escher::ListViewDataSource * listDataSource,
                            Dropdown * dropdown,
                            DropdownCallback * callback = nullptr);

    // View Controller
    Escher::View * view() override { return &m_borderingView; }
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event e) override;
    void registerCallback(DropdownCallback * callback) { m_callback = callback; }
    int selectedRow() { return m_selectionDataSource.selectedRow(); }
    void selectRow(int row);
    void close();

    // MemoizedListViewDataSource
    int numberOfRows() const override { return m_listViewDataSource->numberOfRows(); }
    KDCoordinate cellWidth() override;
    int typeAtIndex(int index) override { return m_listViewDataSource->typeAtIndex(index); }
    KDCoordinate nonMemoizedRowHeight(int j) override;
    int reusableCellCount(int type) override {
      return m_listViewDataSource->reusableCellCount(type);
    }
    PopupItemView * reusableCell(int index, int type) override;
    void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
    void resetMemoization(bool force = true) override;
    Escher::HighlightCell * innerCellAtIndex(int index);

    constexpr static int k_maxNumberOfPopupItems = 4;

  private:
    KDPoint topLeftCornerForSelection(Escher::View * originView);

    Escher::ListViewDataSource * m_listViewDataSource;
    PopupItemView m_popupViews[k_maxNumberOfPopupItems];
    KDCoordinate m_memoizedCellWidth;
    Escher::SelectableTableView m_selectableTableView;
    Escher::SelectableTableViewDataSource m_selectionDataSource;
    BorderingView m_borderingView;
    DropdownCallback * m_callback;
    Dropdown * m_dropdown;
  };

  DropdownPopupController m_popup;
};

}  // namespace Solver

#endif /* SOLVER_GUI_DROPDOWN_VIEW_H */
