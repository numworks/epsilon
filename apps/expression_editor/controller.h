#ifndef EXPRESSION_EDITOR_CONTROLLER_H
#define EXPRESSION_EDITOR_CONTROLLER_H

#include <escher.h>
#include <poincare/expression_layout_cursor.h>
#include "expression_and_layout.h"
#include "expression_editor_view.h"
extern "C" {
#include <assert.h>
}

namespace ExpressionEditor {

class Controller : public ViewController {
public:
  Controller(Responder * parentResponder, Poincare::ExpressionLayout * expressionLayout);
  View * view() override { return &m_view; }
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
private:
  bool privateHandleEvent(Ion::Events::Event event);
  bool handleMoveEvent(Ion::Events::Event event);
  Poincare::ExpressionLayout * handleAddEvent(Ion::Events::Event event);
  ExpressionEditorView m_view;
  Poincare::ExpressionLayout * m_expressionLayout;
  Poincare::ExpressionLayoutCursor m_cursor;
};

}

#endif
