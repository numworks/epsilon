#ifndef STATISTICS_PLOT_CONTROLLER_H
#define STATISTICS_PLOT_CONTROLLER_H

#include <apps/i18n.h>
#include <escher/button_row_controller.h>
#include <escher/responder.h>
#include <escher/stack_view_controller.h>
#include <escher/view_controller.h>
#include "../store.h"
#include "multiple_data_view_controller.h"
#include "plot_view.h"

namespace Statistics {

class PlotController : public MultipleDataViewController {
public:
  PlotController(Escher::Responder * parentResponder,
                 Escher::ButtonRowController * header,
                 Escher::Responder * tabController,
                 Escher::StackViewController * stackViewController,
                 Escher::ViewController * typeViewController,
                 Store * store,
                 int * selectedPointIndex,
                 int * selectedSeriesIndex);

  // MultipleDataViewController
  MultipleDataView * multipleDataView() override { return plotView(); }
  bool moveSelectionHorizontally(int deltaIndex) override;

  // ViewController
  ViewController::TitlesDisplay titlesDisplay() override { return TitlesDisplay::DisplayNoTitle; }
protected:
  // TODO : Handle a multiline banner to allow a greater precision
  constexpr static int k_numberOfSignificantDigits = Poincare::Preferences::VeryShortNumberOfSignificantDigits;
  constexpr static int k_maxNumberOfCharacters = 30;
  static int SanitizeIndex(int index, int numberOfPairs) {
    return std::max(0, std::min(index, numberOfPairs - 1));
  }

  // MultipleDataViewController
  void reloadBannerView() override;
  virtual PlotView * plotView() = 0;
  virtual const char * resultMessageTemplate() const = 0;
  virtual I18n::Message resultMessage() const = 0;
};

}  // namespace Statistics

#endif /* STATISTICS_PLOT_CONTROLLER_H */
