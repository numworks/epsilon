#ifndef ESCHER_EDITABLE_FIELD_HELP_BOX_H
#define ESCHER_EDITABLE_FIELD_HELP_BOX_H

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

#endif
