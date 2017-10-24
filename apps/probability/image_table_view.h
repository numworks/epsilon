#ifndef PROBABILITY_IMAGE_TABLE_VIEW_H
#define PROBABILITY_IMAGE_TABLE_VIEW_H

#include <escher.h>
#include "calculation/calculation.h"

namespace Probability {

class CalculationController;

class ImageCell : public HighlightCell {
public:
  ImageCell();
  void reloadCell() override;
  void setImage(const Image * image, const Image * focusedImage);
  constexpr static KDCoordinate k_width = 39;
  constexpr static KDCoordinate k_height = 23;
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
  ImageTableView(Responder * parentResponder, Law * law, Calculation * calculation, CalculationController * calculationController);
  void drawRect(KDContext * ctx, KDRect rect) const override;
  KDSize minimalSizeForOptimalDisplay() const override;
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
  constexpr static KDCoordinate k_outline = 1;
  constexpr static KDCoordinate k_margin = 3;
  constexpr static KDCoordinate k_totalMargin = k_outline+k_margin;
  constexpr static KDCoordinate k_oneCellWidth = 2*k_totalMargin+ImageCell::k_width;
  constexpr static KDCoordinate k_oneCellHeight = 2*k_totalMargin+ImageCell::k_height;
private:
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
  void setCalculationAccordingToIndex(int index);
  void hideDropdown();
  constexpr static int k_numberOfImages = 4;
  ImageCell m_imageCells[k_numberOfImages];
  SelectableTableView m_selectableTableView;
  bool m_isSelected;
  Law * m_law;
  Calculation * m_calculation;
  CalculationController * m_calculationController;
};

}

#endif
