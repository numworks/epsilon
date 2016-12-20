#ifndef GRAPH_BANNER_VIEW_H
#define GRAPH_BANNER_VIEW_H

#include <escher.h>
#include "../function.h"

namespace Graph {

class BannerView : public View {
public:
  BannerView();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void setContext(Context * context);
  void setAbscissa(float x);
  void setFunction(Function * f);
  void setDisplayDerivative(bool displayDerivative);
  bool displayDerivative();
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  float m_abscissa;
  Function * m_function;
  BufferTextView m_abscissaView;
  BufferTextView m_functionView;
  BufferTextView m_derivativeView;
  bool m_displayDerivative;
  Context * m_context;
};

}

#endif
