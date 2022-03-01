#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include "plot_controller.h"
#include "normal_probability_view.h"

namespace Statistics {

class NormalProbabilityController : public PlotController {
public:
  NormalProbabilityController(Escher::Responder * parentResponder,
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

  TELEMETRY_ID("NormalProbability");
private:
  PlotView * plotView() override { return &m_view; }
  const char * resultMessageTemplate() const override { return "%s : %*.*ed"; }
  I18n::Message resultMessage() const override { return I18n::Message::StatisticsNormalProbabilityZScore; }

  NormalProbabilityView m_view;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H */
