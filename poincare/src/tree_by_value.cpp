#include <poincare/tree_by_value.h>

namespace Poincare {

/* Constructors */

TreeByValue::TreeByValue(const TreeByValue & tr) :
  TreeByValue(tr.clone()) {}

/* Operators */
TreeByValue& TreeByValue::operator=(const TreeByValue& tr) {
  TreeByReference t = tr.clone();
  m_identifier = t.identifier();
  return *this;
}

}
