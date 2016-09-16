#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>
#include "../function.h"
#include "function_expression_view.h"
#include "function_name_view.h"

class FunctionCell : public View, public Responder {
public:
  FunctionCell();
  void setFunction(Graph::Function * f);
  void setEven(bool even);
  bool isEven();
  Graph::Function * function();

  void didBecomeFirstResponder() override;
  void didResignFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;

private:
  static constexpr KDCoordinate k_functionNameWidth = 40;
  static constexpr KDCoordinate k_functionExpressionWidth = 100;
  static constexpr KDCoordinate k_functionCellHeight = 50;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

  FunctionExpressionView m_functionExpressionView;
  FunctionNameView m_functionNameView;
  Graph::Function * m_function;
  bool m_even;
};

#endif
