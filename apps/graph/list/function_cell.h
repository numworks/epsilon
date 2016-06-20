#ifndef GRAPH_FUNCTION_CELL_H
#define GRAPH_FUNCTION_CELL_H

#include <escher.h>

class FunctionCell : public ChildlessView, public Responder {
public:
  FunctionCell();
  void setMessage(const char * message);
  void setEven(bool even);

  void drawRect(KDRect rect) const override;
  void setFocused(bool focused) override;
private:
  const char * m_message;
  bool m_focused;
  bool m_even;
};

#endif
