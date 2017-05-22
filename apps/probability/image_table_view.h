#ifndef PROBABILITY_IMAGE_TABLE_VIEW_H
#define PROBABILITY_IMAGE_TABLE_VIEW_H

#include <escher.h>
#include "calculation/calculation.h"

namespace Probability {

class CalculationController;

class ImageCell : public HighlightCell {
public:
  ImageCell();
  void drawRect(KDContext * ctx, KDRect rect) const override;
  void reloadCell() override;
  void setImage(const Image * image, const Image * focusedImage);
  constexpr static KDCoordinate k_imageMargin = 3;
  constexpr static KDCoordinate k_imageWidth = 35;
  constexpr static KDCoordinate k_imageHeight = 19;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  ImageView m_iconView;
  const Image * m_icon;
  const Image * m_focusedIcon;
};

class ImageTableView : public View, public Responder, public SimpleListViewDataSource, public SelectableTableViewDataSource {
  public:
    ImageTableView(Responder * parentResponder, Calculation * calculation, CalculationController * calculationController);
  void setCalculation(Calculation * calculation, int index);
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
  void select(bool select);
  void setHighlight(bool highlight);
  int numberOfRows() override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate cellHeight() override;
  HighlightCell * reusableCell(int index) override;
  int reusableCellCount() override;
  constexpr static KDCoordinate k_imageCellWidth = 2*ImageCell::k_imageMargin+ImageCell::k_imageWidth;
  constexpr static KDCoordinate k_imageCellHeight = 2*ImageCell::k_imageMargin+ImageCell::k_imageHeight;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void setCalculationAccordingToIndex(int index);
  constexpr static int k_numberOfImages = 3;
  ImageCell m_imageCells[k_numberOfImages];
  SelectableTableView m_selectableTableView;
  bool m_isSelected;
  int m_selectedIcon;
  Calculation * m_calculation;
  CalculationController * m_calculationController;
};

}

#endif
