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
      : SelectableListViewController<SimpleListViewDataSource>(parentResponder),
        m_derivationOrder(0) {}

  // ViewController
  const char* title() const override {
    return I18n::translate(I18n::Message::Color);
  }
  TitlesDisplay titlesDisplay() const override {
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
  void fillCellForRow(Escher::HighlightCell* cell, int row) override;

  // ColorParameterController
  void setRecord(Ion::Storage::Record record, int derivationOrder = 0);

 private:
  // Remaining cell can be above and below so we add +2
  constexpr static int k_numberOfCells =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight + 2 * Escher::Metric::StackTitleHeight);

  KDCoordinate defaultRowHeight() override;
  ExpiringPointer<Function> function();

  Ion::Storage::Record m_record;
  ColorCell m_cells[k_numberOfCells];
  int m_derivationOrder;
};

}  // namespace Shared

#endif
