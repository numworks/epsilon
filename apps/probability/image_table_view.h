#ifndef PROBABILITY_IMAGE_TABLE_VIEW_H
#define PROBABILITY_IMAGE_TABLE_VIEW_H

#include <escher.h>
#include "law.h"

namespace Probability {

class ImageTableView : public View, public Responder, public SimpleListViewDataSource {
public:
  ImageTableView(Responder * parentResponder, Law * law);
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
  Law * m_law;
};

}

#endif
