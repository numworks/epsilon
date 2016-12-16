#ifndef PROBABILITY_IMAGE_TABLE_VIEW_H
#define PROBABILITY_IMAGE_TABLE_VIEW_H

#include <escher.h>
#include "left_integral_calculation.h"
#include "right_integral_calculation.h"
#include "finite_integral_calculation.h"

namespace Probability {

class CalculationController;

class ImageTableView : public View, public Responder, public SimpleListViewDataSource {
public:
  ImageTableView(Responder * parentResponder, Calculation * calculation, CalculationController * calculationController);
  void setCalculation(Calculation * calculation);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void select(bool select);
  void setHighlight(bool highlight);
  int numberOfRows() override;
  void willDisplayCellForIndex(TableViewCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  TableViewCell * reusableCell(int index) override;
  int reusableCellCount() override;
  constexpr static KDCoordinate k_imageWidth = 35;
  constexpr static KDCoordinate k_imageHeight = 19;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void setCalculationAccordingToIndex(int index);
  class ImageCell : public TableViewCell {
  public:
    ImageCell();
    void reloadCell() override;
    void setImage(const Image * image, const Image * focusedImage);
  private:
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    ImageView m_iconView;
    const Image * m_icon;
    const Image * m_focusedIcon;
  };
  constexpr static int k_numberOfImages = 3;
  ImageCell m_imageCells[k_numberOfImages];
  SelectableTableView m_selectableTableView;
  bool m_isSelected;
  Calculation * m_calculation;
  CalculationController * m_calculationController;
};

}

#endif
