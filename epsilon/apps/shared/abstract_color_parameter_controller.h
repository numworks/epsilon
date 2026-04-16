#pragma once

#include <escher/selectable_list_view_controller.h>
#include <escher/stack_view_controller.h>

#include "color_cell.h"
#include "color_names.h"

namespace Shared {

class AbstractColorParameterController
    : public Escher::SelectableListViewController<
          Escher::SimpleListViewDataSource> {
 public:
  AbstractColorParameterController(Escher::Responder* parentResponder)
      : SelectableListViewController<SimpleListViewDataSource>(
            parentResponder) {}

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

 protected:
  virtual int currentColorIndex() const = 0;
  virtual void setColorIndex(int colorIndex) = 0;
  // Default navigation: pop once (suitable for data app parameter menus).
  // Override for apps that need deeper stack unwinding.
  virtual void popAfterColorSelection(Escher::StackViewController* stack) {
    stack->pop();
  }

 private:
  // Remaining cell can be above and below so we add +2
  constexpr static int k_numberOfCells =
      Escher::Metric::MinimalNumberOfScrollableRowsToFillDisplayHeight(
          Escher::AbstractMenuCell::k_minimalLargeFontCellHeight,
          Escher::Metric::TabHeight + 2 * Escher::Metric::StackTitleHeight);

  KDCoordinate defaultRowHeight() override;

  ColorCell m_cells[k_numberOfCells];
};

}  // namespace Shared
