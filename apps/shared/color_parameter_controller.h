#ifndef SHARED_COLOR_PARAMETER_CONTROLLER_H
#define SHARED_COLOR_PARAMETER_CONTROLLER_H

#include <escher/selectable_list_view_controller.h>

#include "color_cell.h"
#include "color_names.h"
#include "function_store.h"

namespace Shared {

class ColorParameterController : public Escher::SelectableListViewController<
                                     Escher::SimpleListViewDataSource> {
 public:
  ColorParameterController(Escher::Responder* parentResponder)
      : SelectableListViewController<SimpleListViewDataSource>(
            parentResponder) {}

  // ViewController
  const char* title() override { return I18n::translate(I18n::Message::Color); }
  TitlesDisplay titlesDisplay() override {
    return TitlesDisplay::DisplayLastThreeTitles;
  }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;

  // SimpleListViewDataSource
  Escher::HighlightCell* reusableCell(int index) override {
    return &m_cells[index];
  }
  int reusableCellCount() const override { return k_numberOfCells; }
  int numberOfRows() const override { return ColorNames::k_count; }
  void willDisplayCellForIndex(Escher::HighlightCell* cell, int index) override;

  // ColorParameterController
  void setRecord(Ion::Storage::Record record) { m_record = record; }

 private:
  constexpr static int k_numberOfCells =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::TableCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight +
              2 * Escher::Metric::StackTitleHeight);  // Remaining cell can be
                                                      // above and below so we
                                                      // add +2

  KDCoordinate defaultRowHeight() override;
  ExpiringPointer<Function> function();

  Ion::Storage::Record m_record;
  ColorCell m_cells[k_numberOfCells];
};

}  // namespace Shared

#endif
