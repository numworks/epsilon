#ifndef ESCHER_FIELD_H
#define ESCHER_FIELD_H

#include <ion.h>

class Field {
public:
  virtual bool isEditing() const = 0;
  virtual void setEditing(bool isEditing, bool reinitDraftBuffer = true) = 0;
  virtual char XNTChar(char defaultXNTChar) = 0;
  virtual bool shouldFinishEditing(Ion::Events::Event event) = 0;
};

#endif
