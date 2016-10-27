#ifndef GRAPH_VALUE_CELL_H
#define GRAPH_VALUE_CELL_H

#include <escher.h>
#include <poincare.h>

namespace Graph {
class ValueCell : public EvenOddCell {
public:
  ValueCell();
  void reloadCell() override;
  void setText(const char * textContent);
  int numberOfSubviews() const override;
  View * subviewAtIndex(int index) override;
  void layoutSubviews() override;

protected:
  BufferTextView m_bufferTextView;
};

}

#endif
