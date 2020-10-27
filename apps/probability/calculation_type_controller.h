#ifndef PROBABILITY_CALCULATION_TYPE_CONTROLLER_H
#define PROBABILITY_CALCULATION_TYPE_CONTROLLER_H

#include <escher.h>
#include "calculation/calculation.h"
#include "distribution/distribution.h"
#include "image_cell.h"

namespace Probability {

class CalculationController;

class CalculationTypeController : public Escher::ViewController, public Escher::SimpleListViewDataSource, public Escher::SelectableTableViewDataSource {
public:
  CalculationTypeController(Escher::Responder * parentResponder, Distribution * distribution, Calculation * calculation, CalculationController * calculationController);
  Escher::View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() const override;
  KDCoordinate cellWidth() override;
  KDCoordinate cellHeight() override;
  Escher::HighlightCell * reusableCell(int index) override;
  int reusableCellCount() const override;
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
