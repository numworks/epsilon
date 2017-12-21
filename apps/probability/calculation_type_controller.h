#ifndef PROBABILITY_CALCULATION_TYPE_CONTROLLER_H
#define PROBABILITY_CALCULATION_TYPE_CONTROLLER_H

#include <escher.h>
#include "calculation/calculation.h"
#include "law/law.h"
#include "image_cell.h"

namespace Probability {

class CalculationController;

class CalculationTypeController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  CalculationTypeController(Responder * parentResponder, Law * law, Calculation * calculation, CalculationController * calculationController);
  View * view() override;
  void viewWillAppear() override;
  void viewDidDisappear() override;
  void didBecomeFirstResponder() override;
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  KDCoordinate cellWidth() override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  constexpr static int k_numberOfImages = 4;
private:
  ImageCell m_imageCells[k_numberOfImages];
  SelectableTableView m_selectableTableView;
  Law * m_law;
  Calculation * m_calculation;
  CalculationController * m_calculationController;
};

}

#endif
