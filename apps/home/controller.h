#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/selectable_table_view_delegate.h>
#include <escher/regular_table_view_data_source.h>
#include <escher/view_controller.h>
#include "app_cell.h"

namespace Home {

class Controller : public Escher::ViewController, public Escher::SimpleTableViewDataSource, public Escher::SelectableTableViewDelegate {
public:
  Controller(Escher::Responder * parentResponder, Escher::SelectableTableViewDataSource * selectionDataSource);

  Escher::View * view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("");

  int numberOfRows() const override;
  int numberOfColumns() const override;
  Escher::HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellAtLocation(Escher::HighlightCell * cell, int i, int j) override;
  void tableViewDidChangeSelectionAndDidScroll(Escher::SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
private:
  // SimpleTableViewDataSource
  KDCoordinate defaultRowHeight() override;
  KDCoordinate defaultColumnWidth() override;

  int numberOfIcons() const;
  Escher::SelectableTableViewDataSource * selectionDataSource() const;
  void switchToSelectedApp();
  class ContentView : public Escher::View {
  public:
    ContentView(Controller * controller, Escher::SelectableTableViewDataSource * selectionDataSource);
    Escher::SelectableTableView * selectableTableView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reload();
    void reloadBottomRow(SimpleTableViewDataSource * dataSource, int numberOfIcons, int numberOfColumns);
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    Escher::SelectableTableView m_selectableTableView;
  };
  constexpr static KDCoordinate k_sideMargin = 4;
  constexpr static KDCoordinate k_bottomMargin = 14;
  constexpr static KDCoordinate k_indicatorMargin = 61;
  constexpr static int k_numberOfColumns = 3;
  constexpr static int k_maxNumberOfCells = 16;
  constexpr static int k_cellHeight = 104;
  constexpr static int k_cellWidth = 104;
  ContentView m_view;
  AppCell m_cells[k_maxNumberOfCells];
};

}

#endif
