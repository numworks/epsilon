#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include "../../shared/banner_view.h"

namespace Graph {

class BannerView : public Shared::BannerView {
public:
  BannerView();
  void setNumberOfSubviews(int numberOfSubviews);
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) const override;
  MessageTextView * messageTextViewAtIndex(int i) const override;
  BufferTextView m_abscissaView;
  BufferTextView m_functionView;
  BufferTextView m_derivativeView;
  MessageTextView m_tangentEquationView;
  BufferTextView m_aView;
  BufferTextView m_bView;
  int m_numberOfSubviews;
};

}

#endif
