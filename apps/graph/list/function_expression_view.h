#ifndef GRAPH_FUNCTION_EXPRESSION_H
#define GRAPH_FUNCTION_EXPRESSION_H

#include <escher.h>
#include "../function.h"

class FunctionCell;

class FunctionExpressionView : public ChildlessView, public Responder {
public:
  FunctionExpressionView(Responder * parentResponder);
  Graph::Function * function() const;
  bool isEven() const;
  bool isFocused() const;

  void drawRect(KDContext * ctx, KDRect rect) const override;
  void didBecomeFirstResponder() override;
  void didResignFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool m_focused;
};

#endif
