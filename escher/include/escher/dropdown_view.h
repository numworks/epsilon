#ifndef ESCHER_DROPDOWN_VIEW_H
#define ESCHER_DROPDOWN_VIEW_H

#include <escher/bordered.h>
#include <escher/bordering_view.h>
#include <escher/image_view.h>
#include <escher/list_view_data_source.h>
#include <escher/list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/transparent_image_view.h>
#include <escher/view.h>
#include <ion/events.h>

namespace Escher {

// TODO: refactor

/* Wraps a HighlightCell to add margins and an optional caret. */
class PopupItemView : public HighlightCell, public Bordered {
public:
  PopupItemView(HighlightCell * cell = nullptr);
  void setHighlighted(bool highlighted) override;
  KDSize minimalSizeForOptimalDisplay() const override;
  void layoutSubviews(bool force) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int i) override;
  HighlightCell * innerCell() { return m_cell; }
  void setInnerCell(HighlightCell * cell) { m_cell = cell; }
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setPopping(bool popping) { m_isPoppingUp = popping; }

protected:
  bool m_isPoppingUp;

private:
  constexpr static int k_marginCaretRight = 2;
  constexpr static int k_marginImageHorizontal = 3;
  constexpr static int k_marginImageVertical = 2;
  HighlightCell * m_cell;
  TransparentImageView m_caret;
};

class DropdownCallback {
public:
  virtual void onDropdownSelected(int selectedRow) = 0;
  virtual bool popupDidReceiveEvent(Ion::Events::Event event, Responder * responder) { return false; }
};

/* A Dropdown is a view that, when clicked on, displays a list of views to choose from
 * It requires a DropdownDataSource to provide a list of views */
class Dropdown : public PopupItemView, public Responder {
public:
  Dropdown(Responder * parentResponder,
           ListViewDataSource * listDataSource,
           DropdownCallback * callback = nullptr);
  Responder * responder() override { return this; }
  bool handleEvent(Ion::Events::Event & e) override;
  void registerCallback(DropdownCallback * callback) { m_popup.registerCallback(callback); }
  void reloadAllCells();
  void init();
  int selectedRow() { return m_popup.selectedRow(); }
  void selectRow(int row) { m_popup.selectRow(row); }

  void open();
  void close();

private:
  /* List of PopupViews shown in a modal view + Wraps a ListViewDataSource to return PopupViews. */

  class DropdownPopupController : public ViewController, public MemoizedListViewDataSource {
  public:
    friend Dropdown;
    DropdownPopupController(Responder * parentResponder,
                            ListViewDataSource * listDataSource,
                            Dropdown * dropdown,
                            DropdownCallback * callback = nullptr);

    // View Controller
    View * view() override { return &m_borderingView; }
    void didBecomeFirstResponder() override;
    bool handleEvent(Ion::Events::Event & e) override;
    void registerCallback(DropdownCallback * callback) { m_callback = callback; }
    int selectedRow() { return m_selectionDataSource.selectedRow(); }
    void selectRow(int row);
    void close();

    // MemoizedListViewDataSource
    int numberOfRows() const override { return m_listViewDataSource->numberOfRows(); }
    KDCoordinate defaultColumnWidth() override;
    int typeAtIndex(int index) const override { return m_listViewDataSource->typeAtIndex(index); }
    KDCoordinate nonMemoizedRowHeight(int j) override;
    int reusableCellCount(int type) override {
      return m_listViewDataSource->reusableCellCount(type);
    }
    PopupItemView * reusableCell(int index, int type) override;
    void willDisplayCellForIndex(HighlightCell * cell, int index) override;
    void resetMemoization(bool force = true) override;
    HighlightCell * innerCellAtIndex(int index);

    constexpr static int k_maxNumberOfPopupItems = 4;

  private:
    KDPoint topLeftCornerForSelection(View * originView);

    ListViewDataSource * m_listViewDataSource;
    PopupItemView m_popupViews[k_maxNumberOfPopupItems];
    KDCoordinate m_memoizedCellWidth;
    SelectableTableViewDataSource m_selectionDataSource;
    SelectableTableView m_selectableTableView;
    BorderingView m_borderingView;
    DropdownCallback * m_callback;
    Dropdown * m_dropdown;
  };

  DropdownPopupController m_popup;
};

}  // namespace Escher

#endif /* ESCHER_DROPDOWN_VIEW_H */
