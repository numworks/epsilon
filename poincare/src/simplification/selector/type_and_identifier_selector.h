#ifndef POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_IDENTIFIER_SELECTOR_H
#define POINCARE_SIMPLIFICATION_SELECTOR_TYPE_AND_IDENTIFIER_SELECTOR_H

#include "type_selector.h"

namespace Poincare {
namespace Simplification {

class TypeAndIdentifierSelector : public TypeSelector {
public:
  constexpr TypeAndIdentifierSelector(Expression::Type type, int identifier, int captureIndex = -1, Selector ** children = nullptr, int numberOfChildren = 0, bool childrenPartialMatch = true) : TypeSelector(type, captureIndex, children, numberOfChildren, childrenPartialMatch), m_identifier(identifier) { }
  bool immediateMatch(const Expression * e) const override;
private:
  int m_identifier;
};

}
}

#endif
