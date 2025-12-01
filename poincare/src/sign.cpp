#include <poincare/sign.h>
#include <poincare/src/expression/properties.h>
#include <poincare/src/memory/tree.h>

namespace Poincare {

#if POINCARE_TREE_LOG
void Sign::log(std::ostream& stream, bool endOfLine) const {
  if (isNull()) {
    stream << "Zero";
  } else {
    if (isUnknown()) {
      stream << "Unknown";
    } else {
      if (m_canBeStrictlyPositive && m_canBeStrictlyNegative) {
        stream << "Non Null";
      } else {
        if (!m_canBeNull) {
          stream << "Strictly ";
        }
        stream << (m_canBeStrictlyNegative ? "Negative" : "Positive");
      }
    }
  }
  if (endOfLine) {
    stream << "\n";
  }
}
#endif

namespace Internal {

Sign GetSign(const Tree* e) { return GetProperties(e).sign(); }

}  // namespace Internal

}  // namespace Poincare
