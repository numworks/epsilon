#ifndef EXPRESSION_EDITOR_CONTROLLER_H
#define EXPRESSION_EDITOR_CONTROLLER_H

#include <escher.h>
#include <poincare.h>
#include "expression_and_layout.h"
extern "C" {
#include <assert.h>
}

namespace ExpressionEditor {

class Controller : public ViewController {
public:
  Controller(Responder * parentResponder, ExpressionAndLayout * expressionAndLayout);
  View * view() override { return &m_view; }
  bool handleEvent(Ion::Events::Event event) override;

private:
  class ContentView : public SolidColorView {
  public:
    ContentView(ExpressionAndLayout * expressionAndLayout);
    int numberOfSubviews() const override { return 1; }
    View * subviewAtIndex(int index) override {
      assert(index == 0);
      return &m_expressionView;
    }
    void layoutSubviews() override;
    KDSize minimalSizeForOptimalDisplay() const override;
  private:
    constexpr static KDCoordinate k_margin = 10;
    ExpressionView m_expressionView;
  };
  ContentView m_view;
};

}

#endif
