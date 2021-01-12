#ifndef PROBABILITY_CALCULATION_TYPE_CONTROLLER_H
#define PROBABILITY_CALCULATION_TYPE_CONTROLLER_H

#include <escher/selectable_table_view.h>
#include <escher/selectable_table_view_data_source.h>
#include <escher/simple_list_view_data_source.h>
#include <escher/view_controller.h>
#include "calculation/calculation.h"
#include "distribution/distribution.h"
#include "image_cell.h"

namespace Probability {

class CalculationController;

class CalculationTypeController : public Escher::ViewController, public Escher::ListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  CalculationTypeController(Escher::Responder * parentResponder, Distribution * distribution, Calculation * calculation, CalculationController * calculationController);
  Escher::View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate cellWidth() override;
  // KDCoordinate rowHeight(int j) override;
  Escher::HighlightCell * reusableCell(int index, int type) override;
  // int reusableCellCount(int type) override;
  void willDisplayCellForIndex(Escher::HighlightCell * cell, int index) override;
  constexpr static int k_numberOfImages = 4;
private:
  ImageCell m_imageCells[k_numberOfImages];
  Escher::SelectableTableView m_selectableTableView;
  Distribution * m_distribution;
  Calculation * m_calculation;
  CalculationController * m_calculationController;
};

}

#endif
