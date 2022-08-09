#ifndef ESCHER_CELL_WITH_TEXTFIELD_H
#define ESCHER_CELL_WITH_TEXTFIELD_H

#include <escher/text_field.h>

namespace Escher {

class CellWithTextField {
public:
  virtual TextField * textField() = 0;
  virtual void reloadCell() = 0;
};

}

#endif
