#ifndef GRAPH_FUNCTION_NAME_VIEW_H
#define GRAPH_FUNCTION_NAME_VIEW_H

#include <escher.h>
#include "../function.h"

class FunctionCell;

class FunctionNameView : public ChildlessView, public Responder {
public:
  FunctionNameView(Responder * parentResponder, Invocation invocation);
  Graph::Function * function() const;
  bool isEven() const;
  bool isFocused() const;

  void drawRect(KDContext * ctx, KDRect rect) const override;
  void didBecomeFirstResponder() override;
  void didResignFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  constexpr static int k_colorIndicatorThickness = 4;
  bool m_focused;
  Invocation m_invocation;
};

#endif
