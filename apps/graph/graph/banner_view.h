#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include <escher.h>
#include "../../banner_view.h"

namespace Graph {

class BannerView : public ::BannerView {
public:
  BannerView();
  void setDisplayDerivative(bool displayDerivative);
  bool displayDerivative();
private:
  int numberOfSubviews() const override;
  TextView * textViewAtIndex(int i) override;
  BufferTextView m_abscissaView;
  BufferTextView m_functionView;
  BufferTextView m_derivativeView;
  bool m_displayDerivative;
};

}

#endif
