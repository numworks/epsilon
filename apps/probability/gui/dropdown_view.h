#ifndef APPS_PROBABILITY_GUI_DROPDOWN_VIEW_H
#define APPS_PROBABILITY_GUI_DROPDOWN_VIEW_H

#include <escher/bordered.h>
#include <escher/image_view.h>
#include <escher/list_view_data_source.h>
#include <escher/responder.h>
#include <escher/selectable_list_view_controller.h>
#include <escher/selectable_table_view.h>
#include <escher/view.h>
#include <ion/events.h>

#include "bordering_view.h"
#include "highlight_image_cell.h"

namespace Probability {

/* Wraps a HighlightCell to add margin and an optional caret. */
class PopupView : public Escher::HighlightCell, public Escher::Bordered {
public:
  PopupView(Escher::HighlightCell * cell = nullptr);
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

/* Wraps a ListViewDataSource to return PopupViews. */
class PopupListViewDataSource : public Escher::ListViewDataSource {
public:
  PopupListViewDataSource(Escher::ListViewDataSource * listViewDataSource);
  int numberOfRows() const override { return m_listViewDataSource->numberOfRows(); }
  KDCoordinate cellWidth() override;
  int typeAtIndex(int index) override { return m_listViewDataSource->typeAtIndex(index); }
  KDCoordinate rowHeight(int j) override;
  int reusableCellCount(int type) override { return m_listViewDataSource->reusableCellCount(type); }
  Escher::HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;

  constexpr static int k_maxNumberOfPopupItems = 4;

private:
  Escher::ListViewDataSource * m_listViewDataSource;
  PopupView m_popupViews[k_maxNumberOfPopupItems];
};

/* List of PopupViews shown in a modal view. */
class DropdownPopup : public Escher::ViewController {
public:
  DropdownPopup(Escher::Responder * parentResponder, Escher::ListViewDataSource * listDataSource);
  Escher::View * view() override { return &m_borderingView; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event e) override;

private:
  PopupListViewDataSource m_listDataSource;
  Escher::SelectableTableView m_selectableTableView;
  Escher::SelectableTableViewDataSource m_selectionDataSource;
  BorderingView m_borderingView;
};

/* A Dropdown is a view that, when clicked on, displays a list of views to choose from
 * It requires a DropdownDataSource to provide a list of views */
class Dropdown : public PopupView, public Escher::Responder {
public:
  Dropdown(Escher::Responder * parentResponder, Escher::ListViewDataSource * listDataSource);
  bool handleEvent(Ion::Events::Event e) override;
  void setHighlighted(bool highlighted) override;

private:
  DropdownPopup m_popup;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_GUI_DROPDOWN_VIEW_H */
