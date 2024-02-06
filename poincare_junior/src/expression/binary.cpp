#include "binary.h"

#include <ion/unicode/utf8_helper.h>
#include <omgpj/unicode_helper.h>

namespace PoincareJ {

bool Binary::IsBinaryLogicalOperator(const CPL *name, int nameLength,
                                     BlockType *type) {
  for (int i = 0; i < k_numberOfOperators; i++) {
    if (OMG::CompareCPLWithNullTerminatedString(name, nameLength,
                                                k_operatorNames[i].name) == 0) {
      if (type) {
        *type = k_operatorNames[i].type;
      }
      return true;
    }
  }
  return false;
}

}  // namespace PoincareJ
