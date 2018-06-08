#ifndef SOLVER_EQUATION_LIST_VIEW_H
#define SOLVER_EQUATION_LIST_VIEW_H

#include <escher.h>

namespace Solver {

class EquationListView : public Responder, public View, public ScrollViewDelegate, public ScrollViewDataSource {
public:
  EquationListView(Responder * parentResponder, TableViewDataSource * dataSource, SelectableTableViewDataSource * selectionDataSource);
  void displayBrace(bool displayBrace);
  void scrollViewDidChangeOffset(ScrollViewDataSource * scrollViewDataSource) override;
  void didBecomeFirstResponder() override;
  SelectableTableView * selectableTableView() {
    return &m_listView;
  }
  constexpr static KDCoordinate k_braceTotalWidth = 30;
  constexpr static KDCoordinate k_margin = 10;
  void layoutSubviews() override;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;

  class BraceView : public View {
  public:
    void drawRect(KDContext * ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    constexpr static KDCoordinate k_braceWidth = 10;
  };
  bool m_displayBrace;
  SelectableTableView m_listView;
  BraceView m_braceView;
  ScrollView m_scrollBraceView;
};

}

#endif

