#include "type_and_identifier_selector.h"
#include "combination.h"

namespace Poincare {
namespace Simplification {

bool TypeAndIdentifierSelector::immediateMatch(const Expression * e) const {
  return TypeSelector::immediateMatch(e) && (m_identifier == e->identifier());
}

}
}
