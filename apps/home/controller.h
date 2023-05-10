#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "selectable_table_view_with_background.h"
#include "app_cell.h"

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource, public SelectableTableViewDelegate {
public:
  Controller(Responder * parentResponder, SelectableTableViewDataSource * selectionDataSource, App * app);

  View * view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;

  int numberOfRows() const override;
  int numberOfColumns() const override;
  KDCoordinate cellHeight() override;
  KDCoordinate cellWidth() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
  void tableViewDidChangeSelectionAndDidScroll(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
private:
  int numberOfIcons() const;
  SelectableTableViewDataSource * selectionDataSource() const;
  class ContentView : public View {
  public:
    ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource);
    SelectableTableView * selectableTableView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reloadBottomRow(SimpleTableViewDataSource * dataSource, int numberOfIcons, int numberOfColumns);
    BackgroundView * backgroundView();
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    SelectableTableViewWithBackground m_selectableTableView;
    BackgroundView m_backgroundView;
  };
  static constexpr KDCoordinate k_sideMargin = 4;
  static constexpr KDCoordinate k_bottomMargin = 14;
  static constexpr KDCoordinate k_indicatorMargin = 61;

  #ifndef _FXCG
  static constexpr int k_numberOfColumns = 3;
  static constexpr int k_cellHeight = 104;
  static constexpr int k_cellWidth = 104;
  #else
  // A different screen resolution so different dimensions
  static constexpr int k_numberOfColumns = 4;
  static constexpr int k_cellHeight = 96;
  static constexpr int k_cellWidth = 97;
  #endif

  static constexpr int k_maxNumberOfCells = 16;
  ContentView m_view;
  AppCell m_cells[k_maxNumberOfCells];
  App * m_app;
};

}

#endif
