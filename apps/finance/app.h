#ifndef FINANCE_APP_H
#define FINANCE_APP_H

#include "../shared/input_event_handler_delegate_app.h"
#include "../shared/shared_app.h"

namespace Finance {

class App : public Shared::InputEventHandlerDelegateApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
  };
  TELEMETRY_ID("FinanceTable");

private:
  App(Snapshot * snapshot);

  // TODO Remove
  class PlaceholderController : public Escher::ViewController {
  public:
    using Escher::ViewController::ViewController;
    const char * title() override { return "Placeholder"; }
    Escher::View * view() override { return &m_view; }

  private:
    class ContentView : public Escher::View {
    public:
      void drawRect(KDContext * ctx, KDRect rect) const;
    };

    ContentView m_view;
  };

  PlaceholderController m_controller;
};

}

#endif