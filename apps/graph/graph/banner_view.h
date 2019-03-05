#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include "../../shared/xy_banner_view.h"

namespace Graph {

class BannerView : public Shared::XYBannerView {
public:
  BannerView();
  void setNumberOfSubviews(int numberOfSubviews) { m_numberOfSubviews = numberOfSubviews; }
private:
  int numberOfSubviews() const override { return m_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  MessageTextView * messageTextViewAtIndex(int i) const override;
  BufferTextView m_derivativeView;
  MessageTextView m_tangentEquationView;
  BufferTextView m_aView;
  BufferTextView m_bView;
  int m_numberOfSubviews;
};

}

#endif
