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
  ExpressionEditorView m_view;
  Poincare::ExpressionLayoutCursor m_cursor;
};

}

#endif
