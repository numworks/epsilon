#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include "../../shared/xy_banner_view.h"

namespace Graph {

class BannerView : public Shared::XYBannerView {
public:
  BannerView(
    Responder * parentResponder,
    InputEventHandlerDelegate * inputEventHandlerDelegate,
    TextFieldDelegate * textFieldDelegate
  );
  BufferTextView * derivativeView() { return &m_derivativeView; }
  BufferTextView * aView() { return &m_aView; }
  BufferTextView * bView() { return &m_bView; }
  void setNumberOfSubviews(int numberOfSubviews) { m_numberOfSubviews = numberOfSubviews; }
  static constexpr int k_numberOfSubviews = Shared::XYBannerView::k_numberOfSubviews + 4;
private:
  int numberOfSubviews() const override { return m_numberOfSubviews; }
  View * subviewAtIndex(int index) override;
  BufferTextView m_derivativeView;
  MessageTextView m_tangentEquationView;
  BufferTextView m_aView;
  BufferTextView m_bView;
  int m_numberOfSubviews;
};

}

#endif
