#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include <escher.h>
#include "graph_window.h"
#include "../../banner_view.h"

namespace Graph {

class BannerView : public ::BannerView {
public:
  BannerView(GraphWindow * graphWindow);
  void reload() override;
  void setDisplayDerivative(bool displayDerivative);
  bool displayDerivative();
private:
  constexpr static int k_maxNumberOfCharacters = 8;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  BufferTextView m_abscissaView;
  BufferTextView m_functionView;
  BufferTextView m_derivativeView;
  bool m_displayDerivative;
  GraphWindow * m_graphWindow;
};

}

#endif
