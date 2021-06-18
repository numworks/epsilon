#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "calculation_store.h"
#include "edit_expression_controller.h"
#include "history_controller.h"
#include "../shared/text_field_delegate_app.h"
#include <escher.h>
#include "../shared/shared_app.h"

namespace Calculation {

class App : public Shared::ExpressionFieldDelegateApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    App::Descriptor::ExaminationLevel examinationLevel() override;
    const Image * icon() override;
  };
  class Snapshot : public ::SharedApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    CalculationStore * calculationStore() { return &m_calculationStore; }
    char * cacheBuffer() { return m_cacheBuffer; }
    size_t * cacheBufferInformationAddress() { return &m_cacheBufferInformation; }
  private:
    CalculationStore m_calculationStore;
    // Set the size of the buffer needed to store the calculations
    static constexpr int k_calculationBufferSize = 10 * (sizeof(Calculation) + Calculation::k_numberOfExpressions * Constant::MaxSerializedExpressionSize + sizeof(Calculation *));
    char m_calculationBuffer[k_calculationBufferSize];
    char m_cacheBuffer[EditExpressionController::k_cacheBufferSize];
    size_t m_cacheBufferInformation;
  };
  static App * app() {
    return static_cast<App *>(Container::activeApp());
  }
  TELEMETRY_ID("Calculation");
  bool textFieldDidReceiveEvent(::TextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(::LayoutField * layoutField, Ion::Events::Event event) override;
  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;

private:
  App(Snapshot * snapshot);
  HistoryController m_historyController;
  void didBecomeActive(Window * window) override;
  void willBecomeInactive() override;
  EditExpressionController m_editExpressionController;
};

}

#endif
