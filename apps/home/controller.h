#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

namespace Home {

class Controller : public ViewController, public SimpleTableViewDataSource, public SelectableTableViewDelegate {
public:
  Controller(Responder * parentResponder, SelectableTableViewDataSource * selectionDataSource);

  View * view() override;

  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  TELEMETRY_ID("");

  int numberOfRows() const override;
  int numberOfColumns() const override;
  KDCoordinate cellHeight() override;
  KDCoordinate cellWidth() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY, bool withinTemporarySelection) override;
private:
  int numberOfIcons() const;
  SelectableTableViewDataSource * selectionDataSource() const;
  class ContentView : public View {
  public:
    ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource);
    SelectableTableView * selectableTableView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reloadBottomRow(SimpleTableViewDataSource * dataSource, int numberOfIcons, int numberOfColumns);
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews(bool force = false) override;
    SelectableTableView m_selectableTableView;
  };
  static constexpr KDCoordinate k_sideMargin = 4;
  static constexpr KDCoordinate k_bottomMargin = 14;
  static constexpr KDCoordinate k_indicatorMargin = 61;
  static constexpr int k_numberOfColumns = 3;
  static constexpr int k_maxNumberOfCells = 16;
  static constexpr int k_cellHeight = 104;
  static constexpr int k_cellWidth = 104;
  ContentView m_view;
  AppCell m_cells[k_maxNumberOfCells];
};

}

#endif
