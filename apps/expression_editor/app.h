#ifndef EXPRESSION_EDITOR_APP_H
#define EXPRESSION_EDITOR_APP_H

#include <escher.h>
#include <apps/math_toolbox.h>
#include "controller.h"
#include "expression_and_layout.h"

namespace ExpressionEditor {

/* TODO This app is used for creating ExpressionLayout edition. It should be
 * removed when the development is finished. */

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
  };
  class Snapshot : public ::App::Snapshot, public SelectableTableViewDataSource {
  public:
    App * unpack(Container * container) override;
    Descriptor * descriptor() override;
    ExpressionAndLayout * expressionAndLayout() { return &m_expressionAndLayout; }
  private:
    ExpressionAndLayout m_expressionAndLayout;
  };
  MathToolbox * mathToolbox() { return &m_toolbox; }
private:
  App(Container * container, Snapshot * snapshot);
  Controller m_controller;
  MathToolbox m_toolbox;
};

}

#endif
