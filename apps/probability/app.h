#ifndef PROBABILITY_PROBABILITY_APP_H
#define PROBABILITY_PROBABILITY_APP_H

#include <escher.h>
#include <poincare.h>
#include "law_controller.h"
#include "../shared/text_field_delegate_app.h"

namespace Probability {

class App : public Shared::TextFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    App * build(Container * container) override;
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  static Descriptor * buildDescriptor();
private:
  App(Container * container, Descriptor * descriptor);
  LawController m_lawController;
  StackViewController m_stackViewController;
};

}

#endif
