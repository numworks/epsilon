#include "type_and_value_selector.h"
#include "combination.h"

namespace Poincare {
namespace Simplification {

bool TypeAndValueSelector::immediateMatch(const Expression * e) const {
  return TypeSelector::immediateMatch(e) && (m_value == e->checksum());
}

}
}
