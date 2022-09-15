#ifndef DISTRIBUTIONS_APP_H
#define DISTRIBUTIONS_APP_H

#include <apps/shared/menu_controller.h>
#include <apps/shared/shared_app.h>
#include <apps/shared/text_field_delegate_app.h>
#include <escher/app.h>
#include <escher/container.h>
#include <escher/stack_view_controller.h>
#include <escher/input_view_controller.h>
#include <ion/ring_buffer.h>

#include "apps/distributions/models/inference.h"
#include "models/models_buffer.h"
#include "probability/distribution_controller.h"
#include "probability/parameters_controller.h"
#include "apps/shared/expression_field_delegate_app.h"

using namespace Escher;

namespace Distributions {

class App : public Shared::ExpressionFieldDelegateApp, public Shared::MenuControllerDelegate {
public:
  // Descriptor
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override { return I18n::Message::DistributionApp; };
    I18n::Message upperName() const override { return I18n::Message::DistributionAppCapital; };
    const Escher::Image * icon() const override;
  };

  // Snapshot
  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    App * unpack(Escher::Container * container) override;
    const Descriptor * descriptor() const override;
    void tidy() override;
    void reset() override;

    Distribution * distribution() { return m_modelBuffer.distribution(); }
    Inference * inference() { return m_modelBuffer.inference(); }
    Calculation * calculation() { return m_modelBuffer.calculation(); }

    Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> * pageQueue() { return &m_pageQueue; }
  private:
    friend App;
    // TODO: optimize size of Stack
    Ion::RingBuffer<Escher::ViewController *, Escher::k_MaxNumberOfStacks> m_pageQueue;
    ModelBuffer m_modelBuffer;
  };

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }
  void didBecomeActive(Window * window) override;

  // Navigation
  void willOpenPage(ViewController * controller) override;
  void didExitPage(ViewController * controller) override;

  TELEMETRY_ID("Distributions");

  // Shared::MenuControllerDelegate
  void selectSubApp(int subAppIndex) override;
  int selectedSubApp() const override { return static_cast<int>(snapshot()->inference()->subApp()); }
  int numberOfSubApps() const override { return static_cast<int>(Inference::SubApp::NumberOfSubApps); }

  Escher::InputViewController * inputViewController() { return &m_inputViewController; }
private:
  App(Snapshot * snapshot, Poincare::Context * parentContext);
  Snapshot * snapshot() const { return static_cast<Snapshot *>(Escher::App::snapshot()); }

  // Controllers
  CalculationController m_calculationController;
  ParametersController m_parameterController;
  DistributionController m_distributionController;
  Shared::MenuController m_menuController;
  Escher::StackViewController m_stackViewController;
  Escher::InputViewController m_inputViewController;
};

}  // namespace Distributions

#endif /* DISTRIBUTIONS_APP_H */
