#ifndef STATISTICS_FREQUENCY_CONTROLLER_H
#define STATISTICS_FREQUENCY_CONTROLLER_H

#include "plot_controller.h"
#include "frequency_view.h"

namespace Statistics {

class FrequencyController : public PlotController {
public:
  FrequencyController(Escher::Responder * parentResponder,
                      Escher::ButtonRowController * header,
                      Escher::Responder * tabController,
                      Escher::StackViewController * stackViewController,
                      Escher::ViewController * typeViewController,
                      Store * store,
                      int * selectedPointIndex,
                      int * selectedSeriesIndex) :
      PlotController(parentResponder,
                     header,
                     tabController,
                     stackViewController,
                     typeViewController,
                     store,
                     selectedPointIndex,
                     selectedSeriesIndex),
      m_view(store) {}

  TELEMETRY_ID("Frequency");
private:
  PlotView * plotView() override { return &m_view; }
  const char * resultMessageTemplate() const override { return "%s : %*.*ed%%"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsFrequencyFcc; }

  FrequencyView m_view;
};

}  // namespace Statistics

#endif /* STATISTICS_FREQUENCY_CONTROLLER_H */
