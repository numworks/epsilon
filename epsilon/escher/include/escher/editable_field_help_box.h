#pragma once

namespace Escher {

class EditableField;

class EditableFieldHelpBox {
 public:
  EditableFieldHelpBox() : m_sender(nullptr) {}
  EditableField* sender() { return m_sender; }
  void setSender(EditableField* sender) { m_sender = sender; }
  virtual void open() = 0;

 private:
  EditableField* m_sender;
};

}  // namespace Escher
