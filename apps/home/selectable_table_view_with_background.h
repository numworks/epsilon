#ifndef ESCHER_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H
#define ESCHER_SELECTABLE_TABLE_VIEW_WITH_BACKGROUND_H

#include <escher/selectable_table_view.h>
#include <escher/background_view.h>

class SelectableTableViewWithBackground : public SelectableTableView {
public:
  SelectableTableViewWithBackground(Responder * parentResponder, TableViewDataSource * dataSource, BackgroundView * backgroundView,
    SelectableTableViewDataSource * selectionDataSource = nullptr, SelectableTableViewDelegate * delegate = nullptr);
  View * subviewAtIndex(int index) override { return (index == 0) ? &m_backgroundInnerView : decorator()->indicatorAtIndex(index); }
protected:
  virtual InnerView * getInnerView() { return &m_backgroundInnerView; }
  class BackgroundInnerView : public ScrollView::InnerView {
    public:
      BackgroundInnerView(ScrollView * scrollView, BackgroundView * backgroundView): InnerView(scrollView), m_backgroundView(backgroundView) {}
      void drawRect(KDContext * ctx, KDRect rect) const override;
      void setBackgroundView(const uint8_t * data);
    private:
      BackgroundView * m_backgroundView;
  };
  BackgroundInnerView m_backgroundInnerView;
};

#endif
