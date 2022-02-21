#ifndef STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H
#define STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H

#include <escher/button_row_controller.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "../store.h"
#include "multiple_data_view_controller.h"
#include "normal_probability_view.h"

namespace Statistics {

class NormalProbabilityController : public MultipleDataViewController {
public:
  NormalProbabilityController(Escher::Responder * parentResponder,
                Escher::ButtonRowController * header,
                Escher::Responder * tabController,
                Escher::StackViewController * stackViewController,
                Escher::ViewController * typeViewController,
                Store * store,
                int * selectedPointIndex,
                int * selectedSeriesIndex);

  // MultipleDataViewController
  MultipleDataView * multipleDataView() override { return &m_view; }
  bool moveSelectionHorizontally(int deltaIndex) override;

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayNoTitle; }
  TELEMETRY_ID("NormalProbability");
private:
  // TODO : Handle a multiline banner to allow a greater precision
  constexpr static int k_numberOfSignificantDigits = Poincare::Preferences::VeryShortNumberOfSignificantDigits;
  constexpr static int k_maxNumberOfCharacters = 30;
  static int SanitizeIndex(int index, int numberOfPairs) {
    return std::max(0, std::min(index, numberOfPairs - 1));
  }

  // MultipleDataViewController
  void reloadBannerView() override;

  NormalProbabilityView m_view;
};

}  // namespace Statistics

#endif /* STATISTICS_NORMAL_PROBABILITY_CONTROLLER_H */
