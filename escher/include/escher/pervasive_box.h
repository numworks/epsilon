#ifndef ESCHER_PERVASIVE_BOX_H
#define ESCHER_PERVASIVE_BOX_H

namespace Escher {

class EditableField;

class PervasiveBox {
 public:
  PervasiveBox() : m_sender(nullptr) {}
  EditableField* sender() { return m_sender; }
  void setSender(EditableField* sender) { m_sender = sender; }
  virtual void open() = 0;

 private:
  EditableField* m_sender;
};

}  // namespace Escher

#endif
