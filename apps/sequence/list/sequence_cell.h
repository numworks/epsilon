#ifndef SEQUENCE_SEQUENCE_CELL_H
#define SEQUENCE_SEQUENCE_CELL_H

#include "../sequence.h"
#include <escher.h>

namespace Sequence {

class SequenceCell : public EvenOddCell {
public:
  SequenceCell();
  virtual void setSequence(Sequence * sequence);
  int numberOfSubCells();
  int selectedSubCell();
  void selectSubCell(int index);
  void setHighlighted(bool highlight) override;
  void setEven(bool even) override;
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;
protected:
  constexpr static KDCoordinate k_separatorThickness = 1;
  int m_numberOfSubCells;
  int m_selectedSubCell;
  Sequence * m_sequence;
private:
  virtual EvenOddCell * viewAtIndex(int index) = 0;
};

}

#endif
