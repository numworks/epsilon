#ifndef CALCULATION_APP_H
#define CALCULATION_APP_H

#include "calculation_store.h"
#include "edit_expression_controller.h"
#include "history_controller.h"
#include "../shared/expression_field_delegate_app.h"
#include "../shared/shared_app.h"
#include <new>

namespace Calculation {

class App : public Shared::ExpressionFieldDelegateApp {
public:
  class Descriptor : public Escher::App::Descriptor {
  public:
    I18n::Message name() const override;
    I18n::Message upperName() const override;
    const Escher::Image * icon() const override;
  };

  class Snapshot : public Shared::SharedApp::Snapshot {
  public:
    Snapshot();

    App * unpack(Escher::Container * container) override;
    void reset() override;
    const Descriptor * descriptor() const override;
    CalculationStore * calculationStore() { return &m_calculationStore; }
    char * cacheBuffer() { return m_cacheBuffer; }
    size_t * cacheBufferInformationAddress() { return &m_cacheBufferInformation; }
    int * cacheCursorOffset() { return &m_cacheCursorOffset; }
    int * cacheCursorPosition() { return &m_cacheCursorPosition; }

  private:
    // Set the size of the buffer needed to store the calculations
    constexpr static int k_calculationBufferSize = 10 * (sizeof(Calculation) + Calculation::k_numberOfExpressions * Constant::MaxSerializedExpressionSize + sizeof(Calculation *));

    CalculationStore m_calculationStore;
    char m_calculationBuffer[k_calculationBufferSize];
    char m_cacheBuffer[EditExpressionController::k_cacheBufferSize];
    size_t m_cacheBufferInformation;
    int m_cacheCursorOffset;
    int m_cacheCursorPosition;
  };

  static App * app() { return static_cast<App *>(Escher::Container::activeApp()); }

  TELEMETRY_ID("Calculation");

  bool textFieldDidReceiveEvent(Escher::AbstractTextField * textField, Ion::Events::Event event) override;
  bool layoutFieldDidReceiveEvent(Escher::LayoutField * layoutField, Ion::Events::Event event) override;

  // TextFieldDelegateApp
  bool isAcceptableExpression(const Poincare::Expression expression) override;

  Snapshot * snapshot() const { return static_cast<Snapshot *>(Shared::ExpressionFieldDelegateApp::snapshot()); }

private:
  App(Snapshot * snapshot);

  void didBecomeActive(Escher::Window * window) override;

  HistoryController m_historyController;
  EditExpressionController m_editExpressionController;
};

}

#endif
