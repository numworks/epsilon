#ifndef ESCHER_PERVASIVE_BOX_H
#define ESCHER_PERVASIVE_BOX_H

namespace Escher {

class InputEventHandler;

class PervasiveBox {
public:
  PervasiveBox() : m_sender(nullptr) {}
  InputEventHandler * sender() { return m_sender; }
  void setSender(InputEventHandler * sender) { m_sender = sender; }
  virtual void open() = 0;
private:
  InputEventHandler * m_sender;
};

}

#endif
