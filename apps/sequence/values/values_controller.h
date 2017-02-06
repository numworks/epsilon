#ifndef SEQUENCE_VALUES_CONTROLLER_H
#define SEQUENCE_VALUES_CONTROLLER_H

#include <escher.h>
#include "../sequence_store.h"

namespace Sequence {

class ValuesController : public ViewController, public HeaderViewDelegate,  public AlternateEmptyViewDelegate {
public:
  ValuesController(Responder * parentResponder, SequenceStore * sequenceStore, HeaderViewController * header);
  const char * title() const override;
  View * view() override;
  void didBecomeFirstResponder() override;
  int numberOfButtons() const override;
  Button * buttonAtIndex(int index) override;
  bool isEmpty() const override;
  const char * emptyMessage() override;
  Responder * defaultController() override;
private:
  SolidColorView m_view;
  SequenceStore * m_sequenceStore;
};

}

#endif
