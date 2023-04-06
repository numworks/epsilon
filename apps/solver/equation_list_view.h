#ifndef SOLVER_EQUATION_LIST_VIEW_H
#define SOLVER_EQUATION_LIST_VIEW_H

#include <escher/responder.h>
#include <escher/scrollable_view.h>
#include <escher/selectable_list_view.h>
#include <escher/view.h>

namespace Solver {

class ListController;

class EquationListView : public Escher::Responder,
                         public Escher::View,
                         public Escher::ScrollViewDelegate,
                         public Escher::ScrollViewDataSource {
 public:
  enum class BraceStyle { None, OneRowShort, Full };
  EquationListView(ListController* listController);
  void setBraceStyle(BraceStyle style);
  void scrollViewDidChangeOffset(
      Escher::ScrollViewDataSource* scrollViewDataSource) override;
  void didBecomeFirstResponder() override;
  Escher::SelectableListView* selectableListView() { return &m_listView; }
  void layoutSubviews(bool force = false) override;

 private:
  constexpr static KDCoordinate k_margin = 10;
  constexpr static KDCoordinate k_rightMargin = 8;

  int numberOfSubviews() const override;
  Escher::View* subviewAtIndex(int index) override;

  class BraceView : public View {
   public:
    void drawRect(KDContext* ctx, KDRect rect) const override;
    KDSize minimalSizeForOptimalDisplay() const override;
    constexpr static KDCoordinate k_braceWidth = 10;
  };
  BraceStyle m_braceStyle;
  Escher::SelectableListView m_listView;
  BraceView m_braceView;
  Escher::ScrollableView<Escher::ScrollView::NoDecorator> m_scrollBraceView;

 public:
  constexpr static KDCoordinate k_braceTotalWidth =
      k_margin + k_rightMargin + BraceView::k_braceWidth;
};

}  // namespace Solver

#endif
