#ifndef PROBABILITY_LAW_CELL_H
#define PROBABILITY_LAW_CELL_H

#include <escher.h>

namespace Probability {

class LawCell : public ChildlessView, public Responder {
public:
  LawCell();
  void setMessage(const char * message);
  void setEven(bool even);

  void drawRect(KDContext * ctx, KDRect rect) const override;
  void didBecomeFirstResponder() override;
  void didResignFirstResponder() override;
private:
  const char * m_message;
  bool m_focused;
  bool m_even;
};

}

#endif
