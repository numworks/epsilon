#ifndef HOME_CONTROLLER_H
#define HOME_CONTROLLER_H

#include <escher.h>
#include "app_cell.h"

class AppsContainer;

namespace Home {

class Controller final : public ViewController, public SimpleTableViewDataSource, public SelectableTableViewDelegate {
public:
  Controller(Responder * parentResponder, ::AppsContainer * container, SelectableTableViewDataSource * selectionDataSource) :
    ViewController(parentResponder),
    m_container(container),
    m_view(this, selectionDataSource),
    m_selectionDataSource(selectionDataSource) {}
  View * view() override {
    return &m_view;
  }
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void viewWillAppear() override;

  int numberOfRows() override;
  int numberOfColumns() override;
  KDCoordinate cellHeight() override;
  KDCoordinate cellWidth() override;
  HighlightCell * reusableCell(int index) override {
    return &m_cells[index];
  }
  int reusableCellCount() override;
  void willDisplayCellAtLocation(HighlightCell * cell, int i, int j) override;
  void tableViewDidChangeSelection(SelectableTableView * t, int previousSelectedCellX, int previousSelectedCellY) override;
private:
  int numberOfIcons();
  class ContentView final : public View {
  public:
    ContentView(Controller * controller, SelectableTableViewDataSource * selectionDataSource) :
      m_selectableTableView(controller, controller, selectionDataSource, controller) {
      m_selectableTableView.setVerticalCellOverlap(0);
      m_selectableTableView.setMargins(0, k_sideMargin, 0, k_sideMargin);
      m_selectableTableView.setColorsBackground(false);
      m_selectableTableView.setIndicatorThickness(k_indicatorThickness);
      m_selectableTableView.horizontalScrollIndicator()->setMargin(k_indicatorMargin);
      m_selectableTableView.verticalScrollIndicator()->setMargin(k_indicatorMargin);
    }
    SelectableTableView * selectableTableView() {
      return &m_selectableTableView;
    }
    void drawRect(KDContext * ctx, KDRect rect) const override;
    void reloadBottomRightCorner(SimpleTableViewDataSource * dataSource);
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    SelectableTableView m_selectableTableView;
  };
  AppsContainer * m_container;
  static constexpr KDCoordinate k_sideMargin = 4;
  static constexpr KDCoordinate k_indicatorThickness = 28;
  static constexpr KDCoordinate k_indicatorMargin = 116;
  static constexpr int k_numberOfRows = 2;
  static constexpr int k_maxNumberOfCells = 16;
  static constexpr int k_cellHeight = 98;
  static constexpr int k_cellWidth = 104;
  ContentView m_view;
  AppCell m_cells[k_maxNumberOfCells];
  SelectableTableViewDataSource * m_selectionDataSource;
};

}

#endif
