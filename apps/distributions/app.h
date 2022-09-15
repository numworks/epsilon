#ifndef DISTRIBUTIONS_APP_H
#define DISTRIBUTIONS_APP_H

#include <apps/shared/shared_app.h>
#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/input_view_controller.h>
#include <ion/ring_buffer.h>

#include "models/distribution_buffer.h"
#include "probability/distribution_controller.h"
#include "probability/parameters_controller.h"
#include "apps/shared/expression_field_delegate_app.h"

using namespace Escher;

namespace Distributions {

class App : public Shared::ExpressionFieldDelegateApp {
public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override { return I18n::Message::DistributionsApp; };
    I18n::Message upperName() const override { return I18n::Message::DistributionsAppCapital; };
    const Escher::Image * icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    void reset() override;

    Distribution * distribution() { return m_distributionBuffer.distribution(); }
    Calculation * calculation() { return m_distributionBuffer.distribution()->calculation(); }

    Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> * pageQueue() { return &m_pageQueue; }
  private:
    friend App;
    // TODO: optimize size of Stack
    Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> m_pageQueue;
    DistributionBuffer m_distributionBuffer;
  };

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }
  void didBecomeActive(Window * window) override;

  // Navigation
  void willOpenPage(ViewController * controller) override;
  void didExitPage(ViewController * controller) override;

  TELEMETRY_ID("Distributions");

  Escher::InputViewController * inputViewController() { return &m_inputViewController; }
private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }

  // Controllers
  CalculationController m_calculationController;
  ParametersController m_parameterController;
  DistributionController m_distributionController;
  Escher::StackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
};

}  // namespace Distributions

#endif /* DISTRIBUTIONS_APP_H */
